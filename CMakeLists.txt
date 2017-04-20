project (ExRandom)
cmake_minimum_required (VERSION 2.8.4)

set (PROJECT_VERSION_MAJOR 2)
set (PROJECT_VERSION_MINOR 1)
set (PROJECT_VERSION_PATCH 0)
set (PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
if (PROJECT_VERSION_PATCH GREATER 0)
  set (PROJECT_VERSION "${PROJECT_VERSION}.${PROJECT_VERSION_PATCH}")
endif ()

string (TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)

message (STATUS
  "C++ version ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/include)

# Set a default build type for single-configuration cmake generators if
# no build type is set.
if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Release)
endif ()

# Make the compiler more picky.
if (MSVC)
  string (REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else ()
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
  # Check for C++11 support.
  include (CheckCXXCompilerFlag)
  foreach (_F 1z 14 1y 11 0x)
    set (CXX11_FLAG "-std=c++${_F}")
    set (_T CXX11TEST${_F})
    message (STATUS "Testing ${CXX11_FLAG}")
    check_cxx_compiler_flag (${CXX11_FLAG} ${_T})
    if (${_T})
      break ()
    endif ()
    unset (CXX11_FLAG)
  endforeach ()
  if (CXX11_FLAG)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_FLAG}")
  endif ()
endif ()

set (DOXYGEN_SKIP_DOT ON)
find_package (Doxygen 1.8.7) # Version 1.8.7 or later needed for &hellip;

add_subdirectory (include/exrandom)
add_subdirectory (examples)
add_subdirectory (multiprec)
add_subdirectory (doc)
add_subdirectory (cmake)

set (CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}-${PROJECT_VERSION}")
set (CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME_LOWER}-${PROJECT_VERSION}")

set (CPACK_SOURCE_IGNORE_FILES
  "#"
  "~\$"
  "/\\\\.git"
  "${PROJECT_SOURCE_DIR}/BUILD"
  "${PROJECT_SOURCE_DIR}/TAGS"
  "${PROJECT_SOURCE_DIR}/distrib/"
  "${PROJECT_SOURCE_DIR}/[^/]*\\\\.(html|css)\$"
  "${PROJECT_SOURCE_DIR}/makefile-admin\$"
  "${PROJECT_SOURCE_DIR}/robots.txt\$"
  "${PROJECT_SOURCE_DIR}/include/mpreal\\\\.h\$"
  "\\\\.dSYM"
  "\\\\.eps\$"
  "\\\\.log\$"
  "\\\\.m\$"
  "\\\\.pdf\$" )

set (CPACK_SOURCE_GENERATOR TGZ ZIP)
include (CPack)

# Add test
enable_testing ()
add_test (NAME exrandom_test COMMAND exrandom_test)
