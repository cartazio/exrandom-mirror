#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <map>
#include <exrandom/unit_normal_distribution.hpp>
#include <exrandom/unit_exponential_distribution.hpp>
#include <exrandom/unit_uniform_distribution.hpp>
#include <exrandom/discrete_normal_distribution.hpp>
#include <exrandom/aux_info.hpp>

// Compute probs for num bins [x0+n*dx, x0+(n+1)*dx] for n=[0,num) plus an
// (num+1)th bin for everything else
std::vector<double> probs(double (*f)(double),
                          double x0, double dx, int nbins) {
  std::vector<double> r(nbins+1, 0);
  double s = 0;
  for (int n = 0; n < nbins; ++n) {
    r[n] = f(x0 + (n + 1) * dx) - f(x0 + n * dx);
    s += r[n];
  }
  r[nbins] = 1 - s;
  return r;
}

// Compute probs for discrete normal using num bins [x0+n*dx, x0+(n+1)*dx) for
// i=[0,num) plus an (num+1)th bin for everything else
template<typename param_type>
std::vector<double> discrete_normal_probs(const param_type& p,
                                          int x0, int dx, int nbins) {
  double H;
  double norm = exrandom::aux_info::discrete_normal_norm(p, H);
  std::vector<double> r(nbins+1, 0);
  double s = 0;
  for (int n = 0; n < nbins; ++n) {
    for (int j = 0; j < dx; ++j)
      r[n] +=
        exrandom::aux_info::discrete_normal_prob(p, x0 + dx * n + j, norm);
    s += r[n];
  }
  r[nbins] = 1 - s;
  return r;
}

double chisqf(const std::vector<double>& probs,
              std::map<int, long long>& counts) {
  int nbins = int(probs.size()) - 1;
  if (nbins < 1) return 0;
  long long num = 0;
  for (auto p = counts.begin(); p != counts.end(); ++p)
    num += p->second;
  double v = 0, x;
  long long s = 0;
  for (int n = 0; n < nbins; ++n) {
    s += counts[n];
    x = counts[n] - num * probs[n];
    v += x * x / (num * probs[n]);
  }
  x = (num - s) - num * probs[nbins];
  v += x * x / (num * probs[nbins]);
  return v;
}

template<typename Generator>
void discrete_chisq(Generator& g, long long num,
                    int mu_num, int mu_den, int sigma_num, int sigma_den,
                    int x0, int dx, int DOF) {
    exrandom::discrete_normal_distribution
      d(mu_num, mu_den, sigma_num, sigma_den);
    double mu = d.mu_num() / double(d.mu_den()),
      sigma = d.sigma_num() / double(d.sigma_den());
    std::map<int, long long> hist;
    for (long long i = 0; i < num; ++i)
      ++hist[int(std::floor((d(g) - x0 + 0.5)/dx))];
    double chisq = chisqf(discrete_normal_probs(d.param(), x0, dx, DOF),
                          hist);
    std::cout << "discrete_normal_distribution (mu = " << mu
              << ", sigma = " << sigma << "):\n            samples = "
              << num << ", DOF = " << DOF
              << ", chi-squared = " << chisq << std::endl;
  }

int main() {
  unsigned s = std::random_device()(); // Set seed from random_device
  std::mt19937 g(s);                   // Initialize URNG
  std::cout << "With 50 degrees of freedom, chisq should lie\n"
            << "   between 29.71 and 76.15, 98% of the time\n"
            << "   between 34.76 and 67.50, 90% of the time\n"
            << "   between 42.94 and 56.33, 50% of the time\n"
            << "See Knuth TAOCP, Vol 2, Sec. 3.3.1\n"
            << "Seed set to " << s << "\n\n";

  std::cout << std::fixed << std::setprecision(2);
  long long num =  5000000LL;
  {
#if !HAVE_CPLUSPLUS11_MATH
    std::cout
      << "NOTE: NaNs will be reported for the unit normal distribution\n\
      if your C++ library does not provide the C++11 function std::erf\n";
#endif
    exrandom::unit_normal_distribution<double> d;
    int DOF = 50;
    double x0 = -4, dx = 0.16;  // 50 bins in [-4,4]
    std::map<int, long long> hist;
    for (long long i = 0; i < num; ++i)
      ++hist[int(std::floor( (d(g) - x0) / dx ))];
    double chisq = chisqf(probs(exrandom::aux_info::cumulative_normal,
                                x0, dx, DOF),
                          hist);
    std::cout << "unit_normal_distribution: samples = "
              << num << ", DOF = " << DOF
              << ", chi-squared = " << chisq << std::endl;
  }

  {
    exrandom::unit_exponential_distribution<double> d;
    int DOF = 50;
    double x0 = 0, dx = 0.16;   // 50 bins in [0,8]
    std::map<int, long long> hist;
    for (long long i = 0; i < num; ++i)
      ++hist[int(std::floor( (d(g) - x0) / dx ))];
    double chisq = chisqf(probs(exrandom::aux_info::cumulative_exponential,
                                x0, dx, DOF),
                          hist);
    std::cout << "unit_exponential_distribution: samples = "
              << num << ", DOF = " << DOF
              << ", chi-squared = " << chisq << std::endl;
  }

  {
    exrandom::unit_uniform_distribution<double> d;
    int DOF = 50;
    double x0 = 0, dx = 1/51.0; // 50 bins in [0,50/51]
    std::map<int, long long> hist;
    for (long long i = 0; i < num; ++i)
      ++hist[int(std::floor( (d(g) - x0) / dx ))];
    double chisq = chisqf(probs(exrandom::aux_info::cumulative_uniform,
                                x0, dx, DOF),
                          hist);
    std::cout << "unit_uniform_distribution: samples = "
              << num << ", DOF = " << DOF
              << ", chi-squared = " << chisq << std::endl;
  }

  discrete_chisq(g, num, 0, 1, 6, 1, -24, 1, 50);
  discrete_chisq(g, num, 1, 3, 6, 1, -24, 1, 50);
  discrete_chisq(g, num, 0, 1, 61, 10, -24, 1, 50);
  discrete_chisq(g, num, -5, 3, 69, 10, -24, 1, 50);
  discrete_chisq(g, num, 201, 7, 1301, 2, -2500, 100, 50);
}
