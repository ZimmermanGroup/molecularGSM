# setting different energy calculators that GSM will use
file(GLOB GSM_SRC "*.h" "*.cpp")
add_executable(gsm ${GSM_SRC})

if(NOT DEFINED GSM_ENABLE_QCHEM)
	SET(GSM_ENABLE_QCHEM 0)
else()
    set(CALCULATOR "qchem" CACHE STRING "Calculator")
    set(CALCULATOR_FLAG "-DGSM_ENABLE_QCHEM=1 -DQCHEM=1")

endif()

if(NOT DEFINED GSM_ENABLE_QCHEM_SF)
	SET(GSM_ENABLE_QCHEM_SF 0)
else()
    set(CALCULATOR "qchem_sf" CACHE STRING "Calculator")
    set(CALCULATOR_FLAG "-DGSM_ENABLE_QCHEM_SF=1 -DQCHEMSF=1")
endif()

if(NOT DEFINED GSM_ENABLE_MOLPRO)
	SET(GSM_ENABLE_MOLPRO 0)
else()
    set(CALCULATOR "molpro" CACHE STRING "Calculator")
    set(CALCULATOR_FLAG "-DGSM_ENABLE_MOLPRO=1 -DUSE_MOLPRO=1")
endif()

if(NOT DEFINED GSM_ENABLE_ASE)
	SET(GSM_ENABLE_ASE 0)
else()
    set(CALCULATOR "ase" CACHE STRING "Calculator")
    set(CALCULATOR_FLAG "-DGSM_ENABLE_ASE=1 -DUSE_ASE=1")
endif()

if(NOT DEFINED GSM_ENABLE_GAUSSIAN)
	SET(GSM_ENABLE_GAUSSIAN 0)
else()
    set(CALCULATOR "gaussian" CACHE STRING "Calculator")
    set(CALCULATOR_FLAG "-DGSM_ENABLE_GAUSSIAN=1 -DUSE_GAUSSIAN")
endif()

if(NOT DEFINED GSM_ENABLE_ORCA)
	SET(GSM_ENABLE_ORCA 0)
else()
    set(CALCULATOR "orca" CACHE STRING "Calculator")
    set(CALCULATOR_FLAG "-DGSM_ENABLE_ORCA=1 -DUSE_ORCA=1")
endif()

if(NOT DEFINED GSM_ENABLE_TURBOMOLE)
        SET(GSM_ENABLE_TURBOMOLE 0)
else()
    set(CALCULATOR "turbomole" CACHE STRING "Calculator")
    set(CALCULATOR_FLAG "-DGSM_ENABLE_TURBOMOLE=1")
endif()


if( NOT(GSM_ENABLE_QCHEM EQUAL 1 OR GSM_ENABLE_QCHEM_SF EQUAL 1 OR GSM_ENABLE_MOLPRO EQUAL 1 OR GSM_ENABLE_ASE EQUAL 1 OR  GSM_ENABLE_GAUSSIAN EQUAL 1 OR GSM_ENABLE_ORCA EQUAL 1 OR GSM_ENABLE_TURBOMOLE EQUAL 1))
		set(CALCULATOR "mopac" CACHE STRING "Calculator")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CALCULATOR_FLAG}")

# source files
file(GLOB GSM_SRC
	"qchem.h"
    "*.h"
    "*.cpp"
)

list(APPEND GSM_SRC "${CMAKE_CURRENT_BINARY_DIR}/GitSHA1.cpp" GitSHA1.h)

# set up libraries
find_package(BLAS REQUIRED)

message("${BLAS_LIBRARIES} ${BLAS_LINKER_FLAGS}")

find_package(BLAS REQUIRED)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${BLAS_LINKER_FLAGS}")
target_include_directories(gsm PUBLIC "$ENV{MKLROOT}/include")

FIND_PACKAGE( OpenMP REQUIRED)
if(OPENMP_FOUND)
message("OPENMP FOUND")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

MESSAGE("-- Calculator is ${CALCULATOR}")
target_link_libraries(gsm PUBLIC ${BLAS_LIBRARIES})

# Install the gsm executable to the bin directory when `make install` is run
install(TARGETS gsm DESTINATION bin)
