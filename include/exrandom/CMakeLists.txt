# Install the header files.
file (GLOB HEADERS [a-z]*.hpp)
install (FILES ${HEADERS} DESTINATION include/exrandom)
