#include <iostream>
#include <random>
#include <exrandom/unit_uniform_distribution.hpp>

int main() {
  unsigned s = std::random_device()(); // Set seed from random_device
  std::mt19937 g(s);                   // Initialize URNG
  std::cout << "Seed set to " << s << "\n\n";

  exrandom::unit_uniform_distribution<long double> N;
  std::cout
    << "Sampling long doubles exactly from the unit uniform distribution:\n";
  for (int i = 0; i < 48; ++i)
    std::cout << N(g) << ((i + 1) % 8 ? ' ' : '\n');
  std::cout << "\n";
}
