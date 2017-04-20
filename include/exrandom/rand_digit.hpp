/**
 * @file rand_digit.hpp
 * @author Charles Karney <charles.karney@sri.com>
 * @brief Definition of rand_digit
 *
 * Copyright (c) Charles Karney (2014) and licensed under the MIT/X11 License.
 * For more information, see http://exrandom.sourceforge.net/
 */

#if !defined(EXRANDOM_RAND_DIGIT_HPP)
#define EXRANDOM_RAND_DIGIT_HPP 1

#include <random>               // for uniform_int_distribution
#include <cmath>                // for std::ldexp

#include <exrandom/digit_arithmetic.hpp>

#if defined(_MSC_VER)
// Squelch warnings about constant conditional expressions
#  pragma warning (push)
#  pragma warning (disable: 4127)
#endif

namespace exrandom {

  /**
   * @brief Machinery to convert a random generator into a random digit.
   *
   * @tparam b the base for the digits.
   *
   * This produces random digits in the range [0,b).  In addition it
   * keeps count of how many digits are produced.
   */
  template<uint_t b> class rand_digit {
  public:
    /**
     * The base for the digits (or 0 if the base is 2<sup>32</sup>).
     */
    static const uint_t base = digit_arithmetic<b>::base;
    /**
     * The minimum value produced by operator().
     */
    static const uint_t min_value = 0U;
    /**
     * The maximum value produced by operator().
     */
    static const uint_t max_value = digit_arithmetic<b>::basem1;
    /**
     * The number of bits needed to represent a digit.
     */
    static const int bits = digit_arithmetic<b>::bits;
    /**
     * Is the base a power of two?
     */
    static const bool power_of_two = digit_arithmetic<b>::power_of_two;
    /**
     * The constructor (which initializes the count to 0).
     */
    rand_digit() : _count(0) {}
    /**
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @return the random digit.
     */
    template<typename Generator>
    uint_t operator()(Generator& g) { // a random digit
      ++_count;
      if ( power_of_two &&
           Generator::min() == 0UL &&
           Generator::max() == 0xffffffffUL )
        // optimize for std::mt19937 which creates 32 bits of randomness
        return uint_t(g() >> (32 - bits));
      else if ( power_of_two &&
                Generator::min() == 0UL &&
                Generator::max() == 0xffffffffffffffffULL )
        // optimize for std::mt19937_64 which creates 64 bits of randomness
        return uint_t((g() & 0xffffffffUL) >> (32 - bits));
      else {
        // In some cases _gen loses track of its parameters, so supply them
        // here instead of in the constructor.
        return _gen(g, uint_random_t::param_type(min_value, max_value));
      }
    }
    /**
     * @return the count.
     */
    long long count() const { return _count; }
    /**
     * @tparam the floating point type of the result.
     * @return 1/@e base as a floating point number.
     */
    template<typename RealType>
    static RealType invrealbase() {
      using std::ldexp;
      return power_of_two ? ldexp(RealType(1), -bits) : 1/RealType(base);
    }
  private:
    typedef std::uniform_int_distribution<uint_t>  uint_random_t;
    uint_random_t _gen;
    long long _count;
  };

}

#if defined(_MSC_VER)
#  pragma warning (pop)
#endif

#endif  // EXRANDOM_RAND_DIGIT_HPP
