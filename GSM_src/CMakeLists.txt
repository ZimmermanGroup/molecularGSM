TRIBITS_PACKAGE(gsm)

file(GLOB GSM_SRC
    "*.h"
    "*.cpp"
)

#set(MKL_USE_STATIC_LIBS ON)
find_package(MKL REQUIRED)
include_directories(${MKL_INCLUDE_DIRS})
set(LIBS ${LIBS} ${MKL_LIBRARIES})

ADD_EXECUTABLE(gfstringq ${GSM_SRC})
#ADD_EXECUTABLE(gfstringq)
target_link_libraries(gfstringq ${LIBS})
TRIBITS_PACKAGE_POSTPROCESS()
