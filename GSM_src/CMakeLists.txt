TRIBITS_PACKAGE(gsm)

#file(GLOB GSM_SRC
#    "*.h"
#    "*.cpp"
#)

#SET(GSM_SRC GLOB *.cpp *.h)

set(MKL_USE_STATIC_LIBS ON)
find_package(MKL)
if (MKL_FOUND)
   include_directories(${MKL_INCLUDE_DIRS})
   link_directories(${MKL_LIBRARY_DIRS})
	# ADD_EXECUTABLE(gfstringq ${GSM_SRC})
	 ADD_EXECUTABLE(gfstringq ase.cpp       grad.cpp     LST.cpp      molpro.cpp
print.cpp    savefile.cpp
bmat.cpp      gstring.cpp  main.cpp     mopac.cpp   pTable.cpp
stringtools.cpp
eckart.cpp    icoord.cpp   mem.cpp      optic.cpp   qchem.cpp    utils.cpp
gaussian.cpp  knnr.cpp     mm_grad.cpp  orca.cpp    qchemsf.cpp
ase.h        eckart.h    grad.h     icoord.h  LST.h     mopac.h  pTable.h
qchemsf.h      utils.h
constants.h  gaussian.h  gstring.h  knnr.h    molpro.h  orca.h   qchem.h
stringtools.h
)
   target_link_libraries(gfstringq ${MKL_LIBRARIES})
endif()
TRIBITS_PACKAGE_POSTPROCESS()

#SET(GSM_SOURCES GLOB *.cpp)
