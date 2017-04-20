/**
 * @file digit_arithmetic.hpp
 * @author Charles Karney <charles.karney@sri.com>
 * @brief Definition of digit_arithmetic
 *
 * Copyright (c) Charles Karney (2014) and licensed under the MIT/X11 License.
 * For more information, see http://exrandom.sourceforge.net/
 */

#if !defined(EXRANDOM_DIGIT_ARITHMETIC_HPP)
#define EXRANDOM_DIGIT_ARITHMETIC_HPP 1

#include <cstdint>              // for uint_fast32_t
#include <limits>               // for numeric_limits<uint_t>::digits;

#include <exrandom/exrandom_config.hpp>

/**
 * @brief The common namespace.
 *
 * The entire library resides in this namespace so avoid the possibility of
 * name conflicts.  For the same reason, all the headers are in the exrandom
 * directory in the include path.
 */
namespace exrandom {

  /**
   * @relates digit_arithmetic
   * The unsigned type used for random digits.
   */
  typedef std::uint_fast32_t uint_t;

  /**
   * @relates digit_arithmetic
   * The size of uint_t in bits.
   */
  const int uint_w = std::numeric_limits<uint_t>::digits;

  /**
   * @relates digit_arithmetic
   * A mask for uint_t.
   */
  const uint_t uint_m = 0xffffffffUL;

  /**
   * @relates digit_arithmetic
   * @param x
   * @return the position of the most significant bit in @e x.
   *
   * returns 0 for @e x = 0, 32 for @e x = uint_m
   */
#if EXRANDOM_CONSTEXPR
  constexpr
#else
  inline
#endif
  int highest_bit_idx(unsigned x)
  { return x == 0 ? 0 : 1 + highest_bit_idx(x >> 1); }

  /**
   * @brief Machinery to manipulate bases.
   *
   * @tparam b the base as an unsigned.
   *
   * Base @e b = 0 is treated as 2<sup>32</sup>. @e b = 1 is disallowed.
   */
  template<uint_t b> class digit_arithmetic {
  public:
    /**
     * The base (or 0 if the base is 2<sup>32</sup>).
     */
    static const uint_t base = b & uint_m;
    /**
     * The base less 1, in range [1, 2<sup>32</sup> &minus; 1].
     */
    static const uint_t basem1 = base ? base - 1UL : uint_m;
#if defined(DOXYGEN) || EXRANDOM_CONSTEXPR
    /**
     * The number of bits needed to hold a digit in [0, @e basem1].
     */
    static const int bits = highest_bit_idx(unsigned(basem1));
#else
    static const int bits =
      (basem1 <=        0x0UL ?  0 :
       basem1 <=        0x1UL ?  1 :
       basem1 <=        0x3UL ?  2 :
       basem1 <=        0x7UL ?  3 :
       basem1 <=        0xfUL ?  4 :
       basem1 <=       0x1fUL ?  5 :
       basem1 <=       0x3fUL ?  6 :
       basem1 <=       0x7fUL ?  7 :
       basem1 <=       0xffUL ?  8 :
       basem1 <=      0x1ffUL ?  9 :
       basem1 <=      0x3ffUL ? 10 :
       basem1 <=      0x7ffUL ? 11 :
       basem1 <=      0xfffUL ? 12 :
       basem1 <=     0x1fffUL ? 13 :
       basem1 <=     0x3fffUL ? 14 :
       basem1 <=     0x7fffUL ? 15 :
       basem1 <=     0xffffUL ? 16 :
       basem1 <=    0x1ffffUL ? 17 :
       basem1 <=    0x3ffffUL ? 18 :
       basem1 <=    0x7ffffUL ? 19 :
       basem1 <=    0xfffffUL ? 20 :
       basem1 <=   0x1fffffUL ? 21 :
       basem1 <=   0x3fffffUL ? 22 :
       basem1 <=   0x7fffffUL ? 23 :
       basem1 <=   0xffffffUL ? 24 :
       basem1 <=  0x1ffffffUL ? 25 :
       basem1 <=  0x3ffffffUL ? 26 :
       basem1 <=  0x7ffffffUL ? 27 :
       basem1 <=  0xfffffffUL ? 28 :
       basem1 <= 0x1fffffffUL ? 29 :
       basem1 <= 0x3fffffffUL ? 30 :
       basem1 <= 0x7fffffffUL ? 31 :
       basem1 <= 0xffffffffUL ? 32 :
       33);
#endif

    /**
     * Is the base a power of 2?
     */
    static const bool power_of_two =
              basem1 == (bits == uint_w ? ~uint_t(0) :
                      ~(~uint_t(0) << (bits < uint_w ? bits : 0)));
  private:
    // Require base > 1
    static_assert(bits != 0, "base must be 2 or more");
  };

}

#endif  // EXRANDOM_DIGIT_ARITHMETIC_HPP
