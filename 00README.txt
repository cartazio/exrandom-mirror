ExRandom, a library for sampling exactly from random distributions

Copyright (c) Charles Karney (2014-2016) and licensed under the MIT/X11
License.

For full documentation, see
  http://exrandom.sourceforge.net/html
  https://dx.doi.org/10.1145/2710016
  http://arxiv.org/abs/1303.6257

Inventory:
  include/exrandom/: the library (just headers)
  doc/: the documentation
  examples/: simple examples
  multiprec/: examples which depend on multi-precision libraries
  cmake/: cmake support for the installed library

Prerequisites: C++11 compiler, e.g.,
  g++ 4.7 or later
  Visual Studio 11 or later

How to compile the example programs, QUICK START:

  Linux/MacOSX and cmake:
    mkdir BUILD
    cd BUILD
    cmake ..
    make
    make test
    examples/simple_normal
    etc.

  Windows and cmake:
    mkdir BUILD
    cd BUILD
    cmake -G "Visual Studio 11 Win64" ..
    cmake --build . --config Release
    cmake --build . --config Release --target RUN_TESTS
    examples\Release\simple_normal
    etc.

  Linux/MacOSX and make (programs in examples only):
    cd examples
    make
    ./simple_normal
    etc.

  Windows "Visual Studio 11 command prompt" (programs in examples only):
    cd examples
    nmake /f makefile.vc
    .\simple_normal
    etc.
