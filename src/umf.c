/*
**  (C) Remo Dentato (rdentato@gmail.com)
**
** Permission to use, copy, modify and distribute this code and
** its documentation for any purpose is hereby granted without
** fee, provided that the above copyright notice, or equivalent
** attribution acknowledgement, appears in all copies and
** supporting documentation.
**
** Copyright holder makes no representations about the suitability
** of this software for any purpose. It is provided "as is" without
** express or implied warranty.
*/

#include "umf.h"
#include "utl.h"


#define MThd 0x4d546864
#define MTrk 0x4d54726b

/* *********************************************************
     ooooooooo.   oooooooooooo       .o.       oooooooooo.
     `888   `Y88. `888'     `8      .888.      `888'   `Y8b
      888   .d88'  888             .8"888.      888      888
      888ooo88P'   888oooo8       .8' `888.     888      888
      888`88b.     888    "      .88ooo8888.    888      888
      888  `88b.   888       o  .8'     `888.   888     d88'
     o888o  o888o o888ooooood8 o88o     o8888o o888bood8P'
** *********************************************************/


/* Get the number of parameters needed by a channel message.
** s is the status byte.
*/

int16_t mf_numparms(int16_t s)  { return ("\2\2\2\2\1\1\2"[((s) & 0x70)>>4]); }

/********************************************************************/

/* == Reading Values
**
**  readnum(n)  reads n bytes and assembles them to create an integer
**              if n is 0, reads a variable length representation
*/

static int32_t readvar(mf_reader *mfile)
{
  int32_t v = 0;
  int16_t c;

  if ((c = fgetc(mfile->file)) == EOF) return -1;

  while (c & 0x80 ) {
    v = (v << 7) | (c & 0x7f);
    if ((c = fgetc(mfile->file)) == EOF) return -1;
  }
  v = (v << 7) | c;
  return (v);
}

static int32_t readnum(mf_reader *mfile, int16_t k)
{
  int32_t x = 0, v = 0;

  if (k == 0) return(readvar(mfile));

  while (k-- > 0) {
    if ((x = fgetc(mfile->file)) == EOF) return -1;
    v = (v << 8) | x;
  }
  return v;
}

/* === Read messages
**   readmsg(n)  reads n bytes, stores them in a buffer end returns
**               a pointer to the buffer;
*/

static uint8_t *chrbuf_set(mf_reader *mfile, int32_t sz)
{
  uint8_t *t = NULL;
  if (sz > mfile->chrbuf_sz) {
    t = realloc(mfile->chrbuf, sz);
    if (t) {
      mfile->chrbuf    = t;
      mfile->chrbuf_sz = sz;
    }
  }
  return mfile->chrbuf;
}

static uint8_t *readmsg(mf_reader *mfile, int32_t n)
{
  int16_t   c;
  uint8_t *s;

  if (n == 0) return (uint8_t *)"";

  chrbuf_set(mfile, n);
  if (mfile->chrbuf_sz < n) return NULL;

  s = mfile->chrbuf;
  while (n-- > 0) {   /*** Read the message ***/
    if ((c = fgetc(mfile->file)) == EOF) return NULL;
    *s++ = c;
  }

  return mfile->chrbuf;
}

/*
** This is the FSM used to scan the midi file

                           .-------.
                           v        \
    mthd -> mtrk -----> event -> midi_evt
            ^    .------^ / \
            |   /        /   |
            \  /        /    v
           sys_evt <---'   meta_evt  
              ^              /
              |             /
              '------------'      
*/

