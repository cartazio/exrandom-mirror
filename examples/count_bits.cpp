#include <iostream>
#include <random>
#include <map>
#include <limits>
#include <exrandom/rand_digit.hpp>
#include <exrandom/u_rand.hpp>
#include <exrandom/unit_normal_dist.hpp>
#include <exrandom/unit_normal_kahn.hpp>
#include <exrandom/unit_exponential_dist.hpp>
#include <exrandom/unit_uniform_dist.hpp>
#include <exrandom/aux_info.hpp>

void print_hist(const std::map<int, long long>& hist) {
  const int maxlength = 70;     // Widest bar in histogram
  long long m = 0;
  for (auto p = hist.begin(); p != hist.end(); ++p)
    m = (std::max)(m, p->second);
  m = (std::max)(m / maxlength, 1LL);
  for (auto p = hist.begin(); p != hist.end(); ++p) {
    unsigned k = unsigned((p->second + m/2) / m);
    if (k) {
      std::cout << p->first << '\t';
      for (unsigned i = 0; i < k; ++i)
        std::cout << '*';
      std::cout << '\n';
    }
  }
  std::cout << std::endl;
}

int main() {
  unsigned s = std::random_device()(); // Set seed from random_device
  std::mt19937 g(s);                   // Initialize URNG
  std::cout << "Seed set to " << s << "\n" << std::endl;

  const unsigned b = 2;
  long long num = 1000000LL;
  std::cout << std::fixed << std::setprecision(5);
  {
    std::cout << "Sampling from unit normal distribution\n";
    exrandom::rand_digit<b> D;
    exrandom::unit_normal_dist<exrandom::rand_digit<b>> N(D);
    exrandom::u_rand<exrandom::rand_digit<b>>
      x(N.digit_generator());
    std::map<int, long long> bitcount;
    std::map<int, long long> fraccount;
    long long counta = 0, countb = 0, fractotal = 0;
    for (long long i = 0; i < num; ++i) {
      long long c0 = D.count();
      N.generate(g, x);
      counta += D.count() - c0;
      fractotal += x.ndigits();
      ++fraccount[int(x.ndigits())];
      x.value<double>(g);
      countb += D.count() - c0;
      ++bitcount[int(D.count() - c0)];
    }
    double H = exrandom::aux_info::normal_entropy() / std::log(2.0),
      Q = exrandom::aux_info::normal_mean_exponent(),
      B = counta/double(num),
      L = fractotal/double(num),
      C = B - L, T = C - H,
      F = countb/double(num);
    int d = std::numeric_limits<double>::digits + 1;
    std::cout << "<B> = " << B
              << ", <L> = " << L
              << ", C = <B> - <L> = " << C
              << ",\nH = " << H
              << ", T = C - H = " << T
              << ",\nQ = " << Q
              << ", F = C - Q + " << d << " = " << C - Q + d
              << " (predicted) = " << F << " (measured)\n" << std::endl;
    std::cout << "Histogram of number of bits in fraction\n";
    print_hist(fraccount);
    std::cout << "Histogram of number of bits needed to generate doubles\n";
    print_hist(bitcount);
  }

  {
    std::cout << "Sampling from unit normal distribution (Kahn method)\n";
    exrandom::rand_digit<b> D;
    exrandom::unit_normal_kahn<exrandom::rand_digit<b>> N(D);
    exrandom::u_rand<exrandom::rand_digit<b>>
      x(N.digit_generator());
    std::map<int, long long> bitcount;
    std::map<int, long long> fraccount;
    long long counta = 0, countb = 0, fractotal = 0;
    for (long long i = 0; i < num; ++i) {
      long long c0 = D.count();
      N.generate(g, x);
      counta += D.count() - c0;
      fractotal += x.ndigits();
      ++fraccount[int(x.ndigits())];
      x.value<double>(g);
      countb += D.count() - c0;
      ++bitcount[int(D.count() - c0)];
    }
    double H = exrandom::aux_info::normal_entropy() / std::log(2.0),
      Q = exrandom::aux_info::normal_mean_exponent(),
      B = counta/double(num),
      L = fractotal/double(num),
      C = B - L, T = C - H,
      F = countb/double(num);
    int d = std::numeric_limits<double>::digits + 1;
    std::cout << "<B> = " << B
              << ", <L> = " << L
              << ", C = <B> - <L> = " << C
              << ",\nH = " << H
              << ", T = C - H = " << T
              << ",\nQ = " << Q
              << ", F = C - Q + " << d << " = " << C - Q + d
              << " (predicted) = " << F << " (measured)\n" << std::endl;
    std::cout << "Histogram of number of bits in fraction\n";
    print_hist(fraccount);
    std::cout << "Histogram of number of bits needed to generate doubles\n";
    print_hist(bitcount);
  }

  {
    std::cout
      << "Sampling from unit exponential distribution (Algorithm V)\n";
    exrandom::rand_digit<b> D;
    exrandom::unit_exponential_dist<exrandom::rand_digit<b>,false> N(D);
    exrandom::u_rand<exrandom::rand_digit<b>>
      x(N.digit_generator());
    std::map<int, long long> bitcount;
    std::map<int, long long> fraccount;
    long long counta = 0, countb = 0, fractotal = 0;
    for (long long i = 0; i < num; ++i) {
      long long c0 = D.count();
      N.generate(g, x);
      counta += D.count() - c0;
      fractotal += x.ndigits();
      ++fraccount[int(x.ndigits())];
      x.value<double>(g);
      countb += D.count() - c0;
      ++bitcount[int(D.count() - c0)];
    }
    double H = exrandom::aux_info::exponential_entropy() / std::log(2.0),
      Q = exrandom::aux_info::exponential_mean_exponent(),
      B = counta/double(num),
      L = fractotal/double(num),
      C = B - L, T = C - H,
      F = countb/double(num);
    int d = std::numeric_limits<double>::digits + 1;
    std::cout << "<B> = " << B
              << ", <L> = " << L
              << ", C = <B> - <L> = " << C
              << ",\nH = " << H
              << ", T = C - H = " << T
              << ",\nQ = " << Q
              << ", F = C - Q + " << d << " = " << C - Q + d
              << " (predicted) = " << F << " (measured)\n" << std::endl;
  }

  {
    std::cout
      << "Sampling from unit exponential distribution (Algorithm E)\n";
    exrandom::rand_digit<b> D;
    exrandom::unit_exponential_dist<exrandom::rand_digit<b>> N(D);
    exrandom::u_rand<exrandom::rand_digit<b>>
      x(N.digit_generator());
    std::map<int, long long> bitcount;
    std::map<int, long long> fraccount;
    long long counta = 0, countb = 0, fractotal = 0;
    for (long long i = 0; i < num; ++i) {
      long long c0 = D.count();
      N.generate(g, x);
      counta += D.count() - c0;
      fractotal += x.ndigits();
      ++fraccount[int(x.ndigits())];
      x.value<double>(g);
      countb += D.count() - c0;
      ++bitcount[int(D.count() - c0)];
    }
    double H = exrandom::aux_info::exponential_entropy() / std::log(2.0),
      Q = exrandom::aux_info::exponential_mean_exponent(),
      B = counta/double(num),
      L = fractotal/double(num),
      C = B - L, T = C - H,
      F = countb/double(num);
    int d = std::numeric_limits<double>::digits + 1;
    std::cout << "<B> = " << B
              << ", <L> = " << L
              << ", C = <B> - <L> = " << C
              << ",\nH = " << H
              << ", T = C - H = " << T
              << ",\nQ = " << Q
              << ", F = C - Q + " << d << " = " << C - Q + d
              << " (predicted) = " << F << " (measured)\n" << std::endl;
    std::cout << "Histogram of number of bits in fraction\n";
    print_hist(fraccount);
    std::cout << "Histogram of number of bits needed to generate doubles\n";
    print_hist(bitcount);
  }

  {
    std::cout
      << "Sampling from unit uniform distribution\n";
    exrandom::rand_digit<b> D;
    exrandom::unit_uniform_dist<exrandom::rand_digit<b>> N(D);
    exrandom::u_rand<exrandom::rand_digit<b>> x(N.digit_generator());
    std::map<int, long long> bitcount;
    std::map<int, long long> fraccount;
    long long counta = 0, countb = 0, fractotal = 0;
    for (long long i = 0; i < num; ++i) {
      long long c0 = D.count();
      N.generate(g, x);
      counta += D.count() - c0;
      fractotal += x.ndigits();
      ++fraccount[int(x.ndigits())];
      x.value<double>(g);
      countb += D.count() - c0;
      ++bitcount[int(D.count() - c0)];
    }
    double H = exrandom::aux_info::uniform_entropy() / std::log(2.0),
      Q = exrandom::aux_info::uniform_mean_exponent(),
      B = counta/double(num),
      L = fractotal/double(num),
      C = B - L, T = C - H,
      F = countb/double(num);
    int d = std::numeric_limits<double>::digits + 1;
    std::cout << "<B> = " << B
              << ", <L> = " << L
              << ", C = <B> - <L> = " << C
              << ",\nH = " << H
              << ", T = C - H = " << T
              << ",\nQ = " << Q
              << ", F = C - Q + " << d << " = " << C - Q + d
              << " (predicted) = " << F << " (measured)\n" << std::endl;
    std::cout << "Histogram of number of bits in fraction\n";
    print_hist(fraccount);
    std::cout << "Histogram of number of bits needed to generate doubles\n";
    print_hist(bitcount);
  }

}
