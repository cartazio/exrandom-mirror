/**
 * @file unit_uniform_distribution.hpp
 * @author Charles Karney <charles.karney@sri.com>
 * @brief Definition of unit_uniform_distribution
 *
 * Copyright (c) Charles Karney (2014) and licensed under the MIT/X11 License.
 * For more information, see http://exrandom.sourceforge.net/
 */

#if !defined(EXRANDOM_UNIT_UNIFORM_DISTRIBUTION_HPP)
#define EXRANDOM_UNIT_UNIFORM_DISTRIBUTION_HPP 1

#include <iostream>             // for std::ostream, etc.
#include <limits>

#include <exrandom/rand_digit.hpp>
#include <exrandom/unit_uniform_dist.hpp>

namespace exrandom {

  /**
   * @brief Sample exactly from the unit uniform distribution.
   *
   * This samples from the unit uniform distribution P(x) = 1, for 0 &lt; x
   * &lt; 1.  This is a replacement for std::uniform_real_distribution (with no
   * parameters).  It is equivalent to sampling a real number exactly from the
   * distribution and rounding it to a floating point number.  With the usual
   * rule for rounding (round to nearest), this means that the end points of
   * the interval can be returned, i.e., the sampling is from the
   * <i>closed</i> interval, [0,1].
   *
   * @tparam RealType the floating point type of the resulting deviates.  This
   *   can include various multi-precision floating point types; see
   *   u_rand::value of details.
   *
   * This is a wrapper for unit_uniform_dist to turn it into a C++11 style
   * random distribution.  If the radix of RealType is 2 (the usual case), then
   * the base for unit_uniform_dist is set to 2<sup>32</sup>; otherwise (e.g.,
   * RealType is a decimal system), the base is set to the radix.
   */
  template<typename RealType = double>
  class unit_uniform_distribution {
  public:
    /**
     * The type of the range of the distribution.
     */
    typedef RealType result_type;
    /**
     * @brief Parameter type for unit_uniform_distribution.
     *
     * This holds no information because this distribution takes no parameters.
     */
    struct param_type {
      /**
       * The type of the random number distribution.
       */
      typedef unit_uniform_distribution<RealType> distribution_type;
    };

    /**
     * Constructs an uniform distribution.
     */
    explicit
    unit_uniform_distribution() : _uniform_dist(_D) {}

    /**
     * Constructs an uniform distribution with a parameter.
     *
     * The parameter is ignored because it has no state.
     */
    explicit
    unit_uniform_distribution(const param_type& /*p*/)
      : _uniform_dist(_D) {}

    /**
     * Resets the distribution state.
     */
    void reset() {}

    /**
     * @return the parameter set of the distribution.
     */
    param_type param() const { return param_type(); }

    /**
     * Sets the parameter set of the distribution.
     *
     * The function does not because a param_type has no state.
     */
    void param(const param_type& /*param*/) {}

    /**
     * @return the greatest lower bound value of the distribution.
     */
    result_type min() const { return result_type(0); }

    /**
     * @return the least upper bound value of the distribution.
     */
    result_type max() const { return result_type(1); }

    /**
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @return an uniform uniform deviate.
     */
    template<typename Generator>
    result_type
    operator()(Generator& g)
    { return _uniform_dist.template value<result_type>(g); }

    /**
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @return an uniform uniform deviate.
     */
    template<typename Generator>
    result_type
    operator()(Generator& g, const param_type& /*p*/)
    { return this->operator()(g); }

  /**
   * Compare two unit_uniform_distributions.
   * @return true.
   */
  friend bool
  operator==(const unit_uniform_distribution<RealType>& /*d1*/,
             const unit_uniform_distribution<RealType>& /*d2*/)
  { return true; }

  /**
   * Contrast two unit_uniform_distributions.
   * @return false.
   */
  friend bool
  operator!=(const unit_uniform_distribution<RealType>& /*d1*/,
             const unit_uniform_distribution<RealType>& /*d2*/)
  { return false; }

  /**
   * Inserts a unit_uniform_distribution random number distribution into the
   * output stream @e os.
   *
   * @param os an output stream.
   * @return os.
   *
   * This function does nothing because this distribution has no state.
   */
  friend std::ostream&
  operator<<(std::ostream& os,
             const unit_uniform_distribution<RealType>& /*x*/)
  { return os; }

  /**
   * Extracts a unit_uniform_distribution random number distribution from the
   * input stream @e is.
   *
   * @param is an input stream.
   * @return is.
   *
   * This function does nothing because this distribution has no state.
   */
  friend std::istream&
  operator>>(std::istream& is, unit_uniform_distribution& /*x*/)
  { return is; }

  private:
    static_assert(!std::numeric_limits<RealType>::is_integer,
                  "template argument not a floating point type");
    param_type  _param;
    static const uint_t _base = std::numeric_limits<RealType>::radix ==  2 ?
      0UL : std::numeric_limits<RealType>::radix;
    rand_digit<_base> _D;
    unit_uniform_dist<rand_digit<_base>> _uniform_dist;
  };

}

#endif  // EXRANDOM_UNIT_UNIFORM_DISTRIBUTION_HPP
