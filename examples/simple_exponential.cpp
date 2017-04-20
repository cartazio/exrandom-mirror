#include <iostream>
#include <random>
#include <exrandom/unit_exponential_distribution.hpp>

int main() {
  unsigned s = std::random_device()(); // Set seed from random_device
  std::mt19937 g(s);                   // Initialize URNG
  std::cout << "Seed set to " << s << "\n\n";

  exrandom::unit_exponential_distribution<float> N;
  std::cout
    << "Sampling floats exactly from the unit exponential distribution:\n";
  for (int i = 0; i < 48; ++i)
    std::cout << N(g) << ((i + 1) % 8 ? ' ' : '\n');
  std::cout << "\n";
}
