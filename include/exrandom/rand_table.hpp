/**
 * @file rand_table.hpp
 * @author Charles Karney <charles.karney@sri.com>
 * @brief Definition of rand_table
 *
 * Copyright (c) Charles Karney (2014) and licensed under the MIT/X11 License.
 * For more information, see http://exrandom.sourceforge.net/
 */

#if !defined(EXRANDOM_RAND_TABLE_HPP)
#define EXRANDOM_RAND_TABLE_HPP 1

#include <exrandom/digit_arithmetic.hpp>

namespace exrandom {

  /**
   * @brief a wrapper to table_gen to make it look like a rand_digit.
   */
  class rand_table {
  public:
    /**
     * The base for the digits, 10.
     */
    static const uint_t base = 10U;
    /**
     * The minimum value produced by operator().
     */
    static const uint_t min_value = 0U;
    /**
     * The maximum value produced by operator().
     */
    static const uint_t max_value = base - 1U;
    /**
     * The number of bits needed to represent a digit.
     */
    static const int bits = 4;
    /**
     * Is the base a power of two?
     */
    static const bool power_of_two = false;
    /**
     * The constructor (which initializes the count to 0).
     */
    rand_table() : _count(0) {};
    /**
     * @tparam Generator the type of g.
     * @param g the random generator engine.
     * @return the random digit.
     */
    template<typename Generator>
    uint_t operator()(Generator& g)
    { ++_count; return g(); }
    /**
     * @return the count.
     */
    long long count() const { return _count; }
    /**
     * @tparam the floating point type of the result.
     * @return 1/@e base as a floating point number.
     */
    template<typename RealType>
    static RealType invrealbase() { return 1/RealType(base); }
  private:
    long long _count;
  };

}

#endif  // EXRANDOM_RAND_TABLE_HPP
