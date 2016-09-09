# Use this Makefile with make

# Executable name
#CMD = test/gfstringq.exe
CMD = gfstringq.exe

# -------- description of DFLAGS ---------------


# -------- Define environmental variable C_COMPILER -----------
# Make sure it is defined
#          ifeq ($(strip$(FORTRAN_COMPILER)),)
# Otherwise you can define it here also by uncommenting next line
 FC = icpc -openmp -I$(MKLROOT)/include
# FC = g++ -fopenmp -I$(MKLROOT)/include
# FC = g++ -I$(MKLROOT)/include
# FC = g++ -g -I$(MKLROOT)/include
# FC = g++ -fopenmp -g -I$(MKLROOT)/include
GIT_VERSION := $(shell git describe --abbrev=4 --dirty --always --tags)
DFLAGS = DFLAGS = -DVERSION=\"$(GIT_VERSION)\" #-Define the cpp flags to be used
OFLAGS =  # optimization
F95ROOT = $(MKLROOT)

#Intel Linkers
#LINKERFLAGS =  -L$(MKLROOT)/lib/em64t $(F95ROOT)/lib/em64t/libmkl_lapack95_lp64.a -lmkl_intel_lp64 -lmkl_sequential -lmkl_core -lpthread -lm
#Intel parallel openmp (only w/icpc compiler)
#LINKERFLAGS =  -L$(MKLROOT)/lib/em64t -lmkl_intel_lp64 -lmkl_intel_thread -lmkl_core -lpthread -lm
LINKERFLAGS =  -L$(MKLROOT)/lib/intel64 -lmkl_intel_lp64 -lmkl_intel_thread -lmkl_core -lpthread -lm
# MAC OS linkers
#LINKERFLAGS = -lm -framework Accelerate


OFLAGS =  # optimization



#
# Implicit rules for handling src files
#  ( uses static pattern rules, see info for make )
.c.o:
	$(FC) -c -g $(DFLAGS) -Wimplicit $<
.cpp.o:
	$(FC) -c -g $(DFLAGS) $<

OBJECTS = gstring.o main.o pTable.o stringtools.o qchem.o utils.o eckart.o mem.o bmat.o print.o icoord.o mm_grad.o optic.o mopac.o grad.o knnr.o ase.o gaussian.o orca.o molpro.o qchemsf.o

$(CMD) : $(OBJECTS)
	$(FC) $(DEBUG_FLAGS) $(OFLAGS) $(OBJECTS) $(LINKERFLAGS)   -o ./$(CMD)

clean:
	/bin/rm -f *.o *.i *.mod *.exe a.out make.log

cleano:
	rm -f *.o *.i

depend :
	g++ -MM *.cpp *.c >> Makefile 

# DO NOT DELETE created with g++ -MM *.cpp *.c
LST.o: LST.cpp LST.h constants.h utils.h
eckart.o: eckart.cpp eckart.h constants.h utils.h
gstring.o: gstring.cpp gstring.h utils.h constants.h stringtools.h pTable.h qchem.h eckart.h icoord.h qchem.h ase.h grad.h
mem.o: mem.cpp icoord.h qchem.h
bmat.o: bmat.cpp icoord.h qchem.h
icoord.o: icoord.cpp icoord.h qchem.h
main.o: main.cpp gstring.h utils.h constants.h stringtools.h pTable.h qchem.h eckart.h
mopac.o: mopac.cpp mopac.h qchem.h
mm_grad.o: mm_grad.cpp icoord.h 
optimize.o: optimize.cpp optimize.h utils.h constants.h eckart.h stringtools.h pTable.h qchem.h 
optic.o: optic.cpp icoord.h
pTable.o: pTable.cpp pTable.h
print.o: print.cpp icoord.h
qchem.o: qchem.cpp qchem.h stringtools.h pTable.h utils.h constants.h
stringtools.o: stringtools.cpp stringtools.h
utils.o: utils.cpp utils.h constants.h
grad.o: mopac.h qchem.h knnr.h grad.h grad.cpp ase.h gaussian.h orca.h
knnr.o: icoord.h utils.h knnr.h knnr.cpp qchem.h
ase.o: ase.h ase.cpp utils.h
gaussian.o: gaussian.h gaussian.cpp constants.h utils.h stringtools.h pTable.h
orca.o: orca.h orca.cpp constants.h utils.h stringtools.h pTable.h
molpro.o: molpro.h molpro.cpp utils.h
qchemsf.o: qchemsf.cpp qchemsf.h utils.h stringtools.h qchem.h
