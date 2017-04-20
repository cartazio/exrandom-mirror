#include <iostream>
#include <random>

#if !defined(EXRANDOM_HAVE_MPREAL)
#define EXRANDOM_HAVE_MPREAL 0
#endif
#if !defined(EXRANDOM_HAVE_BOOST_DEC)
#define EXRANDOM_HAVE_BOOST_DEC 0
#endif
#if !defined(EXRANDOM_HAVE_BOOST_GMP)
#define EXRANDOM_HAVE_BOOST_GMP 0
#endif
#if !defined(EXRANDOM_HAVE_BOOST_MPFR)
#define EXRANDOM_HAVE_BOOST_MPFR 0
#endif
#if !defined(EXRANDOM_HAVE_BOOST_FLOAT128)
#define EXRANDOM_HAVE_BOOST_FLOAT128 0
#endif

#if EXRANDOM_HAVE_MPREAL
#include <mpreal.h>
#endif
#if EXRANDOM_HAVE_BOOST_MPFR
#include <boost/multiprecision/mpfr.hpp>
#endif
#if EXRANDOM_HAVE_BOOST_GMP
#include <boost/multiprecision/gmp.hpp>
#endif
#if EXRANDOM_HAVE_BOOST_DEC
#include <boost/multiprecision/cpp_dec_float.hpp>
#endif
#if EXRANDOM_HAVE_BOOST_FLOAT128
#include <boost/multiprecision/float128.hpp>
#endif

// N.B. This #include should come *after* the headers which define the
// floating point types.
#include <exrandom/unit_normal_distribution.hpp>

int main() {

#if EXRANDOM_HAVE_MPREAL || EXRANDOM_HAVE_BOOST_MPFR || \
  EXRANDOM_HAVE_BOOST_GMP || EXRANDOM_HAVE_BOOST_DEC || \
  EXRANDOM_HAVE_BOOST_FLOAT128
  unsigned s = std::random_device()(); // Set seed from random_device
  std::mt19937 g(s);                   // Initialize URNG
  std::cout << "Seed set to " << s << "\n\n";
#endif

#if EXRANDOM_HAVE_MPREAL
  {
    typedef mpfr::mpreal real;
    real::set_default_prec(240);
    exrandom::unit_normal_distribution<real> N;
    std::cout
      << "Sampling mpreal from the unit normal distribution:\n"
      << std::setprecision(mpfr::bits2digits(real::get_default_prec()));
    for (int i = 0; i < 20; ++i)
      std::cout << N(g) << "\n";
    std::cout << "\n";
  }
#else
  std::cerr << "This example requires mpreal.h\n\n";
#endif

#if EXRANDOM_HAVE_BOOST_MPFR
  {
    typedef boost::multiprecision::mpfr_float_50 real;
    exrandom::unit_normal_distribution<real> N;
    std::cout
      << "Sampling mpfr_float_50 from the unit normal distribution:\n"
      << std::setprecision(50);
    for (int i = 0; i < 20; ++i)
      std::cout << N(g) << "\n";
    std::cout << "\n";
  }
#else
  std::cerr << "This example requires boost/multiprecision/mpfr.hpp\n\n";
#endif

#if EXRANDOM_HAVE_BOOST_GMP
  {
    typedef boost::multiprecision::mpf_float_50 real;
    exrandom::unit_normal_distribution<real> N;
    std::cout
      << "Sampling mpf_float_50 from the unit normal distribution:\n"
      << std::setprecision(50);
    for (int i = 0; i < 20; ++i)
      std::cout << N(g) << "\n";
    std::cout << "\n";
  }
#else
  std::cerr << "This example requires boost/multiprecision/gmp.hpp\n\n";
#endif

#if EXRANDOM_HAVE_BOOST_DEC
  {
    typedef
      boost::multiprecision::number<boost::multiprecision::cpp_dec_float<20U>>
      real;
    exrandom::unit_normal_distribution<real> N;
    std::cout <<
      "Sampling number<cpp_dec_float<20U>> from the unit normal distribution:\n"
      << std::setprecision(20);
    for (int i = 0; i < 20; ++i)
      std::cout << N(g) << "\n";
    std::cout << "\n";
  }
#else
  std::cerr
    << "This example requires boost/multiprecision/cpp_dec_float.hpp\n\n";
#endif

#if EXRANDOM_HAVE_BOOST_FLOAT128
  {
    typedef boost::multiprecision::float128 real;
    exrandom::unit_normal_distribution<real> N;
    std::cout
      << "Sampling float128 exactly from the unit normal distribution:\n"
      << std::setprecision(33);
    for (int i = 0; i < 20; ++i)
      std::cout << N(g) << "\n";
    std::cout << "\n";
  }
#else
  std::cerr << "This example requires boost/multiprecision/float128.hpp\n\n";
#endif
}
