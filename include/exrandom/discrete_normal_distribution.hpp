/**
 * @file discrete_normal_distribution.hpp
 * @author Charles Karney <charles.karney@sri.com>
 * @brief Definition of discrete_normal_distribution
 *
 * Copyright (c) Charles Karney (2014) and licensed under the MIT/X11 License.
 * For more information, see http://exrandom.sourceforge.net/
 */

#if !defined(EXRANDOM_DISCRETE_NORMAL_DISTRIBUTION_HPP)
#define EXRANDOM_DISCRETE_NORMAL_DISTRIBUTION_HPP 1

#include <iostream>             // for std::ostream, etc.
#include <limits>

#include <exrandom/rand_digit.hpp>
#include <exrandom/discrete_normal_dist.hpp>

namespace exrandom {

  /**
   * @brief Sample exactly from the discrete normal distribution.
   *
   * This samples from the discrete normal distribution P<sub>i</sub> &prop;
   * exp[&minus; ((i &minus; &mu;)/&sigma;)<sup>2</sup>/2], where &mu; and
   * &sigma; &gt; 0 are specified as rational numbers (the ratio of two ints).
   * This implements Algorithm D.
   *
   * This is a wrapper for discrete_normal_dist to turn it into a C++11 style
   * random distribution.  The base for discrete_normal_dist is set to
   * 2<sup>16</sup>.
   */
  class discrete_normal_distribution {
  private:
    static const uint_t _base = 1UL<<16;
  public:
    /**
     * The type of the range of the distribution.
     */
    typedef int result_type;
    /**
     * @brief Parameter type for discrete_normal_distribution.
     */
    struct param_type : discrete_normal_dist<rand_digit<_base>>::param_type {
      /**
       * The type of the random number distribution.
       */
      typedef discrete_normal_distribution distribution_type;

      /**
       * The default constructor.
       *
       * Sets &mu; = 0 and &sigma;  = 1.
       */
      explicit param_type()
        : discrete_normal_dist<rand_digit<_base>>::param_type() {}

      /**
       * Construct with integer parameters.
       *
       * @param mu the value of &mu;.
       * @param sigma the value of &sigma;.
       *
       * Sets &mu; = @e mu and &sigma; = @e sigma.
       */
      explicit param_type(int mu, int sigma)
        : discrete_normal_dist<rand_digit<_base>>::param_type(mu, sigma) {}

      /**
       * Construct with parameters with a common denominator.
       *
       * @param mu_num the numerator of &mu;.
       * @param sigma_num the numerator of &sigma;.
       * @param den the common denominator.
       *
       * Sets &mu; = @e mu_num / @e den and &sigma; = @e sigma_num / @e den.
       */
      explicit param_type(int mu_num, int sigma_num, int den)
        : discrete_normal_dist<rand_digit<_base>>::param_type
        (mu_num, sigma_num, den) {}
      /**
       * Construct from the individual parameters.
       *
       * @param mu_num the numerator of &mu;.
       * @param mu_den the denominator of &mu;.
       * @param sigma_num the numerator of &sigma;.
       * @param sigma_den the denominator of &sigma;.
       *
       * Sets &mu; = @e mu_num / @e mu_den and &sigma; = @e sigma_num
       * / @e sigma_den.
       */
      explicit param_type(int mu_num, int mu_den, int sigma_num, int sigma_den)
        : discrete_normal_dist<rand_digit<_base>>::param_type
        (mu_num, mu_den, sigma_num, sigma_den) {}
    };

    /**
     * The default constructor.
     *
     * Sets &mu; = 0 and &sigma; = 1
     */
    explicit discrete_normal_distribution() : _normal_dist(_D) {}

    /**
     * Construct from a param_type.
     *
     * @param p
     */
    explicit discrete_normal_distribution(const param_type& p)
      : _param(p), _normal_dist(_D, _param) {}

    /**
     * Construct with integer parameters.
     *
     * @param mu the value of &mu;.
     * @param sigma the value of &sigma;.
     *
     * Sets &mu; = @e mu and &sigma; = @e sigma.
     */
    explicit discrete_normal_distribution(int mu, int sigma)
      : _param(mu, sigma), _normal_dist(_D, _param) {}

