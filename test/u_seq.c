
#include "umf.h"

int main(int argc, char *argv[])
{
  mf_seq *m;

  m = mf_seq_new("ss.mid", 384);

  if (m) {
    mf_seq_set_track(m, 2);
    mf_seq_evt (m, 0, st_note_on, 0, 60, 90);
    mf_seq_evt (m, 192, st_note_off, 0, 60, 0);
    mf_seq_evt (m, 192, st_note_on, 0, 64, 90);
    mf_seq_evt (m, 2*192, st_note_off, 0, 64, 0);

    mf_seq_set_track(m, 1);
    mf_seq_sys(m,0,st_meta_event,1,5,"ABCDE");

    mf_seq_evt (m, 0, st_note_on, 1, 64, 90);
    mf_seq_evt (m, 255, st_note_on, 1, 64, 0);

    mf_seq_close(m);
  }

  m = ms_new("zz.mid");
  if (m) {
    ms_close(m);    
  }
}

