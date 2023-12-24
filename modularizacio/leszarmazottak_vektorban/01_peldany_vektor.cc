#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

class Alakzat {
 public:
  double x, y;
  void eltol(double dx, double dy);

  // Egy általános alakzatnak nincsen értelmes terulet() függvénye.
  // Ebben a példában csak azért készítünk egyet, hogy a main() alján
  // meghívhassuk a for ciklusban, remélve, hogy a leszármazottak
  // terület függvénye hívódik meg. De persze nem, hanem ez itt, amit
  // jobb híján úgy írunk meg, hogy mindig nullát adjon vissza.
  double terulet();
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

int main() {
  std::vector<Alakzat> alakzatok;
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
      alakzatok.push_back(k);
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
      alakzatok.push_back(t);
    }
  }

  double osszes_terulet = 0.0;
  for (std::size_t i = 0; i < alakzatok.size(); ++i) {
    // Az alakzatok[i] úgy keletkezett, hogy vagy egy Kor-t, vagy egy
    // Teglalap-ot push_back-eltünk az Alakzatok listába. Igenám, de amikor
    // push_back-eltük, akkor levágódott a vége, és csak az a része maradt
    // meg, ami az Alakzat-ot ábrázolja a memóriában (lásd az ábrát:
    // ../images/struct-orokles-memoria-layout.png). Itt ugyan reménykedve
    // meghívjuk a terulet() metódust, de sajnos az nem a Kor vagy a
    // Teglalap, hanem csak az Alakzat terulet metódusa, ami lefut, így
    // aztán mindig nullát kapunk.
    double terulet = alakzatok[i].terulet();
    std::cout << "alakzat #" << i << ": terulet=" << terulet << std::endl;
    osszes_terulet += terulet;
  }

  std::cout << "Az összes terület: " << osszes_terulet << std::endl;
}