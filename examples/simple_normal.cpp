#include <iostream>
#include <random>
#include <exrandom/unit_normal_distribution.hpp>

int main() {
  unsigned s = std::random_device()(); // Set seed from random_device
  std::mt19937 g(s);                   // Initialize URNG
  std::cout << "Seed set to " << s << "\n\n";

  exrandom::unit_normal_distribution<double> N;
  std::cout
    << "Sampling doubles exactly from the unit normal distribution:\n";
  for (int i = 0; i < 48; ++i)
    std::cout << N(g) << ((i + 1) % 8 ? ' ' : '\n');
  std::cout << "\n";
}
