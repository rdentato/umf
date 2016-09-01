# 
#  (C) by Remo Dentato (rdentato@gmail.com)
# 
# This software is distributed under the terms of the BSD license:
#   http://creativecommons.org/licenses/BSD/
#   http://opensource.org/licenses/bsd-license.php
#

# This makefile is for GNU tool chain 


all: lib tests

lib:
	cd src; make

tests:
	cd test; make
  
clean:
	cd src; make clean
	cd test; make clean