int16_t mf_scan(mf_reader *mfile)
{
  int32_t tmp;
  int32_t v1, v2;
  int16_t ERROR = 0;
  int32_t track_time;
  int32_t tracklen;
  int32_t ntracks;
  int32_t curtrack = 0;
  int32_t status = 0;
  uint8_t *msg;
  int32_t chan;

  fsm {
    fsmSTATE(mthd) {
      if (readnum(mfile, 4) != MThd) {ERROR = 110; fsmGOTO(fail);};
      tmp = readnum(mfile, 4); /* chunk length */
      if (tmp < 6) {ERROR = 111; fsmGOTO(fail);};
      v1 = readnum(mfile,2);
      ntracks = readnum(mfile,2);
      v2 = readnum(mfile,2);
      ERROR = mfile->on_header(v1, ntracks, v2);
      if (ERROR) fsmGOTO(fail);
      if (tmp > 6) readnum(mfile,tmp-6);
      fsmGOTO(mtrk);
    }
    
    fsmSTATE(mtrk) {
      if (curtrack++ == ntracks) fsmGOTO(end);
      if (readnum(mfile,4) != MTrk) {ERROR=120; fsmGOTO(fail); }
      tracklen = readnum(mfile,4);
      if (tracklen < 0) {ERROR=121; fsmGOTO(fail); }
      track_time = 0;
      status = 0;
      ERROR = mfile->on_track(0, curtrack, tracklen);
      if (ERROR) fsmGOTO(fail);
      fsmGOTO(event);
    }
    
    fsmSTATE(event) {
      tmp = readnum(mfile,0); if (tmp < 0) {ERROR=211; fsmGOTO(fail); }
      track_time += tmp;
    
      tmp = readnum(mfile,1); if (tmp < 0) {ERROR=212; fsmGOTO(fail); }
    
      if ((tmp & 0x80) == 0) {
        if (status == 0) {ERROR=223; fsmGOTO(fail); } /* running status not allowed! */
        fsmGOTO(midi_evt);
      }
    
      status = tmp;
      v1 = -1;
      if (status == 0xFF) fsmGOTO(meta_evt);
      if (status == 0xF0) fsmGOTO(sys_evt);
      if (status == 0xF7) fsmGOTO(sys_evt);
      if (status >  0xF0) {ERROR=543; fsmGOTO(fail); }
      tmp = readnum(mfile,1);
      fsmGOTO(midi_evt);
    }
    
    fsmSTATE(midi_evt) {
      chan = 1+(status & 0x0F);
      v1 = tmp;
      v2 = -1;
      if (mf_numparms(status) == 2) {
        v2 = readnum(mfile,1);
        if (v2 < 0) {ERROR=212; fsmGOTO(fail); }
      }
      ERROR = mfile->on_midi_evt(track_time, status & 0xF0, chan, v1, v2);
      if (ERROR) fsmGOTO(fail);
    
      fsmGOTO(event);
    }
    
    fsmSTATE(meta_evt) {
      v1 = readnum(mfile,1);
      if (v1 < 0) {ERROR=214; fsmGOTO(fail); }
      fsmGOTO(sys_evt);
    }
    
    fsmSTATE(sys_evt) {
      v2 = readnum(mfile,0);
      if (v2 < 0) {ERROR=215; fsmGOTO(fail); }
    
      msg = readmsg(mfile,v2);
      if (msg == NULL) {ERROR=216; fsmGOTO(fail); }
    
      if (v1 == me_end_of_track) {
        ERROR = mfile->on_track(1, curtrack, track_time);
        if (ERROR) fsmGOTO(fail); 
        fsmGOTO(mtrk);
      }
      ERROR = mfile->on_sys_evt(track_time, status, v1, v2, msg);
      if (ERROR) fsmGOTO(fail); 
      status = 0;
      fsmGOTO(event);
    }
    
    fsmSTATE(fail) {
      if (ERROR < 0) ERROR = -ERROR;
      mfile->on_error(ERROR, NULL);
      fsmGOTO(end);
    }
    
    fsmSTATE(end) {
      return ERROR;
    }
  }  
}


/*************************************************************/

static int16_t mf_dmp_header (int16_t type, int16_t ntracks, int16_t division)
{
  printf("HEADER: %u, %u, %u\n", type, ntracks, division);
  return 0;
}

static int16_t mf_dmp_track (int16_t eot, int16_t tracknum, uint32_t tracklen)
{
  printf("TRACK %s: %d (%lu %s)\n", eot?"END":"START", tracknum, tracklen,eot?"ticks":"bytes");
  return 0;
}

static int16_t mf_dmp_midi_evt(uint32_t tick, int16_t type, int16_t chan,
                                                  int16_t data1, int16_t data2)
{
  printf("%8ld %02X %02X %02X", tick, type, chan, data1);
  if (data2 >= 0) printf(" %02X", data2);  /* data2 < 0 means there's no data2! */
  printf("\n");
  return 0;
}

