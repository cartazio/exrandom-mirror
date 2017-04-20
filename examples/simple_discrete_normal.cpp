#include <iostream>
#include <random>
#include <exrandom/discrete_normal_distribution.hpp>

int main() {
  unsigned s = std::random_device()(); // Set seed from random_device
  std::mt19937 g(s);                   // Initialize URNG
  std::cout << "Seed set to " << s << "\n\n";

  exrandom::discrete_normal_distribution N(1,3,129,2);
  std::cout
    << "Sampling exactly from the discrete normal distribution,\n"
    << "  mu = " << N.mu_num() << "/" << N.mu_den()
    << ", sigma = " << N.sigma_num() << "/" << N.sigma_den() << ":\n";
  for (int i = 0; i < 100; ++i)
    std::cout << N(g) << ((i + 1) % 20 ? ' ' : '\n');
  std::cout << "\n";
}
