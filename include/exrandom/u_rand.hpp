/**
 * @file u_rand.hpp
 * @author Charles Karney <charles.karney@sri.com>
 * @brief Definition of u_rand
 *
 * Copyright (c) Charles Karney (2014) and licensed under the MIT/X11 License.
 * For more information, see http://exrandom.sourceforge.net/
 */

#if !defined(EXRANDOM_U_RAND_HPP)
#define EXRANDOM_U_RAND_HPP 1

#include <iostream>             // for std::ostream, etc.
#include <string>               // for print routines
#include <sstream>              // for print routines
#include <iomanip>              // for print routines
#include <algorithm>            // for std::swap, std::max
#include <utility>              // for std::pair
#include <vector>               // for fraction in u_rand
#include <limits>               // for conversion to floating point
#include <cmath>                // for std::ldexp

#include <exrandom/digit_arithmetic.hpp>
#include <exrandom/i_rand.hpp>

#if defined(_MSC_VER)
// Squelch warnings about constant conditional expressions and possible loss of
// data.  (Could suppress these with casts but then we lose the mixed type
// special cases offered by mpreal.)
#  pragma warning (push)
#  pragma warning (disable: 4127 4244 4267)
#endif

namespace exrandom {

#if !defined(DOXYGEN)
  // This is a bunch or support utilities to enable u_rand::value to work
  // across multiple types of floating point numbers.  So far it's been tested
  // with
  //
  //   float, double, long double
  //   mpfr::mpreal
  //   boost's mpfr_float, mpf_float, cpp_dec_float, and float128
  //
  // The issues are
  //   mpreal has a variable precision and rounding style.
  //   cpp_dec_float has a decimal base
  //   for efficiency we avoid explicit casting in mixed more operatiosn
  //   we want to be able to select between ldexp, scalbn, and pow
  //
  // For the right specializations to be defined, this file must be included
  // *after* the one that defines the floating point type.  The checking here
  // depends on MPREAL_VERSION_STRING for mpfr::mpreal (this appeared in
  // version 3.5.7, June 3, 2014).  It's probable that C++11 template magic
  // using SFINAE could remove this restrication.

  template<typename RealType>
  inline int real_digits() {
    static_assert(!std::numeric_limits<RealType>::is_integer,
                  "invalid real type RealType");
    return std::numeric_limits<RealType>::digits;
  }

  template<typename RealType>
  inline std::float_round_style real_round_style() {
    static_assert(!std::numeric_limits<RealType>::is_integer,
                  "invalid real type RealType");
    return std::numeric_limits<RealType>::round_style;
  }

#if defined(MPREAL_VERSION_STRING)
  // Specializations for mprf::mpreal
  template<>
  inline int real_digits<mpfr::mpreal>()
  { return std::numeric_limits<mpfr::mpreal>::digits(); }

  template<>
  inline std::float_round_style real_round_style<mpfr::mpreal>()
  { return std::numeric_limits<mpfr::mpreal>::round_style(); }
#endif

  template<typename RealType>
  inline RealType real_ldexp(const RealType& x, int n) {
    static_assert(!std::numeric_limits<RealType>::is_integer,
                  "invalid real type RealType");
    using std::ldexp; return ldexp(x, n);
    // Alternatives (assuming radix == 2 for scalbn):
    //   using std::pow; return x * pow(RealType(2), n);
    //   using std::scalbn; return scalbn(x, n);
  }

#if !defined(EXRANDOM_CXX11_MATH)
  // Some (maybe?) optimizations for non Windows (g++ 4.8, clang 5.1)

  // Timing comparison
  // cols are: ldexp pow scalbn
  // rows are times (ns)
  //   normal (float/double/long double)
  //   exponential (float/double/long double)

  // g++ 4.8
  //  361.1  346.6  357.5
  //  330.4  305.7  321.8
  //  394.5  426.7  357.2
  //  102.9   80.8   98.6
  //  116.9   89.5  107.4
  //  179.5  212.3  140.3