static int16_t mf_dmp_sys_evt(uint32_t tick, int16_t type, int16_t aux,
                                               int32_t len, uint8_t *data)
{
  printf("%8ld %02X ", tick, type);
  if (aux >= 0) printf("%02X ", aux);
  printf("%04lX ", (uint32_t)len);
  type = (type == 0xFF && (0x01 <= aux && aux <= 0x09));
  if (type) {  while (len-- > 0) printf("%c", *data++);   }  /* ASCII */
  else      {  while (len-- > 0) printf("%02X", *data++); }  /* DATA */
  printf("\n");

  return 0;
}

static int16_t mf_dmp_error(int16_t err, char *msg)
{
  if (msg == NULL) msg = "";
  fprintf(stderr, "Error %03d - %s\n", err, msg);
  return err;
};

/*************************************************************/

mf_reader *mf_reader_new(char  *fname)
{
  mf_reader *mr = NULL;
  FILE      *f  = NULL;

  f = fopen(fname,"rb");
  if (f) {
    mr = malloc(sizeof(mf_reader));
    if (mr) {
      mr->file = f;

      mr->on_error    = mf_dmp_error    ;
      mr->on_header   = mf_dmp_header   ;
      mr->on_track    = mf_dmp_track    ;
      mr->on_midi_evt = mf_dmp_midi_evt ;
      mr->on_sys_evt  = mf_dmp_sys_evt  ;

      mr->chrbuf      = NULL;
      mr->chrbuf_sz   = 0;

      mr->aux = NULL;
    }
  }
  return mr;
}

void mf_reader_close(mf_reader *mr)
{
  if (mr) {
    if (mr->file)   fclose(mr->file);
    if (mr->chrbuf) free(mr->chrbuf);
    free(mr);
  }
}

int16_t mf_read( char          *fname      , mf_fn_error   fn_error   ,
                 mf_fn_header   fn_header  , mf_fn_track   fn_track   ,
                 mf_fn_midi_evt fn_midi_evt, mf_fn_sys_evt fn_sys_evt  )
{
  int16_t ret = 0;
  mf_reader  *mr;

  mr = mf_reader_new(fname);

  if (!mr) return 79;

  if (fn_error)    mr->on_error    = fn_error;
  if (fn_header)   mr->on_header   = fn_header;
  if (fn_track)    mr->on_track    = fn_track;
  if (fn_midi_evt) mr->on_midi_evt = fn_midi_evt;
  if (fn_sys_evt)  mr->on_sys_evt  = fn_sys_evt;

  ret = mf_scan(mr);

  mf_reader_close(mr);

  return ret;
}


/* ****************************************************************************
     oooooo   oooooo     oooo ooooooooo.   ooooo ooooooooooooo oooooooooooo
      `888.    `888.     .8'  `888   `Y88. `888' 8'   888   `8 `888'     `8
       `888.   .8888.   .8'    888   .d88'  888       888       888
        `888  .8'`888. .8'     888ooo88P'   888       888       888oooo8
         `888.8'  `888.8'      888`88b.     888       888       888    "
          `888'    `888'       888  `88b.   888       888       888       o
           `8'      `8'       o888o  o888o o888o     o888o     o888ooooood8
** ***************************************************************************/
#define eputc(c)  (mw->trk_len++, fputc(c,mw->file))

static void f_write8(mf_writer *mw, uint8_t n)    {eputc(n);}

static void f_write7(mf_writer *mw, uint8_t n)    {eputc(n & 0x7F);}

static void f_write16(mf_writer *mw, uint16_t n)  {eputc(n >> 8  ); eputc(n & 0xFF);}

static void f_write32(mf_writer *mw, uint32_t n)  {eputc((n >> 24)       );  eputc((n >> 16) & 0xFF);
                                                   eputc((n >>  8) & 0xFF);  eputc((n      ) & 0xFF); }

static void f_writemsg(mf_writer *mw,
                     int32_t len, uint8_t *data)  {while(len-- > 0) eputc(*data++);}

static void f_writevar(mf_writer *mw, uint32_t n)
{
  uint32_t buf;

  n &= 0x0FFFFFFF;
  _logdebug("vardata: %08lX -> ", n);

  buf = n & 0x7F;
  while ((n >>= 7) != 0) {
    buf = (buf << 8) | (n & 0x7F) | 0x80;
  }
  _logdebug("%08lX\n", buf);
  while (1) {
    eputc(buf & 0xFF);
    if ((buf & 0x80) == 0) break;
    buf >>= 8;
  }
}

