/* 
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
*/

#include "umf.h"
#include "dbg.h"

int main(int argc, char *argv[])
{
  mf_seq *m;
  int d;

  m = ms_new("ss.mid",960);
  dbgchk(m!=NULL,"");

  if (m) {
    d = mf_pitch_str("C4");
    dbgchk(60==d,"C4 = %d",d);
  
    ms_track(m,1);
    ms_note(m,mf_pitch_str("C4"),mf_quarter_n(m));
    ms_rest(m);
    ms_note(m,62);

    ms_track(m,0);
    ms_rest(m,mf_half_n(m));
    ms_note(m,57,mf_quarter_n(m));

    ms_track(m,1);
    ms_note(m,64);

    ms_setmark(m);
    dbgmsg("Mark: %u %u",m->curtick[m->curtrack],ms_getmark(m));
    ms_note(m,64);

    ms_track(m,0);
    ms_rest(m,mf_half_n(m));
    ms_note(m,57,mf_whole_n(m));
    mf_seq_set_keysig(m,m->curtick[m->curtrack],2,1);
    
    ms_note(m,59,mf_sixteenth_n(m));

    ms_track(m,2);
    d = mf_tuplet_n(3,2,mf_eigth_n(m));
    ms_note(m,66,d);
    ms_note(m,70,d);
    ms_note(m,68,d);

    ms_close(m);
  }
  exit(0);
}

