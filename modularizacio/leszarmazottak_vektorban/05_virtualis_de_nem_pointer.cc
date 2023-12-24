#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

class Alakzat {
 public:
  double x, y;
  void eltol(double dx, double dy);

  // Ebben a kísérletünkben kipróbáljuk, hogy a virtual kulcsszó csodát tesz-e,
  // csak hogy lássuk, nincsenek csodák...
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

      // Hopp, itt a baj: a k az egy Kor, de amikor push_back-eljük az alakzatok-ba,
      // akkor konstruálódik egy teljesen új Alakzat, amelynek az x és y adattagjai
      // a Kor-ből vannak, de a vtable-je az a sima Alakzat vtable. Nem is lehetne
      // korrekt a Kor vtable pointerét használni, mert az olyan metódusokra is mutat,
      // amelyek a többi adattag meglétét is feltételezik (az r adattagét, ebben a
      // példában). Ezért hibás a Kor vtable pointerét használni úgy, hogy valójában csak
      // az Alakzat adattagjai vannak a memóriában az adott helyen.
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
    // Még mindig csak nullákat kapunk, mert az alakzatok[i] csak Alakzat, semmi tobb!
    double terulet = alakzatok[i].terulet();
    std::cout << "alakzat #" << i << ": terulet=" << terulet << std::endl;
    osszes_terulet += terulet;
  }

  std::cout << "Az összes terület: " << osszes_terulet << std::endl;
}