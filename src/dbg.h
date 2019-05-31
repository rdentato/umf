

#ifndef __DBG_H__
#define __DBG_H__

#ifdef NDEBUG
#ifdef DEBUG
#undef DEBUG
#endif
#endif

#ifdef DEBUG
#define dbgmsg(...)   ((fflush(stdout), \
                        fprintf(stderr,__VA_ARGS__), fflush(stderr)))
 
#define dbgchk(e,...) do {int e_=!!(e);
                          fflush(stdout);
                          fprintf(stderr,"%s: '%s' (%s:%d)\n",(e_?"PASS":"FAIL"),#e,__FILE__,__LINE__);
                          if (!e_) {
                            fprintf(stderr,"      ");
                            fprintf(stderr,__VA_ARGS__);
                            fprintf(stderr,"\n");
                          }
                      }
#else
#define dbgmsg(...)
#define dbgcheck(e,...)
#endif

#define _dbgmsg(...)
#define _dbgcheck(e,...)
#endif