  // clang 5.1
  //  376.4  382.0  373.0
  //  422.1  386.8  387.2
  //  411.3  406.8  409.2
  //  107.8  111.7  107.7
  //  122.7  121.7  122.8
  //  143.7  140.6  143.6

  template<>
  inline float real_ldexp<float>(const float& x, int n)
  { return x * std::pow(2.0f, n); }

  template<>
  inline double real_ldexp<double>(const double& x, int n)
  { return x * std::pow(2.0, n); }

  template<>
  inline long double real_ldexp<long double>(const long double& x, int n)
  { return std::scalbn(x, n); }

#endif
#endif

  /**
   * @brief The machinery to handle u-rands, arbitrary precision random
   * deviates.
   *
   * A u_rand is represented in base b as s (n +
   * &sum;<sub>k=0</sub><sup>K&minus;1</sup> d<sub>k</sub>
   * b<sup>&minus;k&minus;1</sup> + b<sup>&minus;K</sup>U) where s = &plusmn;1,
   * n and K are non-negative integers, and d<sub>k</sub> &isin; [0,b).
   *
   * @tparam digit_gen the type of digit generator.
   */
  template<typename digit_gen> class u_rand {
  public:
    /**
     * The constructor.
     *
     * @param D A reference to the digit generator to be used.
     *
     * The initial state represents a random deviate uniform in (0,1)
     */
    u_rand(digit_gen& D) : _s(1), _n(0U), _D(D) {}
    /**
     * Reset to the initial state.
     *
     * @return the u_rand itself.
     *
     * The returned result allows the idiom x.init().less_than(g, y) to be
     * used.
     */
    u_rand& init()
    { _s = 1; _n = 0U; _d.clear(); return *this; }
    /**
     * Swap two u_rands.
     *
     * @param t the u_rand to swap with.
     *
     * The digit generator is not part of the swap.
     */
    void swap(u_rand& t) {
      if (this == &t) return;
      // Don't swap _D, these should be refs to the same object
      std::swap(_s, t._s); std::swap(_n, t._n); _d.swap(t._d);
    }
    /**
     * The copy assignment operator.
     *
     * @param t The u_rand to assign to.
     * @return the u_rand itself.
     *
     * This is a safe assignment.  In the event of an error, the original state
     * will be preserved.
     */
    u_rand& operator=(const u_rand& t)
    { u_rand r(t); swap(r); return *this; }
    /**
     * @return sign of the u_rand (either +1 or -1).
     */
    int sign() const { return _s; }
    /**
     * Change the sign of the u_rand.
     */
    void negate() { _s = -_s; }
    /**
     * @return unsigned integer part of the u_rand.
     */
    unsigned integer() const { return _n; }
    /**
     * Set the integer part of the u_rand.
     * @param n the new value of the integer part.
     *
     * Note that @n is unsigned.
     */
    void set_integer(unsigned n) { _n = n; }
    /**
     * @return the number of digits in the fraction.
     */
    size_t ndigits() const { return _d.size(); }
    /**
     * The k'th digit generating it if necessary.
     *
     * @tparam Generator the type of g.
     * @param g the random generator engine used to generate the digit.
     * @param k which digit of the fraction to return (counting from 0).
     * @return the value of the digit.
     */
    template<typename Generator>
    uint_t digit(Generator& g, size_t k) {
      for (size_t i = ndigits(); i <= k; ++i) _d.push_back(_D(g));
      return _d[k];
    }
    /**
     * The k'th digit (which must already be generated).
     *
     * @param k which digit of the fraction to return (counting from 0).
     * @return the value of the digit.
     */
    uint_t rawdigit(size_t k) const { return _d[k]; }
    /**
     * A reference to the k'th digit (which must already be generated).
     *
     * @param k which digit of the fraction to return (counting from 0).
     * @return a reference to the k'th digit.
     */
    uint_t& rawdigit(size_t k) { return _d[k]; }
    /**
     * Test *this &lt; t.
     *
     * @tparam Generator the type of g.
     * @param g the random generator engine used to generate the digit.
     * @param t the u_rand to compare with.
     * @return *this &lt; t.
     */
    template<typename Generator>
    bool less_than(Generator& g, u_rand& t) {
      if (this == &t) return false;
      if (_s != t._s) return _s < t._s;
      if (_n != t._n) return (_s < 0) ^ (_n < t._n);
      for (size_t k = 0;; ++k) {
        uint_t a = digit(g, k), b = t.digit(g, k);
        if (a != b) return (_s < 0) ^ (a < b);
      }
    }
    /**
     * Test *this &lt; 1/2.
     *
     * @tparam Generator the type of g.
     * @param g the random generator engine used to generate the digit.
     * @return *this &lt; 1/2.
     */
    template<typename Generator>
    bool less_than_half(Generator& g) {
      if (_s < 0) return true;
      if (_n > 0) return false;
      return truncatep(g, size_t(0));
    }
    /**
     * Determine how the result should be rounded.
     *
     * @tparam Generator the type of g.
     * @param g the random generator engine used to generate the digit.
     * @param k which rounding digit.
     * @return true if the result can be truncated towards zero.
     *
     * Look at the kth (and subsequent) digits to determine whether the (k-1)th
     * digit should retained (i.e., the result rounded towards zero) or
     * incremented (i.e., the result should rounded away from zero).  If k = 0,
     * then the (k-1)th digit affects the integer part of the result.  If base
     * is even only one digit is tested.
     */
    template<typename Generator>
    bool truncatep(Generator& g, size_t k) {
      for (;; ++k) {
        uint_t d = digit(g, k);
        if (d <= (bm1 - 1U) / 2U) return true;
        if (d >   bm1       / 2U) return false;
      }
    }
    /**
     * Compare *this with u1/v and u2/v.
     *
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @param u1 the numerator of the first fraction.
     * @param u2 the numerator of the second fraction.
     * @param v the denominator of the fractions.
     * @return -1 if *this &lt; u1/v, +1 if frac(*this) &gt; u2/v, and 0
     *   otherwise.
     *
     * This function requires v &gt; 0, u2 &gt; u1.
     */
    template<typename Generator>
    int compare(Generator& g, long long u1, long long u2, long long v) {
      long long lbase = base;
      u1 = (std::max)(0LL, _s * u1 - (long long)(_n) * v);
      u2 = (std::min)( v , _s * u2 - (long long)(_n) * v);
      for (size_t k = 0;; ++k) {
        if (u1 >= v) return -_s; // u1/v >= 1, so *this < u1/v
        if (u2 <= 0) return  _s; // u2/v <= 0, so *this > u2/v
        if (u1 <= 0 && u2 >= v) return 0;
        long long d = digit(g, k);
        u1 = (std::max)(0LL, u1 * lbase - d * v);
        u2 = (std::min)( v , u2 * lbase - d * v);
      }
    }
    /**
     * Compare *this with a i_rand.
     *
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @param u0 the offset in the numerator.
     * @param c the multiplier for the i_rand.
     * @param v the denomiator of the fraction.
     * @param h the i_rand.
     * @return *this &lt; u/v, where u = u0 + h * c.
     *
     * This function requires v &gt; 0, c &gt; 0.
     */
    template<typename Generator>
    bool less_than(Generator& g, long long u0, long long c, long long v,
                   i_rand<digit_gen>& h) {
      for (;;) {
        int r = compare(g, u0 + h.min()*c, u0 + h.max()*c, v);
        if (r < 0) return true;
        if (r > 0) return false;
        h.refine(g);
      }
    }
    /**
     * The lower end of the range as a rational.
     *
     * @param k the number of digits to include from the fractional part of the
     *   number.
     * @return the lower end of the range represented by the u_rand with a
     *   given number of (already generated) digits in the fraction as a
     *   rational (a std::pair of the numerator and denominator).
     *
     * The fraction is not in lowest terms.  Thus the upper end can be found by
     * adding 1 to the numerator.  Overflow is possible in this routine and no
     * attempt is made to detect this.

     * This function requires that base &le; 2<sup>8</sup> (in an attempt to
     * avoid overflow).
     */
    std::pair<long long, long long> rawrational(size_t k) const {
      static_assert(bm1 <= 0xffUL, "base must not be larger than 256");
      long long num = _n, den = 1LL;
      for (size_t j = 0; j < k; ++j) {
        num = base * num + _d[j];
        den *= base;
      }
      if (_s < 0) num = -num - 1LL;
      return std::pair<long long, long long>(num, den);
    }
    /**
     * The lower end of the range as a rational.
     *
     * @return the lower end of the range represented by the u_rand as a
     *   rational (a std::pair of the numerator and denominator).
     *
     * The fraction is not in lowest terms.  Thus the upper end can be found by
     * adding 1 to the numerator.  Overflow is possible in this routine and no
     * attempt is made to detect this.
     *
     * This function requires that base &le; 2<sup>8</sup> (in an attempt to
     * avoid overflow).
     */
    std::pair<long long, long long> rational() const
    { return rawrational(ndigits());}
    /**
     * The lower end of the range as a rational.
     *
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @param k the number of digits to include from the fractional part of the
     *   number.
     * @return the lower end of the range represented by the u_rand with a
     *   given number of digits in the fraction as a rational (a std::pair of
     *   the numerator and denominator).
     *
     * The fraction is not in lowest terms.  Thus the upper end can be found by
     * adding 1 to the numerator.  Overflow is possible in this routine and no
     * attempt is made to detect this.
     *
     * This function requires that base &le; 2<sup>8</sup> (in an attempt to
     * avoid overflow).
     */
    template<typename Generator>
    std::pair<long long, long long> rational(Generator& g, size_t k) {
      if (k) digit(g, k - 1);
      { return rawrational(k); }
    }
    /**
     * The range.
     *
     * @tparam RealType the floating point type for the ends of the range.
     * @return the range represented by the u_rand as a std::pair.
     *
     * The calculations of the end points are done using ordinary floating
     * point arithmetic.  The results will be reasonably close to the true end
     * points.
     */
    template<typename RealType> std::pair<RealType, RealType> range() const {
      static_assert(!std::numeric_limits<RealType>::is_integer,
                    "invalid real type RealType");
      RealType x = 0, d = 1,
        v = digit_gen::template invrealbase<RealType>();
      for (size_t k = _d.size(); k--;) {
        x = (_d[k] + x) * v;
        d *= v;
      }
      x += _n;
      return std::pair<RealType, RealType>(_s > 0 ? x : -(x + d),
                                           _s > 0 ? (x + d) : -x);
    }
    /**
     * The midpoint of the range.
     *
     * @tparam RealType the floating point type for the result.
     * @return the midpoint of the current range.
     *
     * The calculation of the midpoint is done using ordinary floating point
     * arithmetic.  The result will be reasonably close to the true midpoint.
     */
    template<typename RealType>
      RealType midpoint() {
      auto r = range<RealType>();
      return (r.first + r.second) / 2;
    }
    /**
     * The midpoint of the range.
     *
     * @tparam RealType the floating point type for the result.
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @param k make sure there are at least k digits in the fraction.
     * @return the midpoint of the resulting range.
     *
     * The calculation of the midpoint is done using ordinary floating point
     * arithmetic.  The result will be reasonably close to the true midpoint.
     */
    template<typename RealType, typename Generator>
      RealType midpoint(Generator& g, size_t k) {
      if (k) digit(g, k - 1U);
      return midpoint<RealType>();
    }

    /**
     * Return the value of the u_rand with specified rounding as a floating
     * point number of type RealType and, if necessary, creating additional
     * digits of the number.  Also return inexact flag to indicate whether the
     * rounded result is greater or less than the true result.
     *
     * @tparam RealType the floating point type to convert to.
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @param rnd the rounding mode; by default this is the rounding mode for
     *   RealType.
     * @param[out] flag the inexact flag, +1 (resp. -1) if rounded result is
     *   greater (resp. less) than the true result.
     * @return the value of the u_rand rounded to a RealType.
     *
     * This function is only implemented if the base is a power of two for
     * conventional floating point types (which have radix = 2).  If the
     * floating point radix is not two (e.g., radix = 10), then the base needs
     * to match the radix and be even.  The meaning for the inexact flag is the
     * same as in the MPFR library.
     *
     * Possible values for rnd are
     * - std::round_indeterminate, taken to mean round away from zero
     * - std::round_toward_zero
     * - std::round_to_nearest, this is the default for float, double, etc.
     * - std::round_toward_infinity
     * - std::round_toward_neg_infinity
     *
     * The conversions have been tested for
     * - float, double, long double
     * - mpfr::mpreal
     * - boost's mpfr_float, mpf_float, cpp_dec_float, and float128
     * .
     * RealType needs to support the following operations:
     * - std::numeric_limits<RealType> must include min(), radix, digits,
     *   min_exponent, max_exponent, has_denorm, round_style
     * - std::numeric_limits<RealType>::is_integer is false
     * - basic arithmetic: negation, addition
     * - ldexp(const RealType&, int) if radix is 2; multiplication otherwise
     * .
     * Special treatment is included for
     * - mpfr::mpreal (std::numeric_limits has digits() and round_style()
     *   instead of digits and round_style)
     **********************************************************************/
    template<typename RealType, typename Generator>
    RealType value(Generator& g, std::float_round_style rnd, int& flag) {
      // Need to treat rounding explicitly since the missing digits always
      // imply rounding up.
      static_assert(!std::numeric_limits<RealType>::is_integer,
                    "invalid real type RealType");
      static const uint_t radix = std::numeric_limits<RealType>::radix;
      static const bool binary = radix == 2U;
        // How many RealType digits fit into a u_rand digit
      static const int xbits = binary ? bits  : 1;
      static_assert((binary && power_of_two) ||
                    (radix == base && (base & 1U) == 0U),
                    "require RealType::radix = 2 and base a power of two "
                    "or RealType::radix = base = even");
      // Check that overflow cannot happen (skip this check if radix is not 2
      // or 10)
      static_assert(binary ? std::numeric_limits<RealType>::max_exponent >= 32 :
                    radix == 10U ?
                    std::numeric_limits<RealType>::max_exponent >= 10 : true,
                    "RealType::max_exponent too small");
      // std::numeric_limits<RealType>::digits isn't defined for mpreals
      const int digits = real_digits<RealType>(),
        // Put a sane lower limit on min_exp.  Boost's gmp_float has
        // min_exponent = -2^63 which doesn't fit into an int.
        min_exp = std::numeric_limits<RealType>::min_exponent < -(1<<30) ?
        -(1<<30) : std::numeric_limits<RealType>::min_exponent;
      // round_dir is rounding direction for magnitude of number:
      // -1 = down, 0 = nearest, 1 = up
      flag =
        rnd == std::round_to_nearest ? 0 :
        rnd == std::round_toward_zero ? -1 :
        rnd == std::round_toward_infinity ? _s :
        rnd == std::round_toward_neg_infinity ? -_s :
        +1;                 // round_indeterminate taken to mean away from zero
      int lead;             // Position of leading bit (0.5 = position 0)
      if (_n) {
        if (binary)
          lead = highest_bit_idx(_n);
        else {
          uint_t n = _n;
          lead = 0;
          while (n) {++lead; n /= radix;}
        }
      } else {
        int i = 0;
        while ( digit(g, size_t(i)) == 0 && i < (-min_exp)/xbits ) ++i;
        lead = binary ? highest_bit_idx(_d[i]) : _d[i] ? 1 : 0;
        lead -= (i + 1) * xbits;
        lead = lead >= min_exp ? lead :
          // To handle denormalized numbers set lead = max(lead, min_exp); if
          // no denormalized min_exp - 1 marks underflow.
          std::numeric_limits<RealType>::has_denorm == std::denorm_present ?
          min_exp : min_exp - 1;
      }
      // Position of rounding bit (0.5 = position 0)
      int trail = lead - (lead >= min_exp ? digits : 0);
      if (flag == 0) {          // Get the rounding bit
        if (binary)
          flag = int((trail > 0 ? _n >> (trail - 1) :
                      digit(g, size_t( (-trail)/xbits ))
                      >> (xbits - 1 - (-trail) % xbits)) & 1U);
        else {
          if (trail > 0) {
            uint_t n = _n;
            for (int t = 1; t < trail; ++t) n /= radix;
            flag = int((n % radix) / (radix >> 1));
          } else
            flag = int(digit(g, size_t(-trail)) / (radix >> 1));
        }
        flag = flag == 0 ? -1 : 1;
      }
      RealType z(flag > 0 ? 1 : 0);
      // Result is given by the bits in [trail+1, lead] + rounding, where
      // rounding = z * 2^trail
      if (trail >= 0) {
        if (binary)
          z = real_ldexp<RealType>(z, trail) + (_n & (~uint_t(0) << trail));
        else {
          uint_t n = _n;
          for (int t = 0; t < trail; ++t) n /= radix;
          z += n;
          for (int t = 0; t < trail; ++t) z *= radix;
        }
      } else if (lead >= min_exp) {
        // digit with bit trail+1
        size_t k = size_t((-trail-1) / xbits);
        if (binary) {
          z = real_ldexp<RealType>(z, int(k + 1) * xbits + trail) +
            (digit(g, k) & (~uint_t(0) << ((k + 1) * xbits + trail)));
          while (k--) z = real_ldexp<RealType>(z, -xbits) + _d[k];
          z = real_ldexp<RealType>(z, -xbits) + _n;
        } else {
          z += digit(g, k);
          while (k--) z = z / radix + _d[k];
          z = z / radix + _n;
        }
      } else
        z *= std::numeric_limits<RealType>::min(); // Handle underflow
      flag *= _s;
      return _s * z;
    }

    /**
     * Return the value of the u_rand rounded to nearest floating point number
     * of type RealType and, if necessary, creating additional digits of the
     * number.  Also return inexact flag to indicate whether the rounded result
     * is greater or less than the true result.
     *
     * @tparam RealType the floating point type to convert to.
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @param[out] flag the inexact flag, +1 (resp. -1) if rounded result is
     *   greater (resp. less) than the true result.
     * @return the value of the u_rand rounded to a RealType.
     *
     * This function is only implemented if the base is a power of two for
     * conventional floating point types (which have radix = 2).  If the
     * floating point radix is not two (e.g., radix = 10), then the base needs
     * to match the radix and be even.  The meaning for the inexact flag is the
     * same as in the MPFR library.
     **********************************************************************/
    template<typename RealType, typename Generator>
    RealType value(Generator& g, int& flag) {
      return value<RealType, Generator>(g, real_round_style<RealType>(), flag);
    }
    /**
     * Return the value of the u_rand rounded to nearest floating point number
     * of type RealType and, if necessary, creating additional digits of the
     * number.
     *
     * @tparam RealType the floating point type to convert to.
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @return the value of the u_rand rounded to a RealType.
     *
     * This function is only implemented if the base is a power of two for
     * conventional floating point types (which have radix = 2).  If the
     * floating point radix is not two (e.g., radix = 10), then the base needs
     * to match the radix and be even.
     **********************************************************************/
    template<typename RealType, typename Generator>
    RealType value(Generator& g) {
      int flag;
      return value<RealType, Generator>(g, real_round_style<RealType>(), flag);
    }

    /**
     * Print a u_rand in u-rand format.
     *
     * @return the printed representation of the current number.
     *
     * This prints the number to the existing precision followed by an ellipsis
     * "&hellip;" to indicate the digits which haven't been generated yet.
     * This function is only implemented if the base is less than 16 or a power
     * of 16.  The representation uses the base of the u_rand or hexadecimal if
     * base &gt; 16.
     */
    std::string print() const {
      return bareprint(*this) + "...";
    }
    /**
     * Print a u_rand in rounded fixed point format.
     *
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @param k
     * @return the printed fixed point representation rounded to k digits.
     *
     * This prints the number with k digits in the fraction and (+)
     * (resp. (&minus;)) to indicate that the true result is further from
     * (resp. closer to) zero.  This function is only implemented if the base
     * is less than 16 or a power of 16.  The representation uses the base of
     * the u_rand or hexadecimal if base &gt; 16.
     */
    template<typename Generator>
    std::string print_fixed(Generator& g, size_t k) {
      bool trunc = truncatep(g, k);
      u_rand x(*this);
      x._d.resize(k);
      if (!trunc) {
        while (k--) {
          if (x._d[k] < bm1) {
            ++x._d[k];
            break;
          } else
            x._d[k] = uint_t(0);
        }
        if (++k == 0U)
          ++x._n;
      }
      return bareprint(x) + (trunc ? "(+)" : "(-)");
    }
    /**
     * Print a u_rand in u-rand format.
     *
     * @param os an output stream.
     * @param x a u_rand.
     * @return os.
     *
     * This prints the number to the existing precision followed by an ellipsis
     * "..." to indicate the digits which haven't been generated yet.  This
     * function is only implemented if the base is less than 16 or a power of
     * 16.  The representation uses the base of the u_rand or hexadecimal if
     * base &gt; 16.
     **********************************************************************/
    friend std::ostream& operator<<(std::ostream& os, const u_rand& x)
    { os << x.print(); return os; }

    /**
     * The base of the digit generator.
     */
    static const uint_t base = digit_gen::base;

  private:
    int _s;                     // the sign
    unsigned _n;                // the integer part; N.B. _n >= 0
    std::vector<uint_t> _d;     // the digits in the fraction
    digit_gen& _D;
    static const uint_t bm1 = digit_gen::max_value;
    static const int bits = digit_gen::bits;
    static const bool power_of_two = digit_gen::power_of_two;
    static std::string bareprint(const u_rand& x) {
      // Print in the current base, except that use hexadecimal for base > 16.
      // For that reason if base > 16, we require that it be a power of 16.
      static_assert(bm1 < 15U || (power_of_two && (bits % 4 == 0)),
                    "Output requires base < 16 or base = a power of 16");
      std::ostringstream os;
      os << (x._s < 0 ? '-' : '+') << std::setfill('0') << std::hex;
      if (x._n == 0 || bits >= 4)
        os << x._n;
      else {
        std::ostringstream osint;
        osint << std::hex;
        int n = x._n;
        while (n) {
          // Avoid silly warnings from the compiler when base == 0
          osint << n % (base ? base : uint_t(1));
          n /= base ? base : uint_t(1);
        }
        std::string sint = osint.str();
        std::reverse(sint.begin(), sint.end());
        os << sint;
      }
      if (x._d.size()) os << '.';
      for (size_t k = 0; k < x._d.size(); ++k)
        os << std::setw((bits+3)/4) << x._d[k];
      return os.str();
    }

  };

}

#if defined(_MSC_VER)
#  pragma warning (pop)
#endif

#endif  // EXRANDOM_U_RAND_HPP
