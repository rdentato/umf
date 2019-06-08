/* 
**  (C) by Remo Dentato (rdentato@gmail.com)
**
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
**
**  *** DEBUG AND TESTING MACROS ***
**
**  dbgmsg(char *, ...)        --> Prints a message on stderr (works as printf(...)).
**                                 If DEBUG is not defined, do nothing.
**
**  dbgchk(test, char *, ...)  --> Perform the test. If test fails prints a message on
                                   stderr (works as printf(...)).
**                                 If DEBUG is not defined, do nothing.
**
**  _dbgmsg(char *, ...)       --> Do nothing. Used to disable the debug message.
**
**  _dbgchk(test, char *, ...) --> Do nothing. Used to disable the debug message.
**
**  Note that NDEBUG has precedence over DEBUG
*/

#ifndef __DBG_H__
#define __DBG_H__

#ifdef NDEBUG
#ifdef DEBUG
#undef DEBUG
#endif
#endif

#ifdef DEBUG
#include <stdio.h>
#define dbgmsg(...)   ((fflush(stdout), fprintf(stderr,__VA_ARGS__), \
                        fprintf(stderr," \x9%s:%d\n",__FILE__,__LINE__), \
                        fflush(stderr)))
#define dbg0(x,...)   (x)
#define dbgchk(e,...) do {int e_=!!(e); \
                          char *f_ = dbg0(__VA_ARGS__);\
                          fflush(stdout); /*Ensure dbg message appears after pending stdout prints */ \
                          fprintf(stderr,"%s: (%s) \x9%s:%d\n",(e_?"PASS":"FAIL"),#e,__FILE__,__LINE__); \
                          if (!e_ && f_ && *f_) {  \
                            fprintf(stderr,"    : " __VA_ARGS__); \
                          } \
                          fflush(stderr); \
                      } while(0)                      
#else
#define dbgmsg(...)
#define dbgchk(e,f,...)
#endif

#define _dbgmsg(...)
#define _dbgchk(e,f,...)

#endif // DBG_H

/*  ************ TESTS STATISTICS *************
**  Compile with:
**     cp dbg.h dbgstat.c; cc -DDBGSTAT -Wall -o dbgstat dbgstat.c
*/

#ifdef DBGSTAT
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  int num_fail = 0, num_pass = 0;
  char s[8]; char *p; int c;
  while ((p=fgets(s,7,stdin))) {
    if (strncmp(p,"PASS: ",6)==0)      num_pass++;
    else if (strncmp(p,"FAIL: ",6)==0) num_fail++;
    else while ((c = fgetc(stdin))!=EOF && c != '\n') ; 
  }
  printf("PASS: %d\nFAIL: %d\n",num_pass,num_fail);
  exit(num_fail != 0);
}
#endif // DBGSTAT

/*  ************ TEST SUITE *************
**  Compile with:
**     cp dbg.h dbgtest.c; cc -DDEBUG -DDBGTEST -Wall -o dbgtest dbgtest.c
*/

#ifdef DBGTEST
int main(int argc, char *argv[])
{
  int x;
  dbgmsg("Test %s (argc: %d)","message 1",argc);

  x=0;
  dbgmsg("Testing (1>x) with x=%d",x);
  dbgchk(1>x,"x=%d\n",x);

  x=1;
  dbgmsg("Testing (1>x) with x=%d",x);
  dbgchk(1>x,"x=%d\n",x);

  x=2;
  dbgmsg("Testing (1>x) with x=%d",x);
  dbgchk(1>x,"x=%d\n",x);

  x=2;
  dbgmsg("Testing (1>x) with x=%d(no message on fail)",x);
  dbgchk(1>x,"");
}

#endif // DBG_TEST

