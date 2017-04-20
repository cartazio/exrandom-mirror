#include <iostream>
#include <random>
#include <map>
#include <cmath>
#include <algorithm>
#include <string>
#include <exrandom/unit_normal_distribution.hpp>
#include <exrandom/unit_exponential_distribution.hpp>
#include <exrandom/unit_uniform_distribution.hpp>
#include <exrandom/discrete_normal_distribution.hpp>

template<typename Dist, typename Generator>
void histogram(long long num, Dist& d, Generator& g,
               double step, double offset) {
  std::map<int,unsigned> hist;
  const unsigned maxlength = 40; // Widest bar in histogram
  for (long long i = 0; i < num; ++i)
    ++hist[int(std::floor((double(d(g)) - offset) / step + 0.5))];
  unsigned m = 0;
  for (auto p : hist) m = (std::max)(m, p.second);
  m = (std::max)(m / maxlength, 1U);
  for (auto p : hist) {
    unsigned k = (p.second + m/2) / m;
    if (k)
      std::cout << typename Dist::result_type(p.first * step + offset) << '\t'
                << std::string(k, '*') << '\n';
  }
  std::cout << std::endl;
}

int main() {
  unsigned s = std::random_device()(); // Set seed from random_device
  std::mt19937 g(s);                   // Initialize URNG
  std::cout << "Seed set to " << s << "\n\n";

  long long num = 1000000LL;

  {
    exrandom::unit_normal_distribution<double> d;
    std::cout << "Sampling the unit normal distribution\n";
    histogram(num, d, g, 0.2, 0.0);
  }
  {
    exrandom::unit_exponential_distribution<double> d;
    std::cout << "Sampling the unit exponential distribution\n";
    histogram(num, d, g, 0.2, 0.1);
  }
  {
    exrandom::unit_uniform_distribution<double> d;
    std::cout << "Sampling the unit uniform distribution\n";
    histogram(num, d, g, 0.1, 0.05);
  }
  {
    exrandom::discrete_normal_distribution d(6, 55, 10);
    std::cout << "Sampling the discrete normal distribution"
              << " mu = " << d.mu_num() << "/" << d.mu_den()
              << " sigma = " << d.sigma_num() << "/" << d.sigma_den() << "\n";
    histogram(num, d, g, 1, 0);
  }
}
