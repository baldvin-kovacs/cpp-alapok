#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

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

int main() {
  // No itt a vektorba már pointereket teszünk. Ez jó is meg rossz is, egyfelől
  // elérjük amit szerettünk volna, másfelől elveszítettük annak a hatékonyságát,
  // hogy az alakzatunkat reprezentáló bájtok azok a memóriában egymáshoz közel,
  // szépen egymás után legyenek. Ezzel a megoldással a vektorba tett alakzatok
  // tényleges tartalma a heap-en egyesével foglalt területeken van, a vektorban
  // pedig csak mutatók ezekre a helyekre a heap-en. A vektorban minden elemnek
  // ugyanolyan hosszúnak kell lennie, és hát a pointerek ugyanolyan hosszúak,
  // függetlenül attól, hogy milyen hosszú adatra mutatnak...
  std::vector<Alakzat*> alakzatok;
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
      // A "new Kor" lefoglal a heap-en annyi helyet, amennyi a Kor-nek kell,
      // és inicializálja a default konstruktorral, majd visszaadja a pointert,
      // ami a heap-en lefoglalt helyre mutat, tehát ahol a tényleges osztály példány
      // van.
      Kor* k = new Kor;
      k->x = x;
      k->y = y;
      k->r = r;
      alakzatok.push_back(k);
    } else {
      double x, y, top, left;
      std::cin >> x >> y >> top >> left;
      if (!std::cin) {
        throw std::runtime_error("nem tudtam egy téglalapot beolvasni");
      }
      Teglalap* t = new Teglalap;
      t->x = x;
      t->y = y;
      t->top = top;
      t->left = left;
      alakzatok.push_back(t);
    }
  }

  double osszes_terulet = 0.0;
  for (std::size_t i = 0; i < alakzatok.size(); ++i) {
    double terulet = alakzatok[i]->terulet();
    std::cout << "alakzat #" << i << ": terulet=" << terulet << std::endl;
    osszes_terulet += terulet;
  }

  std::cout << "Az összes terület: " << osszes_terulet << std::endl;
}