#if 0  /* Not needed */
static void f_write14(mf_writer *mw, uint16_t n)
{
  eputc(n >> 7  );
  eputc(n & 0x7F);
}
#endif


mf_writer *mf_new(char *fname, int16_t division)
{
  mf_writer *mw = NULL;

  mw = malloc(sizeof(mf_writer));
  if (!mw) return NULL;

  mw->file = fopen(fname, "wb");

  if (!mw->file) {  free (mw); return NULL; }

  mw->type    = mf_type_file;
  mw->len_pos = 0;
  mw->trk_len = 0;
  mw->trk_cnt = 0;
  mw->trk_in  = 0;
  mw->division  = division;

  /* Write Header chunk (to be rewrite at the end) */
  f_write32(mw, MThd);
  f_write32(mw, 6);
  f_write16(mw, 0);
  f_write16(mw, 1);
  f_write16(mw, division);

  return mw;
}

int16_t mf_track_start (mf_writer *mw)
{
  if (!mw || !mw->file ) { return 309; }

  if (mw->trk_in) mf_track_end(mw);

  mw->trk_cnt++;
  mw->trk_in  = 1;
  mw->chan    = 0;

  f_write32(mw, MTrk);

  /* Save current position for later */
  if ((mw->len_pos = ftell(mw->file)) <  0) { return 301; }

  f_write32(mw, 0);  /* just a place-holder for now */

  mw->trk_len = 0;
  return 0;
}

int16_t mf_track_end(mf_writer *mw)
{
  uint32_t pos_cur;

  if (!mw || !mw->file || !mw->trk_in) { return 329; }

  f_writevar(mw, 0);  f_write8(mw, 0xFF);  f_write8(mw, 0x2F);  f_write8(mw, 0x00);

  if ((pos_cur = ftell(mw->file)) <  0) {  return 322; }

  if (fseek(mw->file, mw->len_pos, SEEK_SET) < 0) {  return 323; }

  f_write32(mw, mw->trk_len);

  if (fseek(mw->file, pos_cur, SEEK_SET) < 0) { return 324; }
  mw->trk_in = 0;
  return 0;
}

int16_t mf_midi_evt (mf_writer *mw, uint32_t delta, int16_t type, int16_t chan,
                                                    int16_t data1, int16_t data2)
{
  uint8_t st;

  if (!mw || !mw->file || !mw->trk_in) { return 319; }

  st = (type & 0xF0);

  if (st == st_system_exclusive)  {return 318; }  /* No sysex accepted here! */

  if (st == st_note_on && data2 == 0) st = st_note_off;

  f_writevar(mw, delta);
  f_write8(mw, st | (chan & 0x0F));
  f_write7(mw, data1);
  if (mf_numparms(st) > 1)  f_write7(mw, data2);
  mw->chan = chan;
  return 0;
}


int16_t mf_sys_evt(mf_writer *mw, uint32_t delta,
                              int16_t type, int16_t aux,
                              int32_t len, uint8_t *data)
{
  if (!mw || !mw->file || !mw->trk_in) { return 349; }

  f_writevar(mw, delta);
  f_write8(mw, type);
  if (type == st_meta_event) f_write8(mw, aux);
  f_writevar(mw, len);
  f_writemsg(mw, len, data );

  return 0;
}

int16_t mf_text_evt(mf_writer *mw, uint32_t delta, int16_t type, char *str)
{
  return mf_sys_evt(mw, delta, st_meta_event, type & 0x0F, strlen(str), (uint8_t *)str);
}

int16_t mf_close (mf_writer *mw)
{
  int16_t ret = 0;
  int16_t format = 0;
  uint32_t pos_cur;

  if (!mw || !mw->file) { return 399; }

  if (mw->trk_in) ret = mf_track_end(mw);

  if (mw->trk_cnt > 1) format = 1;

  if ((pos_cur = ftell(mw->file)) <  0) {  return 392; }

  if (fseek(mw->file, 0, SEEK_SET) < 0) {  return 393; }

  f_write32(mw, MThd);
  f_write32(mw, 6);
  f_write16(mw, format);
  f_write16(mw, mw->trk_cnt);
  f_write16(mw, mw->division);

  if (fseek(mw->file, pos_cur, SEEK_SET) < 0) { return 394; }

  fclose(mw->file);
  free(mw);

  return ret;
}

