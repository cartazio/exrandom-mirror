if (DOXYGEN_FOUND)
  configure_file (doxyfile.in doxyfile)
  file (GLOB SOURCES
    ../include/exrandom/[a-z]*.hpp
    ../examples/[a-z]*.cpp ../multiprec/[a-z]*.cpp)
  file (GLOB LICENSE ../LICENSE.txt)
  add_custom_target (doc DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/html/index.html)
  add_custom_command (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/html/index.html
    DEPENDS
    ${CMAKE_CURRENT_BINARY_DIR}/doxyfile exrandom.dox ${SOURCES} ${LICENSE}
    COMMAND ${CMAKE_COMMAND} -E remove_directory html
    COMMAND ${DOXYGEN_EXECUTABLE} doxyfile > doxygen.log
    COMMAND ${CMAKE_COMMAND} -E copy ${LICENSE} html
    COMMENT "Generating html documentation tree")
endif ()
