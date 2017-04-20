/**
 * @file table_gen.hpp
 * @author Charles Karney <charles.karney@sri.com>
 * @brief Definition of table_gen
 *
 * Copyright (c) Charles Karney (2014) and licensed under the MIT/X11 License.
 * For more information, see http://exrandom.sourceforge.net/
 */

#if !defined(EXRANDOM_TABLE_GEN_HPP)
#define EXRANDOM_TABLE_GEN_HPP 1

#include <string>
#include <algorithm>            // for std::min
#include <stdexcept>            // for std::runtime_error

#include <exrandom/digit_arithmetic.hpp>

namespace exrandom {

  /**
   * @brief a class to generate tabulated random numbers.
   *
   * The "seed" for this generate is a std::string of decimal digits.
   */
  class table_gen {
  public:
    /**
     * The constructor.
     *
     * @param seed the string of random digits.
     */
    table_gen(const std::string& seed = "") : _seed(seed), _pos(0U) {}
    /**
     * Reset the seed.
     *
     * @param seed the new string of random digits.
     */
    void seed(const std::string& seed = "") { _seed = seed; _pos = 0U; }
    /**
     * @return the value of the next random digit.
     *
     * If there are no more digits left, throw an exception.
     */
    uint_t operator()() {
      if (_pos >= _seed.size())
        throw std::runtime_error("table_gen: overflow");
      return (std::min)(uint_t(_seed[_pos++] - '0'), uint_t(9));
    }
  private:
    std::string _seed;
    size_t _pos;
  };

}

#endif  // EXRANDOM_TABLE_GEN_HPP
