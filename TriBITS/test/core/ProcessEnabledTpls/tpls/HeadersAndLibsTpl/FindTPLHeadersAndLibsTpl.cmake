if (FIND_ONE_IN_SET_OF_HEADERS_1)
  set(REQUIRED_HEADERS
    "MissingHeader1.hpp HeadersAndLibsTpl_header1.hpp"
    "MissingHeader2.hpp HeadersAndLibsTpl_header2.hpp"
    )
else()
  set(REQUIRED_HEADERS HeadersAndLibsTpl_header1.hpp HeadersAndLibsTpl_header2.hpp)
endif()

if (FIND_ONE_IN_SET_OF_LIBS_1)
  set(REQUIRED_LIBS_NAMES
    "missinglib1 haltpl1"
    "missinglib2 haltpl2"
    )
else()
  set(REQUIRED_LIBS_NAMES  haltpl1  haltpl2)
endif()

if (NOT NOT_MUST_FIND_ALL_HEADERS)
  set(MUST_FIND_ALL_HEADERS_ARG  MUST_FIND_ALL_HEADERS)
endif()

if (NOT NOT_MUST_FIND_ALL_LIBS)
  set(MUST_FIND_ALL_LIBS_ARG  MUST_FIND_ALL_LIBS)
endif()

tribits_tpl_find_include_dirs_and_libraries( HeadersAndLibsTpl
  REQUIRED_HEADERS  ${REQUIRED_HEADERS}
  ${MUST_FIND_ALL_HEADERS_ARG}
  REQUIRED_LIBS_NAMES  ${REQUIRED_LIBS_NAMES}
  ${MUST_FIND_ALL_LIBS_ARG}
  )
