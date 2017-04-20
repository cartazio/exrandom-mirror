#include <iomanip>
#include <limits>
#include <cmath>
#include <exrandom/unit_normal_distribution.hpp>
#include <exrandom/unit_exponential_distribution.hpp>
#include <exrandom/unit_uniform_distribution.hpp>
#include <exrandom/discrete_normal_distribution.hpp>

int main() {
  static_assert(std::numeric_limits<double>::radix == 2 &&
                std::numeric_limits<double>::digits == 53,
                "tests require that doubles have precision of 53 binary bits");
  int retval = 0;
  std::cerr << std::setprecision(14);
  std::mt19937 g(5489u);        // Initialize URNG with default seed
  {
    unsigned long x;
    for (unsigned i = 0; i < 10000; ++i)
      x = g();
    if (x != 4123659995) {
      ++retval;
      std::cerr << "Error in std::mt19937:\n"
                << "  expected 4123659995, got " << x << "\n";
    }
  }
  {
    g.seed(1u);
    volatile double x = 0, y;
    exrandom::unit_uniform_distribution<double> U;
    for (unsigned i = 0; i < 1000000; ++i) {
      y = U(g) - 0.5;
      x += y;
    }
    if (std::abs(x - (-173.53065882716)) > 0.000000000005) {
      ++retval;
      std::cerr << "Error in exrandom::unit_uniform_distribution:\n"
                << "  expected -173.53065882716, got " << x << "\n";
    }
  }
  {
    g.seed(2u);
    volatile double x = 0, y;
    exrandom::unit_exponential_distribution<double> E;
    for (unsigned i = 0; i < 1000000; ++i) {
      y = E(g) - 1.0;
      x += y;
    }
    if (std::abs(x - 708.92395157383) > 0.000000000005) {
      ++retval;
      std::cerr << "Error in exrandom::unit_exponential_distribution:\n"
                << "  expected 708.92395157383, got " << x << "\n";
    }
  }
  {
    g.seed(3u);
    volatile double x = 0;
    exrandom::unit_normal_distribution<double> N;
    for (unsigned i = 0; i < 1000000; ++i)
      x += N(g);
    if (std::abs(x - 332.17627482462) > 0.000000000005) {
      ++retval;
      std::cerr << "Error in exrandom::unit_normal_distribution:\n"
                << "  expected 332.17627482462, got " << x << "\n";
    }
  }
  {
    g.seed(4u);
    long x = 0;
    exrandom::discrete_normal_distribution D(1,3,129,2);
    for (unsigned i = 0; i < 1000000; ++i)
      x += D(g);
    if (x != 316205) {
      ++retval;
      std::cerr << "Error in exrandom::discrete_normal_distribution:\n"
                << "  expected 316205, got " << x << "\n";
    }
  }
  if (retval > 0)
    std::cerr << "Some tests failed" << "\n";
  else
    std::cerr << "Tests completed successfully" << "\n";
  return retval;
}
