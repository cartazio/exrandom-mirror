#include <iostream>
#include <iomanip>
#include <random>
#include <chrono>

#if !defined(EXRANDOM_HAVE_MPFR)
#define EXRANDOM_HAVE_MPFR 0
#endif
#if !defined(EXRANDOM_HAVE_MPREAL)
#define EXRANDOM_HAVE_MPREAL 0
#endif

#if EXRANDOM_HAVE_MPFR
#include <mpfr.h>

#if MPFR_VERSION >= MPFR_VERSION_NUM(3,1,0)
// mpfr_grandom requies MPFR 3.1
#define EXRANDOM_HAVE_GRANDOM 1
#else
#define EXRANDOM_HAVE_GRANDOM 0
#endif

#if MPFR_VERSION >= MPFR_VERSION_NUM(3,2,0)
// mpfr_nrandom requies MPFR 3.2
#define EXRANDOM_HAVE_NRANDOM 1
#else
#define EXRANDOM_HAVE_NRANDOM 0
#endif

#else
#define EXRANDOM_HAVE_GRANDOM 0
#define EXRANDOM_HAVE_NRANDOM 0
#endif

#if EXRANDOM_HAVE_MPREAL
#include <mpreal.h>
#endif

#include <exrandom/unit_normal_distribution.hpp>

#if EXRANDOM_HAVE_MPFR

int main() {
  gmp_randstate_t r;
  mpfr_t z, z2;
  mpfr_rnd_t rnd = MPFR_RNDN;
  double tx = 1e6; // us per test
  int preclist[] = {4, 8, 16, 24, 32, 48, 53, 64, 128, 256,
                    (1<<10), (1<<12), (1<<14), (1<<16), (1<<18), (1<<20)};
  gmp_randinit_mt(r);
  gmp_randseed_ui(r, std::random_device()());
  mpfr_init(z); mpfr_init(z2);

#if EXRANDOM_HAVE_NRANDOM
  double timelistn[] = {
    0.613,
    0.614,
    0.613,
    0.611,
    0.671,
    0.68,
    0.666,
    0.672,
    0.675,
    0.703,
    0.897,
    1.58,
    4.42,
    15.5,
    59.8,
    238
  };
#endif
#if EXRANDOM_HAVE_GRANDOM
  double timelistg[] = {
    2.13,
    1.95,
    2,
    2.23,
    2.4,
    2.45,
    2.58,
    2.79,
    3.92,
    6.18,
    20,
    126,
    1.3e+03,
    1.3e+04,
    1.15e+05,
    1.14e+06,
  };
#endif
#if EXRANDOM_HAVE_MPREAL
  auto g = std::mt19937(std::random_device()());
  double timelistm[] = {
    1.07,
    1.09,
    1.1,
    1.09,
    1.2,
    1.42,
    1.43,
    1.48,
    2.02,
    3.19,
    12.3,
    61.9,
    414,
    5.34e+03,
    1.01e+05,
    1.82e+06
  };
#endif
  std::cout << "timings (us) for nrandom vs grandom vs mpreal\n"
            << "prec nrandom grandom mpreal\n"
            << std::setprecision(3);
  for (unsigned k = 0; k < sizeof(preclist)/sizeof(int); ++k) {
    int prec = preclist[k];
    double tn = -1, tg = -1, tm = -1;
#if EXRANDOM_HAVE_NRANDOM
    {
      mpfr_set_prec(z, prec);
      size_t m = (size_t)(tx / timelistn[k] + 1);
      auto t0 = std::chrono::high_resolution_clock::now();
      for (size_t i = 0; i < m; ++i)
        mpfr_nrandom(z, r, rnd);
      auto t1 = std::chrono::high_resolution_clock::now();
      double dt = double(std::chrono::duration_cast<std::chrono::nanoseconds>
                         (t1 - t0).count());
      tn = dt/(m * 1000);
    }
#endif
#if EXRANDOM_HAVE_GRANDOM
    {
      mpfr_set_prec(z, prec); mpfr_set_prec(z2, prec);
      size_t m = (size_t)(tx / timelistg[k] + 1);
      auto t0 = std::chrono::high_resolution_clock::now();
      for (size_t i = 0; i < (m + 1)/2; ++i)
        mpfr_grandom(z, z2, r, rnd);
      auto t1 = std::chrono::high_resolution_clock::now();
      double dt = double(std::chrono::duration_cast<std::chrono::nanoseconds>
                        (t1 - t0).count());
      tg = dt/((m + 1)/2 * 2000);
    }
#endif
#if EXRANDOM_HAVE_MPREAL
    {
      mpfr::mpreal::set_default_prec(prec);
      exrandom::unit_normal_distribution<mpfr::mpreal> N;
      size_t m = (size_t)(tx / timelistm[k] + 1);
      auto t0 = std::chrono::high_resolution_clock::now();
      for (size_t i = 0; i < m; ++i) N(g);
      auto t1 = std::chrono::high_resolution_clock::now();
      double dt = double(std::chrono::duration_cast<std::chrono::nanoseconds>
                         (t1 - t0).count());
      tm = dt/(m * 1000);
    }
#endif
    std::cout << prec << " " << tn << " " << tg << " " << tm << "\n";
  }
  mpfr_clear(z); mpfr_clear(z2);
  mpfr_free_cache();
  gmp_randclear(r);
}

#else
int main() {
  std::cerr << "MPFR version 3.1 or later required\n\n";
}
#endif
