/**
 * @file unit_exponential_dist.hpp
 * @author Charles Karney <charles.karney@sri.com>
 * @brief Definition of unit_exponential_dist
 *
 * Copyright (c) Charles Karney (2014) and licensed under the MIT/X11 License.
 * For more information, see http://exrandom.sourceforge.net/
 */

#if !defined(EXRANDOM_UNIT_EXPONENTIAL_DIST_HPP)
#define EXRANDOM_UNIT_EXPONENTIAL_DIST_HPP 1

#include <exrandom/u_rand.hpp>

#if defined(_MSC_VER)
// Squelch warnings about constant conditional expressions
#  pragma warning (push)
#  pragma warning (disable: 4127)
#endif

namespace exrandom {

  /**
   * @brief Sample u-rands exactly from the unit exponential distribution.
   *
   * This samples from the unit exponential distribution P(x) = exp(&minus;x)
   * for x &gt; 0.  This implements Algorithms E and V.
   *
   * @tparam digit_gen the type of digit generator.
   * @tparam bit_optimized if true use Algorithm E, else use Algorithm V.
   *
   * This class allows a u-rand to be returned via the
   * unit_exponential_dist::generate member function or a floating point
   * result via the unit_exponential_dist::value<RealType> member function.
   *
   * See unit_exponential_distribution for a simpler interface to sampling
   * exponential deviates.  (But, you can't obtain u-rands with this class and
   * the constructor locks you into a specific floating point type.)
   *
   * If bit_optimized is true, digit_gen::base must be even.
   */
  template<typename digit_gen, bool bit_optimized = true>
  class unit_exponential_dist {
  public:
    /**
     * The constructor.
     *
     * @param D a reference to the digit generator to be used.
     */
    unit_exponential_dist(digit_gen& D) : _D(D), _v(D), _w(D), _x(D) {}

    /**
     * Generate the next deviate as a u_rand.
     *
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @param[out] x the u_rand to set.
     */
    template<typename Generator>
    void generate(Generator& g, u_rand<digit_gen>& x) {
      // A simple rejection method gives the 1/2 fractional part.  The number of
      // rejections gives the multiples of 1/2.
      //           bits: used    fract
      // original stats: 9.31615 2.05429
      // new      stats: 7.23226 1.74305
      int k = 0;
      while (!F(g, x)) ++k;     // Executed 1/(1 - exp(-1/2)) on average
      // If k is odd, add base/2 to first digit (allowing for base = 2^32)
      if (bit_optimized && (k % 2) != 0) x.rawdigit(0) += (bm1 - 1U) / 2U + 1U;
      x.set_integer(bit_optimized ? k/2 : k);
      return;
    }

    /**
     * Generate the next deviate and round it to a floating point number.
     *
     * @tparam RealType the floating point type of the result.
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @return the exponential deviate.
     */
    template<typename RealType, typename Generator>
    RealType value(Generator& g) {
      generate(g, _x);
      return _x.template value<RealType>(g);
    }

    /**
     * @return a reference to the digit generator used in the constructor.
     */
    digit_gen& digit_generator() const { return _D; }

    /**
     * The base of the digit generator.
     */
    static const uint_t base = digit_gen::base;
  private:
    static const uint_t bm1 = digit_gen::max_value;
    // Disable copy assignment
    unit_exponential_dist& operator=(const unit_exponential_dist&);
    digit_gen& _D;
    static_assert(!bit_optimized || (bm1 & 1U),
                  "unit_exponential_dist: base must be even");
    u_rand<digit_gen> _v, _w, _x; // temporary storage
    template<typename Generator>
    bool F(Generator& g, u_rand<digit_gen>& p) {
      p.init();
      // The early bale out
      if (bit_optimized && !p.less_than_half(g)) return false;
      // Implement the von Neumann algorithm
      if (!_w.init().less_than(g, p)) return true; // if (w < p)
      for (;;) {               // Unroll loop to avoid copying
        if (!_v.init().less_than(g, _w)) return false; // if (v < w)
        if (!_w.init().less_than(g, _v)) return true;  // if (w < v)
      }
    }

  };

}

#if defined(_MSC_VER)
#  pragma warning (pop)
#endif

#endif  // EXRANDOM_UNIT_EXPONENTIAL_DIST_HPP
