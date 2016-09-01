
#include "umf.h"


int16_t main(int16_t argc, char *argv[])
{

  if (argc > 1) {
    mf_read(argv[1],NULL,NULL,NULL,NULL,NULL);
  }

  return 0;
}

