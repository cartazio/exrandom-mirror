#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <exrandom/table_gen.hpp>
#include <exrandom/rand_table.hpp>
#include <exrandom/unit_normal_dist.hpp>

int main() {
  std::vector<std::string> seed_table;
  // digits.txt is the the RAND table of random digits available from
  // http://www.rand.org/content/dam/rand/pubs/monograph_reports/MR1418/MR1418.digits.txt.zip
  std::ifstream seed_file("digits.txt");
  if (seed_file.good()) {
    std::string s;
    while (std::getline(seed_file, s)) {
      std::istringstream is(s);
      is >> s;
      std::string seed;
      for (int i = 0; i < 10; ++i) {
        is >> s;
        seed += s;
      }
      seed_table.push_back(seed);
    }
  } else {
    // lines 9077 - 9086, in case the full table is not available
    seed_table.push_back("91486866851718646640939841418208696361599740927130");
    seed_table.push_back("27085545979733960919843574531542387910433856492503");
    seed_table.push_back("50144629743871836759770203434855225597834197811609");
    seed_table.push_back("06513031977786096289011670052044879745609312522650");
    seed_table.push_back("27360659086699793904300006309496840459265099570954");
    seed_table.push_back("02069431357777418334914891893312167990996169079700");
    seed_table.push_back("32331134586674046873033847880723099262580390063311");
    seed_table.push_back("94820103089730893927855765220910774260125296553336");
    seed_table.push_back("13206851466217116627804451376819961678521211905382");
    seed_table.push_back("73582504716450282034788914850174375938706490445325");
  }

  exrandom::rand_table D;
  exrandom::u_rand<exrandom::rand_table> x(D);
  exrandom::unit_normal_dist<exrandom::rand_table> U(D);
  exrandom::table_gen g;

  for (size_t k = 0; k < seed_table.size(); ++k) {
    try {
      g.seed(seed_table[k]);
      long long c0 = D.count();
      U.generate(g,x);
      long long c1 = D.count();
      std::string x0 = x.print();
      std::string x1 = x.print_fixed(g, 6);
      std::string x2 = x.print();
      long long c2 = D.count();
      std::cout << seed_table[k].substr(0,int(c1-c0)) << "|"
                << seed_table[k].substr(int(c1-c0), int(c2-c1)) << "... -> ";
      std::cout << x0 << " = " << x2 << " ~ " << x1 << "\n";
    }
    catch (const std::exception&) {
      std::cerr << "Ran out of digits with seed:\n" << seed_table[k] << "\n";
    }
  }
}
