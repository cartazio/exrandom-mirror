file (GLOB EXAMPLE_SOURCES [a-z]*.cpp)

foreach (EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
  get_filename_component (EXAMPLE ${EXAMPLE_SOURCE} NAME_WE)
  add_executable (${EXAMPLE} ${EXAMPLE_SOURCE})
endforeach ()
