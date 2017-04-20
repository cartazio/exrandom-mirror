/**
 * @file unit_normal_kahn.hpp
 * @author Charles Karney <charles.karney@sri.com>
 * @brief Definition of unit_normal_kahn
 *
 * Copyright (c) Charles Karney (2014) and licensed under the MIT/X11 License.
 * For more information, see http://exrandom.sourceforge.net/
 */

#if !defined(EXRANDOM_UNIT_NORMAL_KAHN_HPP)
#define EXRANDOM_UNIT_NORMAL_KAHN_HPP 1

#include <utility>              // for std::pair

#include <exrandom/u_rand.hpp>
#include <exrandom/unit_exponential_dist.hpp>

namespace exrandom {

  /**
   * @brief Sample u-rands from the normal distribution,
   * Kahn's method (<b>deprecated</b>)
   *
   * This samples from the unit normal distribution
   * P(x) = exp(&minus; x<sup>2</sup>/2) / &radic;(2&pi;) using Kahn's method
   * This implements Algorithm K.
   *
   * @tparam digit_gen the type of digit generator.
   *
   * <b>WARNING</b>: One of the steps of this algorithm requires the use of
   * arbitrary precision integer arithmetic.  However in this implementation
   * the arithmetic is carried out with 64-bit integers.  As a result there is
   * occasionally (1 time in 10<sup>9</sup>) overflow which results in a bias
   * in the results.  Use of this class is therefore <b>deprecated</b>.
   *
   * digit_gen::base cannot exceed 16.
   */
  template<typename digit_gen> class unit_normal_kahn {
  public:
    /**
     * The constructor.
     *
     * @param D a reference to the digit generator to be used.
     */
    unit_normal_kahn(digit_gen& D) : _D(D), _x(D), _y(D), _e(D) {}

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
        // Generate a pair of exponential deviates
        _e.generate(g, x); _e.generate(g, _y);
        std::pair<long long, long long> px =  x.rational();
        std::pair<long long, long long> py = _y.rational();
        for (;;) {
          long long nxl = px.first, dx = px.second;
          nxl -= dx;                   // x - 1
          if (nxl < 0LL) nxl = -nxl-1; // abs(x - 1)
          long long nxu = nxl + 1;
          nxl *= nxl; nxu *= nxu; dx *= dx; // (x-1)^2
          long long nyl = py.first, nyu = nyl + 1, dy = py.second;
          // 2 * y
          if (dy % 2 == 0)
            dy /= 2;
          else {
            nyl *= 2; nyu *= 2;
          }
          if (!(dx > 0 && dy > 0))
            // Overflow has happened!  About 8 times out of 10^10 with b = 2.
            break;
          // Put on a common denominator max(dx, dy).  Note that the lesser
          // will always evenly divide the greater (even if dy is divided by 2
          // above).
          if (dy > dx) {
            nxl *= dy/dx; nxu *= dy/dx;
          } else if (dx > dy) {
            nyl *= dx/dy; nyu *= dx/dy;
          }
          if (!(nxl >= 0 && nxu > nxl && nyl >= 0 && nyu > nyl))
            // Overflow has happened!  About 4 times out of 10^10 with b = 2.
            break;
          // compare (nxl,nxu) with (nyl,nyl).  These are open ranges, so the
          // tests below include equality.
          if (nxu <= nyl) {
            // (x - 1)^2 < 2*y, return x with random sign
            if (_y.init().less_than_half(g)) x.negate();
            return;
          }
          if (nyu <= nxl)
            break;              // (x - 1)^2 > 2*y, restart
          // Narrow the widest gap.  In case of a tie, refine x, since that
          // contributes a extra digit in the result.
          if (nxu - nxl >= nyu - nyl)
            px = x.rational(g, x.ndigits() + 1);
          else
            py = _y.rational(g, _y.ndigits() + 1);
        }
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
     * Return the midpoint of the next deviate with a specified number of
     * digits.
     *
     * @tparam RealType the floating point type of the result.
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @param k the minimum number of digits in the fraction.
     * @return the normal deviate.
     *
     * Note the @e k gives the numer of digits in the base @e base.
     */
    template<typename RealType, typename Generator>
    RealType midpoint(Generator& g, size_t k) {
      generate(g, _x);
      return _x.template midpoint<RealType>(g, k);
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
    // Allow base in [2,16]; large bases make overflow more likely.
    static_assert(base && base <= 16, "base must not be greater than 16");
    // Disable copy assignment
    unit_normal_kahn& operator=(const unit_normal_kahn&);
    digit_gen& _D;
    u_rand<digit_gen> _x, _y; // temporary storage
    unit_exponential_dist<digit_gen, true> _e;

  };

}

#endif  // EXRANDOM_UNIT_NORMAL_KAHN_HPP
