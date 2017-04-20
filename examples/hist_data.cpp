#include <iostream>
#include <iomanip>
#include <random>
#include <map>
#include <exrandom/rand_digit.hpp>
#include <exrandom/u_rand.hpp>
#include <exrandom/unit_normal_dist.hpp>

int main() {
  unsigned s = std::random_device()(); // Set seed from random_device
  std::mt19937 g(s);                   // Initialize URNG
  std::cerr << "Seed set to " << s << "\n"
            << "columns are l n prob(n/2^l)\n\n";

  const unsigned b = 2;
  exrandom::rand_digit<b> D;
  exrandom::unit_normal_dist<exrandom::rand_digit<b>> N(D);
  exrandom::u_rand<exrandom::rand_digit<b>> x(N.digit_generator());
  long long num = 5000000LL;
  unsigned maxl = 8, maxn = 3;
  std::map<std::pair<long long, long long>, long long> hist;
  for (long long i = 0; i < num; ++i) {
    N.generate(g, x);
    if (!(x.ndigits() <= maxl && x.integer() < maxn))
      continue;
    if (x.sign() < 1) x.negate();
    ++hist[x.rational()];
  }
  std::cout << std::setprecision(10);
  for (long l = 0, d = 1; l <= long(maxl); ++l, d *= 2)
    for (long n = 0; n < long(maxn)*d; ++n)
      std::cout << l << " " << n << " "
                << hist[std::pair<long long,long long>(n,d)]/double(num)
                << "\n";
}
