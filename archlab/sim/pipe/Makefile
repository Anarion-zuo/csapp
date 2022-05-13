# Modify this line to indicate the default version to build

VERSION=std

# Comment this out if you don't have Tcl/Tk on your system

GUIMODE=-DHAS_GUI

# Modify the following line so that gcc can find the libtcl.so and
# libtk.so libraries on your system. You may need to use the -L option
# to tell gcc which directory to look in. Comment this out if you
# don't have Tcl/Tk.

TKLIBS=-L/usr/lib -ltk -ltcl

# Modify the following line so that gcc can find the tcl.h and tk.h
# header files on your system. Comment this out if you don't have
# Tcl/Tk.

TKINC=-isystem /usr/include/tcl8.5

# Modify these two lines to choose your compiler and compile time
# flags.

CC=gcc
CFLAGS=-Wall -O2 -fcommon

##################################################
# You shouldn't need to modify anything below here
##################################################

MISCDIR=../misc
HCL2C=$(MISCDIR)/hcl2c
INC=$(TKINC) -I$(MISCDIR) $(GUIMODE)
LIBS=$(TKLIBS) -lm
YAS = ../misc/yas

all: psim drivers

# This rule builds the PIPE simulator
psim: psim.c sim.h pipe-$(VERSION).hcl $(MISCDIR)/isa.c $(MISCDIR)/isa.h
	# Building the pipe-$(VERSION).hcl version of PIPE
	$(HCL2C) -n pipe-$(VERSION).hcl < pipe-$(VERSION).hcl > pipe-$(VERSION).c
	$(CC) $(CFLAGS) $(INC) -o psim psim.c pipe-$(VERSION).c \
		$(MISCDIR)/isa.c $(LIBS)

# This rule builds driver programs for Part C of the Architecture Lab
drivers: 
	./gen-driver.pl -n 4 -f ncopy.ys > sdriver.ys
	../misc/yas sdriver.ys
	./gen-driver.pl -n 63 -f ncopy.ys > ldriver.ys
	../misc/yas ldriver.ys

# These are implicit rules for assembling .yo files from .ys files.
.SUFFIXES: .ys .yo
.ys.yo:
	$(YAS) $*.ys


clean:
	rm -f psim pipe-*.c *.o *.exe *~ 


##################################################
# My test shortcuts
##################################################

testdrivers: drivers
	@../misc/yis sdriver.yo | grep %rax
	@../misc/yis ldriver.yo | grep %rax
	@./correctness.pl | grep correctness

benchmark:
	@(cd ../y86-code; make testpsim)

psimgui: drivers ncopy.yo
	(make psim VERSION=full && ./psim -g ../y86-code/poptest.yo)

psimtty: drivers ncopy.yo
	(make psim VERSION=full && ./psim -t ../y86-code/poptest.yo)

isatest: drivers
	(make psim VERSION=full ; cd ../ptest; make SIM=../pipe/psim )

score: drivers ncopy.yo
	@make psim VERSION=full
	@./correctness.pl | grep 'correctness test'
	@./check-len.pl < ncopy.yo
	@./benchmark.pl | grep 'Score\|Average'
