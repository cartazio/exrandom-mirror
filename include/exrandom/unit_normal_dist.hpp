/**
 * @file unit_normal_dist.hpp
 * @author Charles Karney <charles.karney@sri.com>
 * @brief Definition of unit_normal_dist
 *
 * Copyright (c) Charles Karney (2014) and licensed under the MIT/X11 License.
 * For more information, see http://exrandom.sourceforge.net/
 */

#if !defined(EXRANDOM_UNIT_NORMAL_DIST_HPP)
#define EXRANDOM_UNIT_NORMAL_DIST_HPP 1

#include <algorithm>            // for std::min, std::max

#include <exrandom/u_rand.hpp>

namespace exrandom {

  /**
   * @brief Sample u-rands exactly from the unit normal distribution.
   *
   * This samples from the unit normal distribution
   * P(x) = exp(&minus; x<sup>2</sup>/2) / &radic;(2&pi;).
   * This implements Algorithm N.
   *
   * @tparam digit_gen the type of digit generator.
   *
   * This class allows a u-rand to be returned via the
   * unit_normal_dist::generate member function or a floating point result via
   * the unit_normal_dist::value<RealType> member function.
   *
   * See unit_normal_distribution for a simpler interface to sampling normal
   * deviates.  (But, you can't obtain u-rands with this class and the
   * constructor locks you into a specific floating point type.)
   *
   * digit_gen::base must be less than 2<sup>15</sup> or a power of two.
   */
  template<typename digit_gen> class unit_normal_dist {
  public:
    /**
     * The constructor.
     *
     * @param D a reference to the digit generator to be used.
     */
    unit_normal_dist(digit_gen& D) : _D(D), _y(D), _z(D), _x(D) {}

    /**
     * Generate the next deviate as a u_rand.
     *
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @param[out] x the u_rand to set.
     */
    template<typename Generator>
    void generate(Generator& g, u_rand<digit_gen>& x) {
      for (;;) {
        int k = G(g);                                // step 1
        if (!P(g, k * (k - 1))) continue;            // step 2
        x.init();                                    // step 3
        int j = k + 1; while (j-- && B(g, k, x)) {}; // step 4
        if (!(j < 0)) continue;
        x.set_integer(k);                            // step 5
        if (_y.init().less_than_half(g)) x.negate(); // step 6
        return;                                      // step 7
      }
    }

    /**
     * Generate the next deviate and round it to a floating point number.
     *
     * @tparam RealType the floating point type of the result.
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @return the normal deviate.
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
    static const int bits = digit_gen::bits;
    static const bool power_of_two = digit_gen::power_of_two;
    // Allow base in [2,2^15] and base = 2^[16,32]
    static_assert(base - 1U < (uint_t(1) << 15) || (power_of_two && bits <= 32),
                  "base must be less than 2^15 or a power of 2");
    // Disable copy assignment
    unit_normal_dist& operator=(const unit_normal_dist&);
    digit_gen& _D;
    u_rand<digit_gen> _y, _z, _x; // temporary storage
    // Algorithm H: true with probability exp(-1/2).
    template<typename Generator>
    bool H(Generator& g) {
      if (!_y.init().less_than_half(g)) return true;
      for (;;) {
        if (!_z.init().less_than(g, _y)) return false;
        if (!_y.init().less_than(g, _z)) return true;
      }
    }

    // Step N1: return n >= 0 with prob. exp(-n/2) * (1 - exp(-1/2)).
    template<typename Generator>
    int G(Generator& g)
    { int n = 0; while (H(g)) ++n; return n; }

    // Step N2: true with probability exp(-n/2).
    template<typename Generator>
    bool P(Generator& g, int n)
    { while (n-- && H(g)) {}; return n < 0; }

    // Algorithm C: return (-1, 0, 1) with prob (1/m, 1/m, 1-2/m).
    template<typename Generator>
    int C(Generator& g, int m) {
      int n1 = 1, n2 = 2;
      // Limit temporary base to 2^maxbits to avoid integer overflow
      const int maxbits = 15;
      const int shift = (power_of_two && bits > maxbits) ? bits - maxbits : 0;
      const int tbase = (power_of_two && bits > maxbits) ?
        (uint_t(1) << maxbits) : base;
      for (;;) {
        int d = int(_D(g) >> shift);
        if ((n1 = (std::max)(0, n1 * tbase - d * m)) >= m) return -1;
        if ((n2 = (std::min)(m, n2 * tbase - d * m)) <= 0) return +1;
        if (n1 <= 0 && n2 >= m) return 0;
      }
    }

    // Algorithm B: true with prob exp(-x * (2*k + x) / (2*k + 2)).
    template<typename Generator>
    bool B(Generator& g, int k, u_rand<digit_gen>& x) {
      int n = 0, m = 2 * k + 2, f;
      for (;; ++n) {
        f = k ? 0 : C(g, m); if (f < 0) break;
        if (!_z.init().less_than(g, n ? _y : x)) break;
        f = k ? C(g, m) : f; if (f < 0) break;
        if (f == 0 && (!_y.init().less_than(g, x))) break;
        _y.swap(_z);            // an efficient way of doing y = z
      }
      return (n % 2) == 0;
    }

  };

}

#endif  // EXRANDOM_UNIT_NORMAL_DIST_HPP
