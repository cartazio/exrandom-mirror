#include <iostream>
#include <random>
#include <exrandom/rand_digit.hpp>
#include <exrandom/i_rand.hpp>
#include <exrandom/discrete_normal_dist.hpp>
#include <exrandom/aux_info.hpp>

template<typename Generator>
void count_bits(Generator& g, long long num,
                int mu_num, int mu_den, int sigma_num, int sigma_den) {

  const unsigned b = 2;
  exrandom::rand_digit<b> D;
  exrandom::discrete_normal_dist<exrandom::rand_digit<b>>
    N(D, mu_num, mu_den, sigma_num, sigma_den);
  exrandom::i_rand<exrandom::rand_digit<b>> h(N.digit_generator());

  long long counta = 0, countb = 0, fractotal = 0;
  for (long long i = 0; i < num; ++i) {
    long long c0 = D.count();
    N.generate(g, h);
    counta += D.count() - c0;
    fractotal += h.entropy();
    h(g);
    countb += D.count() - c0;
  }
  double H;
  exrandom::aux_info::discrete_normal_norm(N.param(), H);
  H /= std::log(2.0);
  double B = counta/double(num),
    L = fractotal/double(num),
    F = countb/double(num),
    T = F - H;
  std::cout << "mu = " << N.mu_num() << "/" << N.mu_den()
            << ", sigma = " << N.sigma_num() << "/" << N.sigma_den()
            << ":\n";
  std::cout << " <B> = " << B
            << ", <L> = " << L
            << ", <F> = <B> + <L> = " << F
            << ",\n H = " << H
            << ", T = <F> - H = " << T << "" << std::endl;
}

int main() {
  unsigned s = std::random_device()(); // Set seed from random_device
  std::mt19937 g(s);                   // Initialize URNG
  std::cout << "Seed set to " << s << "\n\n";

  long long num = 2000000LL;
  std::cout
    << "Sampling from the discrete normal distribution (Algorithm D)\n";
  count_bits(g, num,  1, 7, 16, 100);
  count_bits(g, num,  2, 7, 16, 10);
  count_bits(g, num,  3, 7, 16, 1);
  count_bits(g, num,  0, 7, 160, 1);
  count_bits(g, num, -1, 7, 1600, 1);
  count_bits(g, num, -2, 7, 16000, 1);
  count_bits(g, num, -3, 7, 160000, 1);
  count_bits(g, num,  1, 7, 1600000, 1);
  count_bits(g, num,  0, 7, (1<<17)-1, 1);
  count_bits(g, num,  0, 7, (1<<17),   1);
  count_bits(g, num,  0, 1, (1<<17)+1, 1);
  count_bits(g, num,  0, 1, (1<<18)-1, 1);
  count_bits(g, num,  0, 1, (1<<18),   1);
  count_bits(g, num,  0, 1, (1<<18)+1, 1);

}
