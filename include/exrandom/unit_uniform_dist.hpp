/**
 * @file unit_uniform_dist.hpp
 * @author Charles Karney <charles.karney@sri.com>
 * @brief Definition of unit_uniform_dist
 *
 * Copyright (c) Charles Karney (2014) and licensed under the MIT/X11 License.
 * For more information, see http://exrandom.sourceforge.net/
 */

#if !defined(EXRANDOM_UNIT_UNIFORM_DIST_HPP)
#define EXRANDOM_UNIT_UNIFORM_DIST_HPP 1

#include <exrandom/u_rand.hpp>

namespace exrandom {

  /**
   * @brief Sample u-rands exactly from the unit uniform distribution
   *
   * This samples from the unit uniform distribution P(x) = 1, for 0 &lt; x
   * &lt; 1.  This is a trivial call to the u_rand class.
   *
   * @tparam digit_gen the type of digit generator.
   *
   * This class allows a u-rand to be returned via the
   * unit_uniform_dist::generate member function or a floating point
   * result via the unit_uniform_dist::value<RealType> member function.
   *
   * See unit_uniform_distribution for a simpler interface to sampling
   * uniform deviates.  (But, you can't obtain u-rands with this class and
   * the constructor locks you into a specific floating point type.)
   */
  template<typename digit_gen>
  class unit_uniform_dist {
  public:
    /**
     * The constructor.
     *
     * @param D a reference to the digit generator to be used.
     */
    unit_uniform_dist(digit_gen& D) : _D(D), _x(D) {}

    /**
     * Generate the next deviate as a u_rand.
     *
     * @tparam Generator the type of generator.
     * @param[out] x the u_rand to set.
     */
    template<typename Generator>
    void generate(Generator& /*g*/, u_rand<digit_gen>& x)
    { x.init(); return; }

    /**
     * Generate the next deviate and round it to a floating point number.
     *
     * @tparam RealType the floating point type of the result.
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @return the uniform deviate.
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
     * The base of the digit generator
     */
    static const uint_t base = digit_gen::base;
  private:
    static const uint_t bm1 = digit_gen::max_value;
    // Disable copy assignment
    unit_uniform_dist& operator=(const unit_uniform_dist&);
    digit_gen& _D;
    u_rand<digit_gen> _x;  // temporary storage
  };

}

#endif  // EXRANDOM_UNIT_UNIFORM_DIST_HPP
