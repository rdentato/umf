# 
#  (C) by Remo Dentato (rdentato@gmail.com)
# 
# This software is distributed under the terms of the BSD license:
#   http://creativecommons.org/licenses/BSD/
#   http://opensource.org/licenses/bsd-license.php
#

# This makefile is for GNU tool chain 


_EXE=

ifeq ($(SYS),MINGW)
_EXE=.exe
endif

CC=gcc
AR=ar -ru
LN=gcc $(LIBPATH)
 
RM=rm -f
 
CFLAGS = -DDEBUG -g	
CFLAGS = -DDEBUG

INCPATH =-I../src
LIBPATH =-L../src

.c.o:
	$(CC) $(CFLAGS) $(INCPATH) -c -o $*.o $*.c


all: t_seq$(_EXE) t_write$(_EXE) t_read$(_EXE)
  
t_seq$(_EXE): u_seq.o
	$(LN) -o $@ u_seq.o -lumf

t_write$(_EXE): u_write.o
	$(LN) -o $@ u_write.o -lumf
  
t_read$(_EXE): u_read.o
	$(LN) -o $@ u_read.o -lumf

clean:
	$(RM) *.log *.o ss.mid xx.mid
	$(RM) t_*
	$(RM) gmon.out



