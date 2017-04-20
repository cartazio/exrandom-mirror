/**
 * @file exrandom_config.hpp
 * @author Charles Karney <charles.karney@sri.com>
 * @brief Configuration for exrandom
 *
 * Copyright (c) Charles Karney (2016) and licensed under the MIT/X11 License.
 * For more information, see http://exrandom.sourceforge.net/
 */

#if !defined(EXRANDOM_CONFIG_HPP)
#define EXRANDOM_CONFIG_HPP 1

/**
 * The major version number for ExRandom.  This was introduced in version 2.1.
 **********************************************************************/
#define EXRANDOM_VERSION_MAJOR 2

/**
 * The minor version number for ExRandom.  This was introduced in version 2.1.
 **********************************************************************/
#define EXRANDOM_VERSION_MINOR 1

/**
 * The patch number for ExRandom.  This was introduced in version 2.1.
 **********************************************************************/
#define EXRANDOM_VERSION_PATCH 0

/**
 * Pack the version components into a single integer.  Users should not rely on
 * this particular packing of the components of the version number; see the
 * documentation for EXRANDOM_VERSION, below.  This was introduced in version
 * 2.1.
 **********************************************************************/
#define EXRANDOM_VERSION_NUM(a,b,c) ((((a) * 10000 + (b)) * 100) + (c))

/**
 * The version of Exrandom as a single integer, packed as MMmmmmpp where MM is
 * the major version, mmmm is the minor version, and pp is the patch level.
 * This was introduced in version 2.1.
 *
 * Users should not rely on this particular packing of the components of the
 * version number.  Instead they should use a test such as \code
   #if EXRANDOM_VERSION >= EXRANDOM_VERSION_NUM(2,1,0)
   ...
   #endif
 * \endcode
 **********************************************************************/
#define EXRANDOM_VERSION EXRANDOM_VERSION_NUM(EXRANDOM_VERSION_MAJOR,   \
                                              EXRANDOM_VERSION_MINOR,   \
                                              EXRANDOM_VERSION_PATCH)

#if !(__cplusplus >= 201103 || (defined(_MSC_VER) && _MSC_VER >= 1700))
#error Need to use a C++11 compiler supporting <random> and static_assert
#endif

#if !defined(DOXYGEN)
#if !(defined(_MSC_VER) && _MSC_VER < 1900)
#define EXRANDOM_CONSTEXPR 1
#else
#define EXRANDOM_CONSTEXPR 0
#endif

#if !(defined(_MSC_VER) && _MSC_VER < 1800)
#define EXRANDOM_CXX11_MATH 1
#else
#define EXRANDOM_CXX11_MATH 0
#endif
#endif

#endif  // EXRANDOM_CONFIG_HPP
