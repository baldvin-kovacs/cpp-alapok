#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <utility>

class Alakzat {
 public:
  double x, y;
  void eltol(double dx, double dy);
  virtual double terulet();
};

void Alakzat::eltol(double dx, double dy) {
  x += dx;
  y += dy;
}

double Alakzat::terulet() {
  return 0.0;
}

class Kor : public Alakzat {
 public:
  double r;
  double terulet();
};

double Kor::terulet() {
  return 3.14 * r *r;
}

class Teglalap : public Alakzat {
 public:
  double top, left;
  double terulet();
};

double Teglalap::terulet() {
  double szelesseg = x - left;
  double magassag = y - top;
  return szelesseg * magassag;
}

class AlakzatTarolo {
 public:
  enum {KOR, TEGLALAP} melyik;
  union {
    Kor kor;
    Teglalap teglalap;
  };
  Alakzat* alakzat() {
    if (melyik == KOR) {
      return &kor;
    }
    return &teglalap;
  }
};

int main() {
  std::vector<AlakzatTarolo> alakzatok;
  while (true) {
    std::string micsoda;
    std::cin >> micsoda;
    if (!std::cin) {
      break;
    }
    if (micsoda == "kor") {
      double x, y, r;
      std::cin >> x >> y >> r;
      if (!std::cin) {
        throw std::runtime_error("nem tudtam egy kört beolvasni");
      }
      Kor k;
      k.x = x;
      k.y = y;
      k.r = r;
      alakzatok.emplace_back(AlakzatTarolo::KOR, k);
    } else {
      double x, y, top, left;
      std::cin >> x >> y >> top >> left;
      if (!std::cin) {
        throw std::runtime_error("nem tudtam egy téglalapot beolvasni");
      }
      Teglalap t;
      t.x = x;
      t.y = y;
      t.top = top;
      t.left = left;
      alakzatok.emplace_back(AlakzatTarolo::TEGLALAP, t);
    }
  }

  double osszes_terulet = 0.0;
  for (std::size_t i = 0; i < alakzatok.size(); ++i) {
    double terulet = alakzatok[i].alakzat()->terulet();
    std::cout << "alakzat #" << i << ": terulet=" << terulet << std::endl;
    osszes_terulet += terulet;
  }

  std::cout << "Az összes terület: " << osszes_terulet << std::endl;
}