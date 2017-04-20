/**
 * @file unit_exponential_distribution.hpp
 * @author Charles Karney <charles.karney@sri.com>
 * @brief Definition of unit_exponential_distribution
 *
 * Copyright (c) Charles Karney (2014) and licensed under the MIT/X11 License.
 * For more information, see http://exrandom.sourceforge.net/
 */

#if !defined(EXRANDOM_UNIT_EXPONENTIAL_DISTRIBUTION_HPP)
#define EXRANDOM_UNIT_EXPONENTIAL_DISTRIBUTION_HPP 1

#include <iostream>             // for std::ostream, etc.
#include <limits>

#include <exrandom/rand_digit.hpp>
#include <exrandom/unit_exponential_dist.hpp>

namespace exrandom {

  /**
   * @brief Sample exactly from the unit exponential distribution.
   *
   * This samples from the unit exponential distribution P(x) = exp(&minus;x)
   * for x &gt; 0.  This is a replacement for std::exponential_distribution
   * (with no parameters).  It is equivalent to sampling a real number exactly
   * from the distribution and rounding it to a floating point number.  This
   * implements Algorithm V.
   *
   * @tparam RealType the floating point type of the resulting deviates.  This
   *   can include various multi-precision floating point types; see
   *   u_rand::value of details.
   *
   * This is a wrapper for unit_exponential_dist to turn it into a C++11 style
   * random distribution.  If the radix of RealType is 2 (the usual case), then
   * the base for unit_exponential_dist is set to 2<sup>32</sup>; otherwise
   * (e.g., RealType is a decimal system), the base is set to the radix.
   */
  template<typename RealType = double>
  class unit_exponential_distribution {
  public:
    /**
     * The type of the range of the distribution.
     */
    typedef RealType result_type;
    /**
     * @brief Parameter type for unit_exponential_distribution.
     *
     * This holds no information because this distribution takes no parameters.
     */
    struct param_type {
      /**
       * The type of the random number distribution.
       */
      typedef unit_exponential_distribution<RealType> distribution_type;
    };

    /**
     * Constructs an exponential distribution.
     */
    explicit
    unit_exponential_distribution() : _exponential_dist(_D) {}

    /**
     * Constructs an exponential distribution with a parameter.
     *
     * The parameter is ignored because it has no state.
     */
    explicit
    unit_exponential_distribution(const param_type& /*p*/)
      : _exponential_dist(_D) {}

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
    result_type max() const { return std::numeric_limits<result_type>::max(); }

    /**
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @return an exponential exponential deviate.
     */
    template<typename Generator>
    result_type
    operator()(Generator& g)
    { return _exponential_dist.template value<result_type>(g); }

    /**
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @return an exponential exponential deviate.
     */
    template<typename Generator>
    result_type
    operator()(Generator& g, const param_type& /*p*/)
    { return this->operator()(g); }

  /**
   * Compare two unit_exponential_distributions.
   * @return true.
   */
  friend bool
  operator==(const unit_exponential_distribution<RealType>& /*d1*/,
             const unit_exponential_distribution<RealType>& /*d2*/)
  { return true; }

  /**
   * Contrast two unit_exponential_distributions.
   * @return false.
   */
  friend bool
  operator!=(const unit_exponential_distribution<RealType>& /*d1*/,
             const unit_exponential_distribution<RealType>& /*d2*/)
  { return false; }

  /**
   * Inserts a unit_exponential_distribution random number distribution into the
   * output stream @e os.
   *
   * @param os an output stream.
   * @return os.
   *
   * This function does nothing because this distribution has no state.
   */
  friend std::ostream&
  operator<<(std::ostream& os,
             const unit_exponential_distribution<RealType>& /*x*/)
  { return os; }

  /**
   * Extracts a unit_exponential_distribution random number distribution from
   * the input stream @e is.
   *
   * @param is an input stream.
   * @return is.
   *
   * This function does nothing because this distribution has no state.
   */
  friend std::istream&
  operator>>(std::istream& is, unit_exponential_distribution& /*x*/)
  { return is; }

  private:
    static_assert(!std::numeric_limits<RealType>::is_integer,
                  "template argument not a floating point type");
    param_type  _param;
    static const uint_t _base = std::numeric_limits<RealType>::radix ==  2 ?
      0UL : std::numeric_limits<RealType>::radix;
    rand_digit<_base> _D;
    // Setting bit_optimized ends up being slightly slower with base = 2^32
    unit_exponential_dist<rand_digit<_base>, _base != 0UL> _exponential_dist;
  };

}

#endif  // EXRANDOM_UNIT_EXPONENTIAL_DISTRIBUTION_HPP
