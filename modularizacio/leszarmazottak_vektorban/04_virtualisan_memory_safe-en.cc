#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>
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
  // Az std::unique_ptr<Valami> egy olyan oszály, amely egy Valami* típusú adattagot
  // tárol. Miért jobb, mint simán Valami*-ot használni? Két fő oka van. Az első, hogy
  // az std::unique_ptr<Valami> úgy van megírva, hogy amikor a destruktora fut, tehát
  // amikor felszámolódik, akkor lefuttatja a Valami desktruktorát is. Így sosem
  // felejtjük el a "free valami" utasítást, a unique_ptr megcsinálja helyettünk. A
  // másik, hogy a unique_ptr vigyáz arra, hogy összesen egy helyen lehessen érvényes
  // példánya a valami-re mutató pointernek. Amikor átadjuk vagy egy függvénynek, vagy
  // beletesszük egy vektorba, akkor muszáj az std::move-val jeleznünk, hogy az
  // std::unique_ptr példányában levő Valami*-ot teljesen átadtuk, itt helyben már
  // nem nyúlunk hozzá. 
  std::vector<std::unique_ptr<Alakzat>> alakzatok;
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
      std::unique_ptr<Kor> k = std::make_unique<Kor>();
      k->x = x;
      k->y = y;
      k->r = r;
      alakzatok.push_back(std::move(k));
      // Mivel az std::move-val adtuk át a k-t, ezért itt már nem szabad hozzányúlnunk,
      // tehát nem írhatnánk már például, hogy
      // k->r = 27.0;
      // Ha hozzá szeretnénk férni, akkor az alakzatok-on keresztül kellene:
      // alakzatok[alakzatok.size()-1]->r = 27.0;
    } else {
      double x, y, top, left;
      std::cin >> x >> y >> top >> left;
      if (!std::cin) {
        throw std::runtime_error("nem tudtam egy téglalapot beolvasni");
      }
      std::unique_ptr<Teglalap> t = std::make_unique<Teglalap>();
      t->x = x;
      t->y = y;
      t->top = top;
      t->left = left;
      alakzatok.push_back(std::move(t));
    }
  }

  double osszes_terulet = 0.0;
  for (std::size_t i = 0; i < alakzatok.size(); ++i) {
    // A unique_ptr úgy van megírva, hogy a pointerekhez megszokott normális operátorok
    // mind azt csinálják, amit várunk. Tehát például a `->` operátor ugyanazt adja vissza,
    // mintha az alakzatok[i] egy sima Alakzat* lenne, nem pedig egy Alakzat*-ot tartalamazó
    // unique_ptr.
    double terulet = alakzatok[i]->terulet();
    std::cout << "alakzat #" << i << ": terulet=" << terulet << std::endl;
    osszes_terulet += terulet;
  }

  std::cout << "Az összes terület: " << osszes_terulet << std::endl;
}