int16_t mf_pitch_bend(mf_writer *mw, uint32_t delta, uint8_t chan, int16_t bend)
{/* bend is in the range  -8192 .. 8191 */

  if (bend < -8192) bend = -8192;
  if (bend >  8191) bend =  8191;

  bend += 8192;

  return mf_midi_evt(mw, delta, st_pitch_bend, chan, bend, bend  >> 7);
}

int16_t mf_set_tempo(mf_writer *mw, uint32_t delta, int32_t tempo)
{
  uint8_t buf[4];

  buf[0] = (tempo >> 16) & 0xFF;
  buf[1] = (tempo >>  8) & 0xFF;
  buf[2] = (tempo      ) & 0xFF;

  return mf_sys_evt(mw, delta, st_meta_event, me_set_tempo, 3, buf);
}

int16_t mf_set_keysig(mf_writer *mw, uint32_t delta, int16_t acc, int16_t mi)
{
  uint8_t buf[4];

  buf[0] = (uint8_t)(acc & 0xFF);
  buf[1] = (uint8_t)(mi  & 0x01);
  return mf_sys_evt(mw, delta, st_meta_event, me_key_signature, 2, buf);
}



/* ******************************************* **
       .oooooo..o oooooooooooo   .oooooo.     
      d8P'    `Y8 `888'     `8  d8P'  `Y8b    
      Y88bo.       888         888      888   
       `"Y8888o.   888oooo8    888      888   
           `"Y88b  888    "    888      888   
      oo     .d8P  888       o `88b    d88b   
      8""88888P'  o888ooooood8  `Y8bood8P'Ybd'
** ******************************************* */


mf_seq *mf_seq_new (char *fname, uint16_t division)
{
  int16_t k;
  mf_seq *ms = NULL;

  ms = malloc(sizeof(mf_seq));

  if (ms) {
    ms->type     = mf_type_seq;
    ms->flags    = 0;
    
    ms->buf = NULL; ms->buf_cnt = 0; ms->buf_max = 0;
    ms->evt = NULL; ms->evt_cnt = 0; ms->evt_max = 0;

    ms->fname    = fname;
    if (division == 0) division = (2*2*2*2)*(3*3)*5*7; /* 5040 */
    ms->division = division;
    ms->curtrack = 0;

  }
  return ms;
}

#define getlong(q)  ((q)[0] << 24 | (q)[1] << 16 | (q)[2] << 8 | (q)[3])

#if 0
static void dmp_evts(mf_seq *ms)
{
  uint32_t k;
  uint8_t *p;
  int32_t l;

  if (!ms) return;


  for (k=0; k< ms->evt_cnt; k++) {
    p = ms->evt[k].p;

    printf("%02X %02X%02X%02X%02X ", p[0], p[1],p[2],p[3],p[4]);
    printf("%02X %02X ", p[5], p[6]);

    if (p[5] < 0xF0) {
      printf("%02X %02X", p[7], p[8]);
    } else {
      l = getlong(p+7);
      printf("%08X ", l);
      p = p+11;
      /*printf("** %d **",l);*/
       while (l--) printf("%02X", *p++);
    }
    printf("\n");
  }
}
#endif

      /* Status Byte:   89ABCDEF */
static char *evt_ord = "71023456";
      /* Sort order (NoteOff is always first!) */

#define evt_cmp_st(x) (evt_ord[((x)>>4) & 0x07])

static int evt_cmp(const void *a, const void *b)
{
  int      ret = 0;
  uint8_t *pa = ((mf_evt *)a)->p;
  uint8_t *pb = ((mf_evt *)b)->p;

  /* This works only because we represented ticks in a special way*/
  if (!(ret = pa[0]-pb[0]))
  if (!(ret = pa[1]-pb[1]))
  if (!(ret = pa[2]-pb[2]))
  if (!(ret = pa[3]-pb[3]))
  if (!(ret = pa[4]-pb[4])) {
    ret = evt_cmp_st(pb[5]) - evt_cmp_st(pa[5]);  
  }
  
  return ret;
}

