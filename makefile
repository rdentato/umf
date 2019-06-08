## (C) Remo Dentato (rdentato@gmail.com)
## UMF is distributed under the terms of the MIT License
## as detailed in the 'LICENSE' file.

# This makefile is for GNU tool chain 

_EXE=.exe
ifeq "$(COMSPEC)" ""
_EXE=
endif

CC=gcc
AR=ar -ru
RM=rm -f
LN=gcc $(LIBPATH)

CFLAGS = -DDEBUG -g	-Wall
# RELEASE Flags
#CFLAGS = -O2 -DNDEBUG -Wall

LIBOBJ=src/umf.o

INCPATH =-I./src
LIBPATH =-L./src

TST=test/t_seq$(_EXE) test/t_write$(_EXE) test/t_read$(_EXE) test/t_ms$(_EXE)
LIB=src/libumf.a

.c.o:
	$(CC) $(CFLAGS) $(INCPATH) -c -o $*.o $*.c

#        .o.       ooooo        ooooo        
#       .888.      `888'        `888'        
#      .8"888.      888          888         
#     .8' `888.     888          888         
#    .88ooo8888.    888          888         
#   .8'     `888.   888       o  888       o 
#  o88o     o8888o o888ooooood8 o888ooooood8 

all: $(SRC) $(TST) 

#   .oooooo..o ooooooooo.     .oooooo.   
#  d8P'    `Y8 `888   `Y88.  d8P'  `Y8b  
#  Y88bo.       888   .d88' 888          
#   `"Y8888o.   888ooo88P'  888          
#       `"Y88b  888`88b.    888          
#  oo     .d8P  888  `88b.  `88b    ooo  
#  8""88888P'  o888o  o888o  `Y8bood8P'  

src/umf.o: src/umf.h src/umf.c
	$(CC) $(CFLAGS_SRC) $(INCPATH) -c -o $*.o $*.c

src/libumf.a : src/umf.o src/umf.h
	$(AR) $@ $(LIBOBJ)

#  ooooooooooooo oooooooooooo  .oooooo..o ooooooooooooo 
#  8'   888   `8 `888'     `8 d8P'    `Y8 8'   888   `8 
#       888       888         Y88bo.           888      
#       888       888oooo8     `"Y8888o.       888      
#       888       888    "         `"Y88b      888      
#       888       888       o oo     .d8P      888      
#      o888o     o888ooooood8 8""88888P'      o888o                                                         

test_prg=test/t_ms$(_EXE) test/t_write$(_EXE) \
         test/t_seq$(_EXE) test/t_read$(_EXE)

test/test.log: test/dbgstat$(_EXE) $(test_prg)
	@date +"DATE: %Y/%m/%d %H:%M:%S" > test/test.log
	@echo "Running tests"
	@cd test ; for f in t_*; do ./$$f 2>> test.log; done; ./dbgstat < test.log;

runtest: test/test.log 

test/t_ms$(_EXE): src/libumf.a test/u_ms.o
	$(LN) -o $@ test/u_ms.o -lumf

test/t_seq$(_EXE): src/libumf.a test/u_seq.o
	$(LN) -o $@ test/u_seq.o -lumf

test/t_write$(_EXE): src/libumf.a test/u_write.o
	$(LN) -o $@ test/u_write.o -lumf
  
test/t_read$(_EXE): src/libumf.a test/u_read.o
	$(LN) -o $@ test/u_read.o -lumf

test/dbgstat$(_EXE): src/dbg.h
	cp src/dbg.h test/dbgstat.c
	$(CC) -o test/dbgstat -O2 -Wall -DDBGSTAT test/dbgstat.c
	rm -f test/dbgstat.c

#  oooooooooo.     .oooooo.     .oooooo.   
#  `888'   `Y8b   d8P'  `Y8b   d8P'  `Y8b  
#   888      888 888      888 888          
#   888      888 888      888 888          
#   888      888 888      888 888          
#   888     d88' `88b    d88' `88b    ooo  
#  o888bood8P'    `Y8bood8P'   `Y8bood8P'  
                                        
doc:
	cd doc; make
 
#    .oooooo.   ooooo        oooooooooooo       .o.       ooooo      ooo 
#   d8P'  `Y8b  `888'        `888'     `8      .888.      `888b.     `8' 
#  888           888          888             .8"888.      8 `88b.    8  
#  888           888          888oooo8       .8' `888.     8   `88b.  8  
#  888           888          888    "      .88ooo8888.    8     `88b.8  
#  `88b    ooo   888       o  888       o  .8'     `888.   8       `888  
#   `Y8bood8P'  o888ooooood8 o888ooooood8 o88o     o8888o o8o        `8  

clean:
	$(RM) test/*.log test/*.o test/??.mid
	$(RM) test/t_*
	$(RM) test/gmon.out
	$(RM) src/libumf.a src/*.log src/*.o
	cd doc; make clean



