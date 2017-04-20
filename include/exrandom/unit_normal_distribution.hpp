/**
 * @file unit_normal_distribution.hpp
 * @author Charles Karney <charles.karney@sri.com>
 * @brief Definiton of unit_normal_distribution
 *
 * Copyright (c) Charles Karney (2014) and licensed under the MIT/X11 License.
 * For more information, see http://exrandom.sourceforge.net/
 */

#if !defined(EXRANDOM_UNIT_NORMAL_DISTRIBUTION_HPP)
#define EXRANDOM_UNIT_NORMAL_DISTRIBUTION_HPP 1

/**
 * @relates exrandom::unit_normal_distribution
 *
 * Should exrandom::unit_normal_distribution use exrandom::unit_normal_kahn
 * instead of exrandom::unit_normal_dist as the underlying distribution.
 * Because the output of exrandom::unit_normal_kahn is slightly biased,
 * EXRANDOM_USE_KAHN should be set to 0.
 */
#define EXRANDOM_USE_KAHN 0

#include <iostream>             // for std::ostream, etc.
#include <limits>

#include <exrandom/rand_digit.hpp>
#if EXRANDOM_USE_KAHN
#include <exrandom/unit_normal_kahn.hpp>
#else
#include <exrandom/unit_normal_dist.hpp>
#endif

namespace exrandom {

  /**
   * @brief Sample exactly from the unit normal distribution.
   *
   * This samples from the unit normal distribution
   * P(x) = exp(&minus; x<sup>2</sup>/2) / &radic;(2&pi;).
   * This is a replacement for std::normal_distribution (with no parameters).
   * It is equivalent to sampling a real number exactly from the distribution
   * and rounding it to a floating point number.  This implements Algorithm N.
   *
   * @tparam RealType the floating point type of the resulting deviates.  This
   *   can include various multi-precision floating point types; see
   *   u_rand::value of details.
   *
   * This is a wrapper for unit_normal_dist to turn it into a C++11 style
   * random distribution.  If the radix of RealType is 2 (the usual case), then
   * the base for unit_normal_dist is set to 2<sup>32</sup>; otherwise (e.g.,
   * RealType is a decimal system), the base is set to the radix.
   *
   * If the macro EXRANDOM_USE_KAHN is set to 1, then unit_normal_kahn is used
   * instead (with the base set to the radix for RealType).  Because this
   * introduces a slight bias, this is not recommended.
   *
   * @tparam RealType the floating point type of the resulting deviates.
   */
  template<typename RealType = double>
  class unit_normal_distribution {
  public:
    /**
     * The type of the range of the distribution.
     */
    typedef RealType result_type;
    /**
     * @brief Parameter type for unit_normal_distribution.
     *
     * This holds no information because this distribution takes no parameters.
     */
    struct param_type {
      /**
       * The type of the random number distribution.
       */
      typedef unit_normal_distribution<RealType> distribution_type;
    };
    /**
     * Constructs a normal distribution.
     */
    explicit
    unit_normal_distribution() : _normal_dist(_D) {}

    /**
     * Constructs a normal distribution with a parameter.
     *
     * The parameter is ignored because it has no state.
     */
    explicit
    unit_normal_distribution(const param_type& /*p*/)
      : _normal_dist(_D) {}

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
    result_type min() const
    { return std::numeric_limits<result_type>::lowest(); }

    /**
     * @return the least upper bound value of the distribution.
     */
    result_type max() const { return std::numeric_limits<result_type>::max(); }

    /**
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @return a normal deviate.
     */
    template<typename Generator>
    result_type operator()(Generator& g)
    { return _normal_dist.template value<result_type>(g); }

    /**
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @return a normal deviate.
     */
    template<typename Generator>
    result_type operator()(Generator& g, const param_type& /*p*/)
    { return this->operator()(g); }

  /**
   * Compare two unit_normal_distributions.
   * @return true.
   */
  friend bool
  operator==(const unit_normal_distribution<RealType>& /*d1*/,
             const unit_normal_distribution<RealType>& /*d2*/)
  { return true; }

  /**
   * Contrast two unit_normal_distributions.
   * @return false.
   */
  friend bool
  operator!=(const unit_normal_distribution<RealType>& /*d1*/,
             const unit_normal_distribution<RealType>& /*d2*/)
  { return false; }

  /**
   * Inserts a unit_normal_distribution random number distribution into the
   * output stream @e os.
   *
   * @param os an output stream.
   * @return os.
   *
   * This function does nothing because this distribution has no state.
   */
  friend std::ostream&
  operator<<(std::ostream& os, const unit_normal_distribution<RealType>& /*x*/)
  { return os; }

  /**
   * Extracts a unit_normal_distribution random number distribution from the
   * input stream @e is.
   *
   * @param is an input stream.
   * @return is.
   *
   * This function does nothing because this distribution has no state.
   */
  friend std::istream&
  operator>>(std::istream& is, unit_normal_distribution& /*x*/)
  { return is; }

  private:
    static_assert(!std::numeric_limits<RealType>::is_integer,
                  "template argument not a floating point type");
    param_type  _param;
#if EXRANDOM_USE_KAHN
    static const uint_t _base = std::numeric_limits<RealType>::radix;
    rand_digit<_base> _D;
    unit_normal_kahn<rand_digit<_base>> _normal_dist;
#else
    static const uint_t _base = std::numeric_limits<RealType>::radix ==  2 ?
      0UL : std::numeric_limits<RealType>::radix;
    rand_digit<_base> _D;
    unit_normal_dist<rand_digit<_base>> _normal_dist;
#endif
  };

}

#endif  // EXRANDOM_UNIT_NORMAL_DISTRIBUTION_HPP
