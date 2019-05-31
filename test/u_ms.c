
#include "umf.h"

int main(int argc, char *argv[])
{
  mf_seq *m;

  m = ms_new("ss.mid");

  if (m) {
    ms_track(m,1);
    ms_note(m,60,ms_quarter_n);
    ms_rest(m);
    ms_note(m,62);

    ms_track(m,0);
    ms_rest(m,ms_half_n);
    ms_note(m,57,ms_quarter_n);

    ms_track(m,1);
    ms_note(m,64);

    ms_setmark(m);
    ms_note(m,64);

    ms_close(m);
  }
}

