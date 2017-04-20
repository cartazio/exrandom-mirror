#include <iostream>
#include <random>
#include <map>
#include <exrandom/rand_digit.hpp>
#include <exrandom/discrete_normal_dist.hpp>

int main() {
  std::cout << "Sampling from the discrete normal distribution\n";
  unsigned s = std::random_device()(); // Set seed from random_device
  std::mt19937 g(s);                   // Initialize URNG
  std::cout << "Seed set to " << s << "\n\n";

  const unsigned b = 2;
  exrandom::rand_digit<b> D;
  exrandom::discrete_normal_dist<exrandom::rand_digit<b>>
    N(D, 87331, 3, 16000051, 100);
  exrandom::i_rand<exrandom::rand_digit<b>> h(N.digit_generator());
  std::cout << "Parameters: "
            << " mu = " << N.mu_num() << "/" << N.mu_den()
            << " sigma = " << N.sigma_num() << "/" << N.sigma_den() << "\n"
            << "columns are: uniform range, final int\n";
  int num = 20;
  for (int i = 0; i < num; ++i) {
    N.generate(g, h);
    std::cout << h << " = ";
    std::cout << h(g) << "\n";
  }
    std::cout << "Total number of base " << b << " digits used = "
              << D.count() << "\n\n";
}
