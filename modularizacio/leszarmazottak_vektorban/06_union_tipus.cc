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
  virtual ~Alakzat() {};
};

void Alakzat::eltol(double dx, double dy) {
  x += dx;
  y += dy;
}

double Alakzat::terulet() {
  std::cerr << "oooooo" << std::endl;
  return 0.0;
}

class Kor : public Alakzat {
 public:
  double r;
  double terulet();
};

double Kor::terulet() {
  std::cerr << "xxxx" << std::endl;
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

struct AlakzatTarolo {
  enum {KOR, TEGLALAP} melyik;
  
  union {
    Kor kor;
    Teglalap teglalap;
  };
  AlakzatTarolo() {};
  AlakzatTarolo(AlakzatTarolo&& at) : melyik{at.melyik} {
    if (at.melyik == KOR) {
      kor = at.kor;
      return;
    }
    teglalap = at.teglalap;
  }  
  AlakzatTarolo(AlakzatTarolo const& at) : melyik{at.melyik} {
    if (at.melyik == KOR) {
      kor = at.kor;
      return;
    }
    teglalap = at.teglalap;
  }
  ~AlakzatTarolo() {}
  Alakzat& alakzat() {
    if (melyik == KOR) {
      std::cerr << "kor" << std::endl;
      std::cerr << "fffff " << kor.terulet() << std::endl;
      Alakzat& a = kor;
      std::cerr << "uuuuu " << a.terulet() << std::endl;
      return a;
    }
    std::cerr << "teglalap" << std::endl;
    return dynamic_cast<Alakzat&>(teglalap);
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
      AlakzatTarolo at;
      at.melyik = AlakzatTarolo::KOR;
      at.kor.x = x;
      at.kor.y = y;
      at.kor.r = r;
      alakzatok.push_back(at);
    } else {
      double x, y, top, left;
      std::cin >> x >> y >> top >> left;
      if (!std::cin) {
        throw std::runtime_error("nem tudtam egy téglalapot beolvasni");
      }
      AlakzatTarolo at;
      at.melyik = AlakzatTarolo::TEGLALAP;
      at.teglalap.x = x;
      at.teglalap.y = y;
      at.teglalap.top = top;
      at.teglalap.left = left;
      alakzatok.push_back(at);
    }
  }

  double osszes_terulet = 0.0;
  for (std::size_t i = 0; i < alakzatok.size(); ++i) {
    Alakzat& a = alakzatok[i].alakzat();
    double terulet = a.terulet();
    std::cout << "alakzat #" << i << ": terulet=" << terulet << std::endl;
    osszes_terulet += terulet;
  }

  std::cout << "Az összes terület: " << osszes_terulet << std::endl;
}