    /**
     * Construct with parameters with a common denominator.
     *
     * @param mu_num the numerator of &mu;.
     * @param sigma_num the numerator of &sigma;.
     * @param den the common denominator.
     *
     * Sets &mu; = @e mu_num / @e den and &sigma; = @e sigma_num
     * / @e den.
     */
    explicit
    discrete_normal_distribution(int mu_num, int sigma_num, int den)
      : _param(mu_num, sigma_num, den), _normal_dist(_D, _param) {}

    /**
     * Construct from the individual parameters.
     *
     * @param mu_num the numerator of &mu;.
     * @param mu_den the denominator of &mu;.
     * @param sigma_num the numerator of &sigma;.
     * @param sigma_den the denominator of &sigma;.
     *
     * Sets &mu; = @e mu_num / @e mu_den and &sigma; = @e sigma_num / @e
     * sigma_den.
     */
    explicit
    discrete_normal_distribution(int mu_num, int mu_den,
                                 int sigma_num, int sigma_den)
      : _param(mu_num, mu_den, sigma_num, sigma_den)
      , _normal_dist(_D, _param) {}

    /**
     * Resets the distribution state.
     */
    void reset() {}

    /**
     * @return the numerator of &mu;.
     */
    result_type mu_num() const { return _param.mu_num(); }
    /**
     * @return the denominator of &mu;.
     */
    result_type mu_den() const { return _param.mu_den(); }
    /**
     * @return the numerator of &sigma;.
     */
    result_type sigma_num() const { return _param.sigma_num(); }
    /**
     * @return the denominator of &sigma;.
     */
    result_type sigma_den() const { return _param.sigma_den(); }

    /**
     * @return the parameter set of the distribution.
     */
    param_type param() const { return _param; }

    /**
     * Sets the parameter set of the distribution.
     * @param param The new parameter set of the distribution.
     */
    void param(const param_type& param)
    { _param = param; _normal_dist.init(_param); }

    /**
     * @return the greatest lower bound value of the distribution.
     */
    result_type min() const
    { return std::numeric_limits<result_type>::min(); }

    /**
     * @return the least upper bound value of the distribution.
     */
    result_type max() const
    { return std::numeric_limits<result_type>::max(); }

    /**
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @return a discrete normal deviate.
     */
    template<typename Generator>
    result_type operator()(Generator& g)
    { return _normal_dist(g); }

    /**
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @param p a parameter set.
     * @return a discrete normal deviate using the specified parameters.
     */
    template<typename Generator>
    result_type operator()(Generator& g, const param_type& p) {
      discrete_normal_dist<rand_digit<_base>> N(_D, p);
      return N(g);
    }

  /**
   * @return true if two normal distributions have the same parameters.
   */
  friend bool
  operator==(const discrete_normal_distribution& d1,
             const discrete_normal_distribution& d2)
  { return d1.param() == d2.param(); }

  /**
   * @return false if two normal distributions have the same parameters.
   */
  friend bool
  operator!=(const discrete_normal_distribution& d1,
             const discrete_normal_distribution& d2)
  { return !(d1 == d2); }

  /**
   * Inserts a discrete_normal_distribution random number distribution
   * @e x into the output stream @e os.
   *
   * @param os an output stream.
   * @param x a discrete_normal_distribution random number distribution.
   * @return os.
   */
  friend std::ostream&
  operator<<(std::ostream& os, const discrete_normal_distribution& x)
  { os << x.param(); return os; }

  /**
   * Extracts a discrete_normal_distribution random number distribution @e x
   * from the input stream @e is.
   *
   * @param is an input stream.
   * @param x a discrete_normal_distribution random number generator engine.
   * @return is.
   */
  friend std::istream&
  operator>>(std::istream& is, discrete_normal_distribution& x) {
    discrete_normal_distribution::param_type p;
    is >> p;
    x.param(p);
    return is;
  }

  private:
    param_type  _param;
    rand_digit<_base> _D;
    discrete_normal_dist<rand_digit<_base>> _normal_dist;
  };

}

#endif  // EXRANDOM_DISCRETE_NORMAL_DISTRIBUTION_HPP
