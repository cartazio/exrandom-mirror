#include <iostream>
#include <iomanip>
#include <random>
#include <chrono>
#include <exrandom/unit_normal_distribution.hpp>
#include <exrandom/unit_exponential_distribution.hpp>
#include <exrandom/discrete_normal_distribution.hpp>

template<typename Dist, typename Generator>
double timer(long long num, Dist& d, Generator& g) {
  typename Dist::result_type sum = 0;
  auto t0 = std::chrono::high_resolution_clock::now();
  for (long long i = 0; i < num; ++i)
    sum += d(g);
  auto t1 = std::chrono::high_resolution_clock::now();
  double dt = double(std::chrono::duration_cast<std::chrono::nanoseconds>
                       (t1 - t0).count());
  return dt/num;
}

template<typename Generator>
void discrete_timer(Generator& g, long long num,
                    int mu_num, int mu_den, int sigma_num, int sigma_den) {
  exrandom::discrete_normal_distribution d(mu_num, mu_den,
                                           sigma_num, sigma_den);
  double t = timer(num, d, g);
  std::cout << "  time with mu = " << d.mu_num() << "/" << d.mu_den()
            << ", sigma = " << d.sigma_num() << "/" << d.sigma_den()
            << ": " << t << " ns" << std::endl;
}

int main() {
  unsigned s = std::random_device()(); // Set seed from random_device
  std::mt19937 g(s);                   // Initialize URNG
  std::cout << "Seed set to " << s << "\n\n";
  long long num = 5000000LL;
  std::cout << std::fixed << std::setprecision(1);

  std::cout << "Compare times to sample from the normal distribution\n";
  {
    typedef float real;
    std::normal_distribution<real> d1;
    double t1 = timer(num, d1, g);
    exrandom::unit_normal_distribution<real> d2;
    double t2 = timer(num, d2, g);
    std::cout << "  time with prec "
              << std::numeric_limits<real>::digits
              << ": C++11/random = " << t1
              << " ns; exrandom = " << t2 << " ns" << std::endl;
  }
  {
    typedef double real;
    std::normal_distribution<real> d1;
    double t1 = timer(num, d1, g);
    exrandom::unit_normal_distribution<real> d2;
    double t2 = timer(num, d2, g);
    std::cout << "  time with prec "
              << std::numeric_limits<real>::digits
              << ": C++11/random = " << t1
              << " ns; exrandom = " << t2 << " ns" << std::endl;
  }
  {
    // For Visual Studio long double is the same as double
    typedef long double real;
    std::normal_distribution<real> d1;
    double t1 = timer(num, d1, g);
    exrandom::unit_normal_distribution<real> d2;
    double t2 = timer(num, d2, g);
    std::cout << "  time with prec "
              << std::numeric_limits<real>::digits
              << ": C++11/random = " << t1
              << " ns; exrandom = " << t2 << " ns" << std::endl;
  }

  std::cout << "Compare times to sample from the exponential distribution\n";
  {
    typedef float real;
    std::exponential_distribution<real> d1;
    double t1 = timer(num, d1, g);
    exrandom::unit_exponential_distribution<real> d2;
    double t2 = timer(num, d2, g);
    std::cout << "  time with prec "
              << std::numeric_limits<real>::digits
              << ": C++11/random = " << t1
              << " ns; exrandom = " << t2 << " ns" << std::endl;
  }
  {
    typedef double real;
    std::exponential_distribution<real> d1;
    double t1 = timer(num, d1, g);
    exrandom::unit_exponential_distribution<real> d2;
    double t2 = timer(num, d2, g);
    std::cout << "  time with prec "
              << std::numeric_limits<real>::digits
              << ": C++11/random = " << t1
              << " ns; exrandom = " << t2 << " ns" << std::endl;
  }
  {
    // For Visual Studio long double is the same as double
    typedef long double real;
    std::exponential_distribution<real> d1;
    double t1 = timer(num, d1, g);
    exrandom::unit_exponential_distribution<real> d2;
    double t2 = timer(num, d2, g);
    std::cout << "  time with prec "
              << std::numeric_limits<real>::digits
              << ": C++11/random = " << t1
              << " ns; exrandom = " << t2 << " ns" << std::endl;
  }

  std::cout << "Times to sample from the discrete normal distribution\n";
  discrete_timer(g, num,  1, 7, 16, 100);
  discrete_timer(g, num,  2, 7, 16, 10);
  discrete_timer(g, num,  3, 7, 16, 1);
  discrete_timer(g, num,  0, 7, 160, 1);
  discrete_timer(g, num, -1, 7, 1600, 1);
  discrete_timer(g, num, -2, 7, 16000, 1);
  discrete_timer(g, num, -3, 7, 160000, 1);
  discrete_timer(g, num,  1, 7, 1600000, 1);
  discrete_timer(g, num,  0, 7, (1<<17)-1, 1);
  discrete_timer(g, num,  0, 7, (1<<17),   1);
  discrete_timer(g, num,  0, 1, (1<<17)+1, 1);
  discrete_timer(g, num,  0, 1, (1<<18)-1, 1);
  discrete_timer(g, num,  0, 1, (1<<18),   1);
  discrete_timer(g, num,  0, 1, (1<<18)+1, 1);
}
