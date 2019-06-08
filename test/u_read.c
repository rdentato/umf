
#include "umf.h"


int main(int argc, char *argv[])
{

  if (argc > 1) {
    mf_read(argv[1],NULL,NULL,NULL,NULL,NULL);
  }

  return 0;
}

