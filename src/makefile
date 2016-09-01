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
RM=rm -f
 
CFLAGS = -DDEBUG -g	
CFLAGS = 

LIBOBJ=umf.o

.c.o:
	$(CC) $(CFLAGS) -c -o $*.o $*.c


libumf.a : $(LIBOBJ) umf.h
	$(AR) $@ $(LIBOBJ)


clean:
	$(RM) libumf.a *.log *.o