int16_t mf_seq_close(mf_seq *ms)
{
  int16_t k;
  int16_t trk = -1;
  uint32_t tick=0;
  uint32_t delta;
  uint32_t nxtk;
  uint8_t *p;

  mf_writer *mw;

  if (!ms) return 799;

  /* Convert the offset into pointers */
  for (k=0; k< ms->evt_cnt; k++) {
    ms->evt[k].p = ms->buf + ms->evt[k].l;
  }

  /*
  logdebug("Events: %d\n", ms->evt_cnt);
  dmp_evts(ms);
  */

  qsort(ms->evt, ms->evt_cnt,sizeof(mf_evt), evt_cmp);

  /*
  logdebug("Events: %d\n", ms->evt_cnt);
  dmp_evts(ms);
  */

  mw = mf_new(ms->fname, ms->division);

  if (mw) {

    if (ms->evt_cnt == 0) {
         mf_track_start(mw);
         mf_sys_evt(mw, 0, st_meta_event, me_text, 5, (uint8_t *)"Empty");
    }
    else for (k=0; k< ms->evt_cnt; k++) {
       p = ms->evt[k].p;

       if ((uint8_t)(*p) != trk) {  /* Start a new track */
         mf_track_start(mw);
         trk = *p;
         tick = 0;
       }

       nxtk = getlong(p+1);
       delta = nxtk - tick;
       _logdebug("DELTA: (%d-%d) = %d\n", nxtk,tick,delta);
       tick = nxtk;

       if (p[5] < 0xF0) {
         mf_midi_evt(mw, delta, p[5], p[6], p[7],p[8]);
       } else {
         mf_sys_evt(mw, delta, p[5], p[6], getlong(p+7), p+11);
       }
    }

    mf_close(mw);
  }

  /* Clean up */
  if (ms->buf) free(ms->buf);
  if (ms->evt) free(ms->evt);
  free(ms);

  return 0;
}

static int16_t chkbuf(mf_seq *ms, uint32_t spc)
{
   uint32_t newsize;
   uint8_t *buf;

   if (!ms) return 739;
   if (spc == 0) return 0;

   newsize = ms->buf_max;
   if (newsize == 0) newsize = spc+1;
   
   _logdebug("CHKBUF(: buf:%p cnt:%d max:%d need:%d\n", ms->buf, ms->buf_cnt, ms->buf_max,spc);

   while (spc >= (newsize - ms->buf_cnt))
      newsize += (newsize /2);

   if (newsize > ms->buf_max) {
      buf = realloc(ms->buf, newsize);
      if (!buf) return 730;
      ms->buf = buf;
      ms->buf_max = newsize;
   }
   _logdebug("CHKBUF): buf:%p cnt:%d max:%d need:%d\n", ms->buf, ms->buf_cnt, ms->buf_max,spc);

   return 0;
}

static int16_t chkevt(mf_seq *ms, uint32_t n)
{
   uint32_t newsize;
   mf_evt *evt = NULL;

   if (!ms) return 749;
   if (n == 0) return 0;

   _logdebug("CHKEVT(: evt:%p cnt:%d max:%d need:%d\n", ms->evt, ms->evt_cnt, ms->evt_max,n);
   newsize = ms->evt_max;
   if (newsize == 0) newsize = n+1;
   
   while (n >= (newsize - ms->evt_cnt))
      newsize += newsize/2;

   if (newsize > ms->evt_max) {
      evt = realloc(ms->evt, newsize * sizeof(mf_evt));
      if (!evt) return 740;
      ms->evt = evt;
      ms->evt_max = newsize;
   }

   _logdebug("CHKEVT): evt:%p cnt:%d max:%d need:%d\n", ms->evt, ms->evt_cnt, ms->evt_max,n);
   return 0;
}

int16_t mf_seq_set_track(mf_seq *ms, int16_t track)
{
  if (!ms) return 719;
  ms->curtrack = track & 0xFF;
  return 0;
}

int16_t mf_seq_get_track(mf_seq *ms, int16_t track)
{
  if (!ms) return 789;
  return ms->curtrack;
}

