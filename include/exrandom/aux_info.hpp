/**
 * @file aux_info.hpp
 * @author Charles Karney <charles.karney@sri.com>
 * @brief Definition of aux_info
 *
 * Copyright (c) Charles Karney (2014) and licensed under the MIT/X11 License.
 * For more information, see http://exrandom.sourceforge.net/
 */

#if !defined(EXRANDOM_AUX_INFO_HPP)
#define EXRANDOM_AUX_INFO_HPP 1

#include <exrandom/exrandom_config.hpp>

#include <cmath>
#include <limits>               // For numeric_limits::quiet_NaN

namespace exrandom {

  /**
   * @brief Some auxiliary functions related to normal and exponential
   * distributions.
   *
   * These are implemented as static functions.  These aren't only needed to
   * implement the chi-squared tests and to compute the cost and toll of the
   * algorithms.
   */
  class aux_info {
  public:
    /**
     * @return the value of pi.
     */
    static double pi() { return std::atan2(0.0, -1.0); }

    /**
     * The cumulative uniform function.
     *
     * @param x
     * @return the cumulative probabilty at @e x.
     */
    static double cumulative_uniform(double x)
    { return x < 0 ? 0 : x < 1 ? x : 1; }

    /**
     * The cumulative exponential function.
     *
     * @param x
     * @return the cumulative probabilty at @e x.
     */
    static double cumulative_exponential(double x) {
#if EXRANDOM_CXX11_MATH
      return x > 0 ? -std::expm1(-x) : 0;
#else
      return x > 0 ? 1 - std::exp(-x) : 0;
#endif
    }

    /**
     * The cumulative normal function.
     *
     * @param x
     * @return the cumulative probabilty at @e x.
     */
#if defined(DOXYGEN) || EXRANDOM_CXX11_MATH
    static double cumulative_normal(double x)
    { return std::erf(x / std::sqrt(2.0)) / 2; }
#else
    static double cumulative_normal(double /*x*/)
    { return std::numeric_limits<double>::quiet_NaN(); }
#endif

    /**
     * @return the entropy of the unit uniform distribution.
     */
    static double uniform_entropy() { return 0; }

    /**
     * @return the entropy of the unit exponential distribution.
     */
    static double exponential_entropy() { return 1; }

    /**
     * @return the entropy of the unit exponential distribution.
     */
    static double normal_entropy() { return std::log(2 * pi()) / 2 + 0.5; }

    /**
     * @return the mean exponent of the floating point representation of a unit
     *   uniform deviate.
     */
    static double uniform_mean_exponent() { return -1; }

    /**
     * @return the mean exponent of the floating point representation of a unit
     *   exponential deviate.
     */
    static double exponential_mean_exponent() {
      double z = 0;
      double y0 = cumulative_exponential(0);
      for (int k = -std::numeric_limits<double>::digits;; ++k) {
        double y1 = cumulative_exponential(std::pow(2.0, k));
        z += k * (y1 - y0);
        if (!(y1 < 1)) break;   // Make sure NaN causes an exit
        y0 = y1;
      }
      return z;
    }

    /**
     * @return the mean exponent of the floating point representation of a unit
     *   normal deviate.
     */
    static double normal_mean_exponent() {
      double z = 0;
      // times 2 to account for negative values too
      double y0 = 2 * cumulative_normal(0);
      for (int k = -40;; ++k) {
        double y1 = 2 * cumulative_normal(std::pow(2.0, k));
        z += k * (y1 - y0);
        if (!(y1 < 1)) break;   // Make sure NaN causes an exit
        y0 = y1;
      }
      return z;
    }

    /**
     * @tparam param_type the type of the discrete_normal_dist::param_type.
     * @param p the parameters for the discrete_normal_dist.
     * @param i
     * @param norm the normalizing constant.
     * @return the probabilty at @e i (unnormalized if norm = 1).
     */
    template <typename param_type>
    static double
      discrete_normal_prob(const param_type& p, int i, double norm = 1) {
      double mu = p.mu_num() / double( p.mu_den () ),
        sigma = p.sigma_num() / double( p.sigma_den () ),
        x = (i - mu)/sigma;
      return std::exp( - x * x / 2) / norm;
    }
    /**
     * @tparam param_type the type of the discrete_normal_dist::param_type.
     * @param p the parameters for the discrete_normal_dist.
     * @param H the entropy of the distribution.
     * @return the normalizing constant.
     */
    template <typename param_type>
    static double discrete_normal_norm(const param_type& p, double& H) {
      int imu = p.mu_num() / p.mu_den(),
        isig = ( p.sigma_num() + p.sigma_den() - 1 ) / p.sigma_den();
      double s;
      if (isig < 10) {
        s = 0; H = 0;
        for (int i = imu - 10 * isig; i < imu + 10 * isig; ++i) {
          double P = discrete_normal_prob(p, i, 1.0);
          s += P;
          if (P > 0)
            H -= P * std::log(P);
        }
        H = H/s + std::log(s);
      } else {
        // Continuous approximations are very good
        double sigma = p.sigma_num() / double(p.sigma_den());
        s = std::sqrt(2 * pi()) * sigma;
        H = std::log(s) + 0.5;
      }
      return s;
    }
  };

}

#endif  // EXRANDOM_AUX_INFO_HPP
