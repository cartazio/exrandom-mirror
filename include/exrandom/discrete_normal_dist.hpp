/**
 * @file discrete_normal_dist.hpp
 * @author Charles Karney <charles.karney@sri.com>
 * @brief Definition of discrete_normal_dist
 *
 * Copyright (c) Charles Karney (2014) and licensed under the MIT/X11 License.
 * For more information, see http://exrandom.sourceforge.net/
 */

#if !defined(EXRANDOM_DISCRETE_NORMAL_DIST_HPP)
#define EXRANDOM_DISCRETE_NORMAL_DIST_HPP 1

#include <algorithm>            // for std::min, std::max
#include <cstdlib>              // for std::abs
#include <stdexcept>            // for std::runtime_error
#include <iostream>             // for std::ostream, etc.

#include <exrandom/i_rand.hpp>
#include <exrandom/u_rand.hpp>

namespace exrandom {

  /**
   * @brief Partially sample exactly from the discrete normal distribution.
   *
   * This samples from the discrete normal distribution P<sub>i</sub> &prop;
   * exp[&minus; ((i &minus; &mu;)/&sigma;)<sup>2</sup>/2].  This implements
   * Algorithm D.
   *
   * @tparam digit_gen the type of digit generator.
   *
   * This class allows a i_rand to be returned via the
   * discrete_normal_dist::generate member function or an int
   * result via the discrete_normal_dist::operator()() member function.
   *
   * See discrete_normal_distribution for a simpler interface to sampling
   * discrete normal deviates.  (But, you can't obtain an i_rand with
   * this class.)
   *
   * digit_gen::base must be less than 2<sup>24</sup>.
   */
  template<typename digit_gen> class discrete_normal_dist {
  public:
    /**
     * @brief Hold the parameters of discrete_normal_dist.
     */
    struct param_type {
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
      explicit param_type(int mu_num, int mu_den,
                          int sigma_num, int sigma_den)
      { param_init(mu_num, mu_den, sigma_num, sigma_den); }

      /**
       * The default constructor.
       *
       * Sets &mu; = 0 and &sigma;  = 1.
       */
      explicit param_type()
        : _mu_num(0), _mu_den(1), _sigma_num(1), _sigma_den(1) {}

      /**
       * Construct with integer parameters.
       *
       * @param mu the value of &mu;.
       * @param sigma the value of &sigma;.
       *
       * Sets &mu; = @e mu and &sigma; = @e sigma.
       */
      explicit param_type(int mu, int sigma)
      { param_init(mu, 1, sigma, 1); }

      /**
       * Construct with parameters with a common denominator.
       *
       * @param mu_num the numerator of &mu;.
       * @param sigma_num the numerator of &sigma;.
       * @param den the common denominator.
       *
       * Sets &mu; = @e mu_num / @e den and &sigma; = @e sigma_num / @e den.
       */
      param_type(int mu_num, int sigma_num, int den)
      { param_init(mu_num, den, sigma_num, den); }

      /**
       * @return the numerator of &mu;.
       */
      int mu_num() const { return _mu_num; }
      /**
       * @return the denominator of &mu;.
       */
      int mu_den() const { return _mu_den; }
      /**
       * @return the numerator of &sigma;.
       */
      int sigma_num() const { return _sigma_num; }
      /**
       * @return the denominator of &sigma;.
       */
      int sigma_den() const { return _sigma_den; }

      /**
       * Test for equality.
       *
       * @param p1
       * @param p2
       * @return p1 == p2.
       */
      friend bool
      operator==(const param_type& p1, const param_type& p2) {
        return
          p1._mu_num == p2._mu_num &&
          p1._mu_den == p2._mu_den &&
          p1._sigma_num == p2._sigma_num &&
          p1._sigma_den == p2._sigma_den;
      }

      /**
       * Inserts a param_type @e x into the output stream @e os.
       *
       * @param os an output stream..
       * @param x a param_type.
       * @return os.
       */
      friend std::ostream& operator<<(std::ostream& os, const param_type& x) {
        const auto flags = os.flags();
        os.flags(std::ios::dec);
        os << x.mu_num() << ' ' << x.mu_den() << ' '
           << x.sigma_num() << ' ' << x.sigma_den();
        os.flags(flags);
        return os;
      }

      /**
       * Extracts a param_type @e x from the input stream @e is.
       *
       * @param is an input stream.
       * @param x a param_type.
       * @return is.
       */
      friend std::istream& operator>>(std::istream& is, param_type& x) {
        const auto flags = is.flags();
        is.flags(std::ios::dec | std::ios::skipws);
        int mu_num, mu_den, sigma_num, sigma_den;
        if (is >> mu_num >> mu_den >> sigma_num >> sigma_den)
          x.param_init(mu_num, mu_den, sigma_num, sigma_den);
        is.flags(flags);
        return is;
      }
    private:
      int _mu_num, _mu_den, _sigma_num, _sigma_den;
      void param_init(int mu_num, int mu_den,
                      int sigma_num, int sigma_den) {
        if (!( sigma_num > 0 && sigma_den > 0 && mu_den > 0 &&
               mu_num > std::numeric_limits<int>::min()))
          throw std::runtime_error("discrete_normal_dist: need sigma > 0");
        int l;
        l = gcd(mu_num, mu_den);
        _mu_num = mu_num/l; _mu_den = mu_den/l;
        l = gcd(sigma_num, sigma_den);
        _sigma_num = sigma_num/l; _sigma_den = sigma_den/l;
      }
    };

    /**
     * The default constructor.
     *
     * @param D a reference to the digit generator to be used.
     *
     * Sets &mu; = 0 and &sigma; = 1.
     */
    discrete_normal_dist(digit_gen& D)
      : _D(D), _y(D), _z(D), _j(D), _param()
    { init(); }

    /**
     * Construct from a param_type.
     *
     * @param D a reference to the digit generator to be used.
     * @param p the param_type.
     */
    discrete_normal_dist(digit_gen& D, const param_type& p)
      : _D(D), _y(D), _z(D), _j(D), _param(p) { init(); }

    /**
     * Construct with integer parameters.
     *
     * @param D a reference to the digit generator to be used.
     * @param mu the value of &mu;.
     * @param sigma the value of &sigma;.
     *
     * Sets &mu; = @e mu and &sigma; = @e sigma.
     */
    discrete_normal_dist(digit_gen& D, int mu, int sigma)
      : _D(D), _y(D), _z(D), _j(D), _param(mu, sigma)
    { init(); }

    /**
     * Construct with parameters with a common denominator.
     *
     * @param D a reference to the digit generator to be used.
     * @param mu_num the numerator of &mu;.
     * @param sigma_num the numerator of &sigma;.
     * @param den the common denominator.
     *
     * Sets &mu; = @e mu_num / @e den and &sigma; = @e sigma_num / @e den.
     */
    discrete_normal_dist(digit_gen& D, int mu_num, int sigma_num, int den)
      : _D(D), _y(D), _z(D), _j(D), _param(mu_num, den, sigma_num, den)
    { init(); }

    /**
     * Construct from the individual parameters.
     *
     * @param D a reference to the digit generator to be used.
     * @param mu_num the numerator of &mu;.
     * @param mu_den the denominator of &mu;.
     * @param sigma_num the numerator of &sigma;.
     * @param sigma_den the denominator of &sigma;.
     *
     * Sets &mu; = @e mu_num / @e mu_den and &sigma; = @e sigma_num / @e
     * sigma_den.
     */
    discrete_normal_dist(digit_gen& D,
                    int mu_num, int mu_den,
                    int sigma_num, int sigma_den)
      : _D(D), _y(D), _z(D), _j(D), _param(mu_num, mu_den, sigma_num, sigma_den)
    { init(); }

    /**
     * @return the numerator of &mu;.
     */
    int mu_num() const { return _param.mu_num(); }
    /**
     * @return the denominator of &mu;.
     */
    int mu_den() const { return _param.mu_den(); }
    /**
     * @return the numerator of &sigma;.
     */
    int sigma_num() const { return _param.sigma_num(); }
    /**
     * @return the denominator of &sigma;.
     */
    int sigma_den() const { return _param.sigma_den(); }

    /**
     * Return a deviate as a i_rand.
     *
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @param j the i_rand to set.
     */
    template<typename Generator>
    void generate(Generator& g, i_rand<digit_gen>& j) {
      for (;;) {
        int k = G(g);           // step 1
        // There's addtional scope for optimization if sigma is small, since
        // some values of k never yield an allowed result and be can bail out
        // now.  However, small sigma is not a very likely limit, so we don't
        // muck up the code to treat this case specially.
        if (!P(g, k * (k - 1))) continue; // step 2
        // Explanation of Steps 3 & 5.  The scheme for unit_normal samples k,
        // samples x in [0,1], and (unless rejected) returns s*(k+x).  For the
        // discrete case, we sample x in [0,1) such that s*(k+x) = (i-mu)/sigma
        // or
        //
        //   x = s*(i - mu)/sigma - k
        //
        // The value of i which results in the smallest x >= 0 is
        //
        //   s*i0 = s*ceil(sigma*k + s*mu)
        //
        // so sample
        //
        //   i = s * (i0 + j)
        //
        // where j is uniformly distributed in [0, ceil(sigma)).  The
        // corresponding value of x is
        //
        //   x = (i0 - (sigma*k + s*mu))/sigma + j/sigma
        //     = x0 + j/sigma
        //   x0 = (ceil(sigma*k + s*mu) - (sigma*k + s*mu))/sigma
        int s = j.init(g,2)(g) ? -1 : 1; // step 6
        long long xn0 = _sig * k + s * _mu;
        int i0 = int(iceil(xn0, _d)); // step 5
        xn0 = i0 * _d - xn0;          // step 3, xn = xn0 + j * _d
        j.init(g, _isig);             // i = s * (i0 + j)
        // If sigma is not an integer, this may result (with j = _isig-1) in x
        // >= 1.  Reject such samples.  Reject also the case s = -1, k = 0, and
        // x == 0 (since this is treated by the case s = 1, k = 0, x = 0).
        if (!j.less_than(g, _sig - xn0, _d) ||
            (k == 0 && s < 0 && !j.greater_than(g, -xn0, _d)))
          continue;
        int h = k + 1; while (h-- && B(g, k, xn0, j)) {}; // step 4
        if (!(h < 0)) continue;
        j.add(i0 + s*_imu);     // step 5
        if (s < 0) j.negate();  // step 6
        return;                 // step 7
      }
    }

    /**
     * Return a deviate.
     *
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @return the random deviate.
     */
    template<typename Generator>
    int operator()(Generator& g) {
      generate(g, _j);
      return _j(g);
    }
    /**
     * @return a reference to the digit generator used in the constructor.
     */
    digit_gen& digit_generator() const { return _D; }
    /**
     * @return the parameters.
     */
    const param_type& param() const { return _param; }
    /**
     * Set new parameters.
     *
     * @param param the new parameters.
     */
    void init(const param_type& param) { _param = param; init(); }
  private:
    static const int b = digit_gen::base;
    // Allow base in [2,2^24].  Need digit to be representable as an int.  This
    // also allows kmax * base to be representable as in int.
    static_assert(digit_gen::bits <= 24, "base must be in [2,2^24]");
    // Disable copy assignment
    discrete_normal_dist& operator=(const discrete_normal_dist&);
    digit_gen& _D;
    u_rand<digit_gen> _y, _z; // temporary storage
    i_rand<digit_gen> _j;     // temporary storage
    param_type _param;
    long long _sig, _mu, _d;    // sigma = _sig/_d, mu = _imu + _mu/_d
    int _imu, _isig;            // _isig = ceil(sigma)
    static long long iceil(long long n, long long d) // ceil(n/d) for d > 0
    { long long k = n / d; return k + (k * d < n ? 1 : 0); }
    // Knuth, TAOCP, vol 2, 4.5.2, Algorithm A
    static int gcd(int u, int v) {
      u = std::abs(u); v = std::abs(v);
      while (v > 0) { int r = u % v; u = v; v = r; }
      return u;
    }
    /*    void init(int mu_num, int mu_den, int sigma_num, int sigma_den) {
      _param = param_type(mu_num, mu_den, sigma_num, sigma_den);
      init();
    }
    */
    void init() {
      const long long maxll = std::numeric_limits<long long>::max();
      const int maxint = std::numeric_limits<int>::max();
      _imu = int(_param.mu_num() / _param.mu_den());
      int fmu_num = _param.mu_num() - _imu * _param.mu_den();
      _isig = int(iceil(_param.sigma_num(), _param.sigma_den()));
      long long l = gcd(_param.sigma_den(), _param.mu_den());
      if (!( _param.mu_den() / l <= maxll / _param.sigma_num() &&
             std::abs(fmu_num) <= maxll / (_param.sigma_den() / l) &&
             _param.mu_den() / l <= maxll / _param.sigma_den() ))
        throw std::runtime_error("discrete_normal_dist: sigma or mu overflow");
      _sig = _param.sigma_num() * (_param.mu_den() / l);
      _mu = fmu_num * (_param.sigma_den() / l);
      _d  = _param.sigma_den() * (_param.mu_den() / l);
      // sigma = _sig / _d; _isig = ceil(sigma); check _isig * _d is
      // representable as a long long (in i_rand.less_than)
      if (!(_isig <= maxll / _d))
        throw std::runtime_error("discrete_normal_dist: sigma or mu overflow");
      // The rest of the constructor tests for possible overflow
      // The probability that k =  kmax is about 10^-543.
      int kmax = 50 + 1;
      // Check that max plausible result fits in an int
      if (!(_isig <= maxint / kmax))
        throw std::runtime_error("discrete_normal_dist: possible overflow a");
      if (!(std::abs(_imu) <= maxint - _isig * kmax))
        throw std::runtime_error("discrete_normal_dist: possible overflow b");
      // Need to represent
      //   _sig * kmax as long long -- xn0 = _sig * k ...)
      //   base * 2 * kmax as long long -- in compare(g, 1, 2, m)
      //   _isig * base as long long -- in i_rand::start
      //   _sig * kmax * base as long long -- in u_rand::less_than
      //   _sig * base as long long -- in u_rand::less_than
      // Combine requirements as
      //   max(2,_sig) * base * kmax
      if (!((std::max)(2LL, _sig) <= maxll / (b * kmax)))
        throw std::runtime_error("discrete_normal_dist: possible overflow c");
    }
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

    // Algorithm B: true with prob exp(-x * (2*k + x) / (2*k + 2)) where
    // x = (xn0 + _d * j) / _sig
    template<typename Generator>
    bool B(Generator& g, int k, long long xn0,
           i_rand<digit_gen>& j) {
      int n = 0, m = 2 * k + 2, f;
      for (;; ++n) {
        f = k > 0 ? 0 : _z.init().compare(g, 1, 2, m); if (f < 0) break;
        _z.init();
        if (!(n ? _z.less_than(g, _y) : _z.less_than(g, xn0, _d, _sig, j)))
          break;
        f = k > 0 ? _y.init().compare(g, 1, 2, m) : f; if (f < 0) break;
        if (f == 0 && (!_y.init().less_than(g, xn0, _d, _sig, j))) break;
        _y.swap(_z);            // an efficient way of doing y = z
      }
      return (n % 2) == 0;
    }
  };

}

#endif  // EXRANDOM_DISCRETE_NORMAL_DIST_HPP
