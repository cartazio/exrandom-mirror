# config file support for find_package (ExRandom).  This needs to deal
# with two environments: (1) finding the build tree and (2) finding the
# install tree.  exrandom-config.cmake detects which situation it is
# handing by looking at @PROJECT_ROOT_DIR@.  If this is an absolute
# path, it's in the build tree; otherwise, it's in the install tree.
# (Note that the whole install tree can be relocated.)

# exrandom-config.cmake for the build tree
set (PROJECT_ROOT_DIR "${PROJECT_BINARY_DIR}")
set (PROJECT_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include" )
configure_file (project-config.cmake.in
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME_LOWER}-config.cmake" @ONLY)
configure_file (project-config-version.cmake.in
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME_LOWER}-config-version.cmake" @ONLY)

# exrandom-config.cmake for the install tree.  It's installed in
# ${INSTALL_CMAKE_DIR} and @PROJECT_ROOT_DIR@ is the relative
# path to the root from there.  (Note that the whole install tree can
# be relocated.)
if (NOT WIN32)
  set (INSTALL_CMAKE_DIR "share/cmake/${PROJECT_NAME}")
  set (PROJECT_ROOT_DIR "../../..")
else ()
  set (INSTALL_CMAKE_DIR "cmake")
  set (PROJECT_ROOT_DIR "..")
endif ()
# @PROJECT_INCLUDE_DIRS@ is not used in the install tree; reset
# it to prevent the source and build paths appearing in the installed
# config files
set (PROJECT_INCLUDE_DIRS)
configure_file (project-config.cmake.in project-config.cmake @ONLY)
configure_file (project-config-version.cmake.in
  project-config-version.cmake @ONLY)
install (FILES
  "${CMAKE_CURRENT_BINARY_DIR}/project-config.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}"
  RENAME "${PROJECT_NAME_LOWER}-config.cmake")
install (FILES
  "${CMAKE_CURRENT_BINARY_DIR}/project-config-version.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}"
  RENAME "${PROJECT_NAME_LOWER}-config-version.cmake")
