# Look for Boost, GMP, and MPFR

find_package(Boost 1.53 QUIET)
if (Boost_FOUND)
  include_directories (${Boost_INCLUDE_DIRS})
  message (STATUS
    "Boost version ${Boost_VERSION} found: ${Boost_INCLUDE_DIRS}")
  add_definitions (-DEXRANDOM_HAVE_BOOST_DEC=1)
  if (${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION} VERSION_GREATER 1.53 AND
      NOT (WIN32 OR APPLE))
    list (APPEND CMAKE_FIND_LIBRARY_SUFFIXES ".so.0")
    find_library (QUADMATH_LIBRARIES quadmath)
    list (REMOVE_ITEM CMAKE_FIND_LIBRARY_SUFFIXES ".so.0")
    if (QUADMATH_LIBRARIES)
      message (STATUS "quadmath found: ${QUADMATH_LIBRARIES}")
      add_definitions (-DEXRANDOM_HAVE_BOOST_FLOAT128=1)
      if (CMAKE_CXX_COMPILER_ID STREQUAL GNU AND
          NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
	# Enable Q suffix for quad precision in g++ 4.8 and later
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fext-numeric-literals")
      endif ()
    endif ()
  endif ()
  if (NOT WIN32)
    # Suppress the warnings that boost can spew out, e.g.,
    #   "typedef 'boost_concept_check905' locally defined but not used"
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
  endif ()
endif ()

if (NOT GMP_DIR)
  set (GMP_DIR /usr)
endif ()
find_path (GMP_INCLUDE_DIRS gmp.h HINTS ${GMP_DIR}/include)
find_library (GMP_LIBRARIES gmp HINTS ${GMP_DIR}/lib64 ${GMP_DIR}/lib)

if (GMP_INCLUDE_DIRS AND GMP_LIBRARIES)
  set (GMP_FOUND TRUE)
  include_directories (${GMP_INCLUDE_DIRS})
  message (STATUS "GMP found: ${GMP_LIBRARIES}")
  if (Boost_FOUND)
    add_definitions (-DEXRANDOM_HAVE_BOOST_GMP=1)
  endif ()
endif ()

if (GMP_FOUND)
  if (NOT MPFR_DIR)
    set (MPFR_DIR /usr)
  endif ()
  find_path (MPFR_INCLUDE_DIRS mpfr.h HINTS ${MPFR_DIR}/include)
  find_library (MPFR_LIBRARIES mpfr HINTS ${MPFR_DIR}/lib64 ${MPFR_DIR}/lib)

  if (MPFR_INCLUDE_DIRS AND MPFR_LIBRARIES )
    set (MPFR_FOUND TRUE)
    include_directories (${MPFR_INCLUDE_DIRS})
    message (STATUS "MPFR found: ${MPFR_LIBRARIES}")
    add_definitions (-DEXRANDOM_HAVE_MPFR=1)
    if (Boost_FOUND)
      add_definitions (-DEXRANDOM_HAVE_BOOST_MPFR=1)
    endif ()

    find_path (MPREAL_INCLUDE_DIRS NAMES mpreal.h
      HINTS ${PROJECT_SOURCE_DIR}/include
      ${MPFR_INCLUDE_DIRS} ${GMP_INCLUDE_DIRS})
    if (MPREAL_INCLUDE_DIRS)
      include_directories (${MPREAL_INCLUDE_DIRS})
      message (STATUS "mpreal found: ${MPREAL_INCLUDE_DIRS}/mpreal.h")
      add_definitions (-DEXRANDOM_HAVE_MPREAL=1)
    endif ()
  endif ()
endif ()

file (GLOB EXAMPLE_SOURCES [a-z]*.cpp)

foreach (EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
  get_filename_component (EXAMPLE ${EXAMPLE_SOURCE} NAME_WE)
  add_executable (${EXAMPLE} ${EXAMPLE_SOURCE})
  if (QUADMATH_LIBRARIES)
    target_link_libraries(${EXAMPLE} ${QUADMATH_LIBRARIES})
  endif ()
  if (GMP_LIBRARIES)
    target_link_libraries(${EXAMPLE} ${GMP_LIBRARIES})
  endif ()
  if (MPFR_LIBRARIES)
    target_link_libraries(${EXAMPLE} ${MPFR_LIBRARIES})
  endif ()
endforeach ()