#define add_evt(ms)    (ms->evt[ms->evt_cnt++].l = ms->buf_cnt)
#define add_byte(ms,b) (ms->buf[ms->buf_cnt++] = (uint8_t)(b))

static void add_data(mf_seq *ms, int32_t l, uint8_t *d)
{  if (ms) while (l--) add_byte(ms,*d++); }

/* this forces the 32 bit number 0xFE12AB34 to be stored as a sequence of four
   bytes {0xFE,0x12,0xAB,0x23} rather than according the CPU representations.
   This is used when sorting the array of the events. */
static void add_ulong(mf_seq *ms, uint32_t l)
{
  add_byte(ms,(l >>24) & 0xFF);
  add_byte(ms,(l >>16) & 0xFF);
  add_byte(ms,(l >> 8) & 0xFF);
  add_byte(ms,(l     ) & 0xFF);
}

/*
static void add_str(mf_seq *ms, char *s)
{
  add_data(ms,strlen(s),(uint8_t *)s);
}
*/

int16_t mf_seq_evt (mf_seq *ms, uint32_t tick, uint16_t type, uint16_t chan, uint16_t data1, uint16_t data2)
{
  int16_t ret = 0;

  type &= 0xF0;
  _logdebug("SEQ EVT\n");

  if (!ms)  ret = 759;
  if (!ret) ret = chkbuf(ms,32);
  if (!ret) ret = chkevt(ms,1);
  if (!ret) ret = type == 0xF0 ? 758 : 0;  /* no meta! */
  
  if (!ret) {
    add_evt(ms);

    add_byte(ms, ms->curtrack);
    add_ulong(ms,tick);

    chan  &= 0x0F;
    data1 &= 0xFF;
    data2 &= 0xFF;

    /* Force the NoteOff status when NoteOn and Vel=0 */
    if (type == st_note_on && data2 == 0) type = st_note_off;

    add_byte(ms, type );
    add_byte(ms, chan );
    add_byte(ms, data1);
    add_byte(ms, data2);
  }
  return ret;
}

int16_t mf_seq_sys(mf_seq *ms, uint32_t tick, uint16_t type, uint16_t aux,
                                               int32_t len, uint8_t *data)
{
  int16_t ret = 0;

  if (!ms)  ret = 779;
  if (!ret) ret = chkbuf(ms,32+len);
  if (!ret) ret = chkevt(ms,1);
  if (!ret) ret = (type >= 0xF0) ? 0 : 778;
  if (!ret) {
    _logdebug("SEQSYS: %d %d\n",ms->curtrack, type);
    add_evt(ms);

    add_byte(ms, ms->curtrack);
    add_ulong(ms,tick);

    add_byte(ms,type);
    add_byte(ms,aux);

    add_ulong(ms,len);
    add_data(ms,len,data);
  }

  return ret;
}

int16_t mf_seq_text(mf_seq *ms, uint32_t tick, uint16_t type, char *txt)
{
  return mf_seq_sys(ms, tick, st_meta_event, type & 0x0F, strlen(txt), (uint8_t *)txt);
}

int16_t mf_seq_pitch_bend(mf_seq *ms, uint32_t tick, uint8_t chan, int16_t bend)
{/* bend is in the range  -8192 .. 8191 */

  if (bend < -8192) bend = -8192;
  if (bend >  8191) bend =  8191;

  bend += 8192;

  return mf_seq_evt(ms, tick, st_pitch_bend, chan, bend, bend  >> 7);
}

int16_t mf_seq_set_tempo(mf_seq *ms, uint32_t tick, int32_t tempo)
{
  uint8_t buf[4];

  buf[0] = (tempo >> 16) & 0xFF;
  buf[1] = (tempo >>  8) & 0xFF;
  buf[2] = (tempo      ) & 0xFF;

  return mf_seq_sys(ms, tick, st_meta_event, me_set_tempo, 3, buf);
}

int16_t mf_seq_set_keysig(mf_seq *ms, uint32_t tick, int16_t acc, int16_t mi)
{
  uint8_t buf[4];

  buf[0] = (uint8_t)(acc & 0xFF);
  buf[1] = (uint8_t)(mi  & 0x01);
  return mf_seq_sys(ms, tick, st_meta_event, me_key_signature, 2, buf);
}


