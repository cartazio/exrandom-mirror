/**
 * @file i_rand.hpp
 * @author Charles Karney <charles.karney@sri.com>
 * @brief Definition of i_rand
 *
 * Copyright (c) Charles Karney (2014) and licensed under the MIT/X11 License.
 * For more information, see http://exrandom.sourceforge.net/
 */

#if !defined(EXRANDOM_I_RAND_HPP)
#define EXRANDOM_I_RAND_HPP 1

#include <string>               // for print routines
#include <sstream>              // for print routines
#include <iostream>             // for std::ostream, etc.

namespace exrandom {

  /**
   * @brief A class to sample integers [0,m).
   *
   * The basic method for sampling is taken from J. Lumbroso (2013),
   * <a href="http://arxiv.org/abs/1304.1916">arxiv:1304.1916</a> generalized
   * to accept random digits in an arbitrary base, b.  However, an important
   * additional feature is that only sufficient random digits are drawn to
   * narrow the allowed range to a power of b.  Thus, for b = 2, after
   * initializing with m = 9 the i_rand represents
   *
   *    range prob
   *    [0,8) 32/63
   *    [0,2)  2/21
   *    [2,6)  4/21
   *    [6,8)  2/21
   *    [8,9)  1/9
   *
   * min() and max() give the current extent of the closed range.  The number
   * of additional random digits needed to fix the value is given by entropy().
   * The comparison operations may require additional digits to be drawn and so
   * the range might be narrowed down, e.g., to [4,8).  If you need a definite
   * value then use operator()().
   *
   * @tparam digit_gen the type of digit generator.
   */
  template<typename digit_gen> class i_rand {
  public:
    explicit
    /**
     * The constructor.
     *
     * @param D A reference to the digit generator to be used.
     *
     * Initializes to a random integer in [0,1), i.e., to 0.
     */
    i_rand(digit_gen& D)
      : _a(0), _d(1), _l(0), _D(D) {}
    /**
     * Initialize.
     *
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @param m this must be greater than 0.
     * @return *this representing a integer uniform distributed in [0,m).
     *
     * If @e m is less than 1, it is treated as
     */
    template<typename Generator>
    i_rand& init(Generator& g, int m) {
      if (m <= 0) m = 1;
      for (long long v = 1, c = 0;;) {
        _l = 0;
        for (long long w = v, a = c, d = 1;;) {
          // play out Lumbroso's algorithm without drawing random digits with w
          // playing the role of v and c represented by the range [a, a + d).
          // Return if both ends of range qualify as return values at the same
          // time.  Otherwise, fail and draw another random digit.
          if (w >= m) {
            long long j = (a / m) * m; a -= j; w -= j;
            if (w >= m) {
              if (a + d <= m) { _a = int(a); _d = int(d); return *this; }
              break;
            }
          }
          w *= b; a *= b; d *= b; ++_l;
        }
        long long j = (v / m) * m; v -= j; c -= j;
        v *= b; c *= b; c += _D(g);
      }
    }
    /**
     * Sample enough digits to narrow the range to an integer.
     *
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @return value of the random integer.
     */
    template<typename Generator>
    int operator()(Generator& g)
    { while (_l) refine(g); return _a; }
    /**
     * @return the current lower end of the range.
     */
    int min() const { return _a; }
    /**
     * @return the current upper end of the range.
     */
    int max() const { return _a + _d - 1; }
    /**
     * @return the number of digits needs to complete the sampling.
     */
    int entropy() const { return _l; }
    /**
     * Negate the range.
     */
    void negate() { _a = -max(); }
    /**
     * Add a constant to the range.
     *
     * @param c the constant to be added.
     */
    void add(int c) { _a += c; }
    /**
     * Test *this &lt; m/n.
     *
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @param m the numerator of the fraction.
     * @param n the denominate of the fraction (default value 1).
     * @return *this &lt; m/n.
     *
     * Requires that n &gt; 0.
     */
    template<typename Generator>
    bool less_than(Generator& g, long long m, long long n = 1) {
      for (;;) {
        if ( (n * max() < m)) return true;
        if (!(n * min() < m)) return false;
        refine(g);
      }
    }
    /**
     * Test *this &le; m/n.
     *
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @param m the numerator of the fraction.
     * @param n the denominator of the fraction (default value 1).
     * @return *this &le; m/n.
     *
     * Requires that n &gt; 0.
     */
    template<typename Generator>
    bool less_than_equal(Generator& g, long long m, long long n = 1)
    { return less_than(g, m + 1, n); }
    /**
     * Test *this &gt; m/n.
     *
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @param m the numerator of the fraction.
     * @param n the denominator of the fraction (default value 1).
     * @return *this &gt; m/n.
     *
     * Requires that n &gt; 0.
     */
    template<typename Generator>
    bool greater_than(Generator& g, long long m, long long n = 1)
    { return !less_than_equal(g, m, n); }
    /**
     * Test *this &ge; m/n.
     *
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @param m the numerator of the fraction.
     * @param n the denominator of the fraction (default value 1).
     * @return *this &ge; m/n.
     *
     * Requires that n &gt; 0.
     */
    template<typename Generator>
    bool greater_than_equal(Generator& g, long long m, long long n = 1)
    { return !less_than(g, m, n); }
    /**
     * @return a string representing the range.
     *
     * If the entropy is zero, the number is printed.  Otherwise the range is
     * represented by min()+[0,max()-min()+1).  (Note that max()-min()+1 is a
     * power of the base.)
     */
    std::string print() const {
      std::ostringstream os;
      if (_l)
        os << min() << "+[0," << max() - min() + 1 << ')';
      else
        os << min();
      return os.str();
    }
    /**
     * Refine the range by sampling an extra digit.
     *
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     */
    template<typename Generator>
    void refine(Generator& g) {
      if (_l > 0) { --_l; _d /= b; _a += _D(g) * _d; }
    }

    /**
     * Print an i_rand.  Format is min+[0,max-min+1) unless the entropy is
     * zero, in which case it's val.
     *
     * @param os an output stream.
     * @param h an i_rand.
     * @return os.
     **********************************************************************/
    friend std::ostream& operator<<(std::ostream& os, const i_rand& h)
    { os << h.print(); return os; }

  private:
    static const int b = digit_gen::base;
    int _a, _d, _l;             // current range is _a + [0, _d); _d = b^_l.
    // Disable copy assignment
    i_rand& operator=(const i_rand&);
    digit_gen& _D;
  };

}

#endif  // EXRANDOM_I_RAND_HPP
