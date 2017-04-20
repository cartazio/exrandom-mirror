#include <iostream>
#include <iomanip>
#include <random>
#include <exrandom/rand_digit.hpp>
#include <exrandom/unit_normal_dist.hpp>

int main() {
  std::cout << "Sampling from the normal distribution\n";
  unsigned s = std::random_device()(); // Set seed from random_device
  std::mt19937 g(s);                   // Initialize URNG
  std::cout << "Seed set to " << s << "\n\n";

  {
    const unsigned b = 16;
    std::cout << "Deviates rounded to floating point with base = " << b << "\n"
              << "columns are: initial u-rand, rounded float, final u-rand\n"
              << std::setprecision(std::numeric_limits<float>::digits10);
    exrandom::rand_digit<b> D;
    exrandom::unit_normal_dist<exrandom::rand_digit<b>> N(D);
    exrandom::u_rand<exrandom::rand_digit<b>> x(N.digit_generator());
    for (int i = 0; i < 10; ++i) {
      N.generate(g, x);
      std::cout << x << " = ";
      std::cout << x.value<float>(g) << " = ";
      std::cout << x << "\n";
    }
    std::cout << "Total number of base " << b << " digits used = "
              << D.count() << "\n\n";
  }

  {
    const unsigned b = 10;
    int p = 6;
    std::cout << "Deviates rounded to fixed point with base = " << b
              << " and precision = " << p << "\n"
              << "columns are: initial u-rand, rounded fixed, final u-rand\n";
    exrandom::rand_digit<b> D;
    exrandom::unit_normal_dist<exrandom::rand_digit<b>> N(D);
    exrandom::u_rand<exrandom::rand_digit<b>> x(N.digit_generator());
    for (int i = 0; i < 10; ++i) {
      N.generate(g, x);
      std::cout << x << " = ";
      std::cout << x.print_fixed(g,p) << " = ";
      std::cout << x << "\n";
    }
    std::cout << "Total number of base " << b << " digits used = "
              << D.count() << "\n\n";
  }
}
