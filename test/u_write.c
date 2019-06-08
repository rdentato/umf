
#include "umf.h"


int main(int argc, char *argv[])
{
   mf_writer *m;
   int16_t ret = 999;

   if ((m = mf_new("xx.mid",192))) {  ret = 0; }

   if (!ret)  {ret = mf_track_start(m);}
   if (!ret)  {ret = mf_text(m, 0, "First");}
   if (!ret)  {ret = mf_midi_evt(m,  0,mf_st_note_on ,0,64,100);}
   if (!ret)  {ret = mf_midi_evt(m,192,mf_st_note_off,0,64,0);}

   if (!ret)  {ret = mf_midi_evt(m,  0,mf_st_note_on ,0,67,100);}
   if (!ret)  {ret = mf_midi_evt(m,192,mf_st_note_off,0,67,0);}
   if (!ret)  {ret = mf_track_end(m);}

   if (!ret)  {ret = mf_track_start(m);}
   if (!ret)  {ret = mf_copyright_notice(m, 0, "(C) by me");}
   if (!ret)  {ret = mf_midi_evt(m,  0,mf_st_note_on ,0,60,100);}
   if (!ret)  {ret = mf_midi_evt(m,384,mf_st_note_off,0,60,0);}
   if (!ret)  {ret = mf_track_end(m);}

   if (!ret)  {ret = mf_close(m);}

   if (ret)   { fprintf(stderr, "ERROR: %d\n",ret); }
   return ret;
}

