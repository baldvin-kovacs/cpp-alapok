#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <utility>

class Alakzat {
 public:
  double x, y;
  void eltol(double dx, double dy);

  // Az `= 0` a függvény deklarációjában azt jelenti, hogy ebben az osztályban
  // nem adunk implementációt hozzá egyáltalán, azaz ez a metódus egy
  // úgynevezett "absztrakt" metódus. Olyan osztályokból, amelyeknek van
  // absztrakt metódusa nem lehet példányt készíteni, az ilyen osztályok csak
  // arra valók, hogy leszármaztatni lehessen belőlük. 
  virtual double terulet() = 0;
  
  // Ha egy bázis osztályt írunk, akkor majdnem mindig deklarálunk virtuális
  // destruktort. Ez akkor fontos, ha előfordulhat, hogy egy leszármazott
  // osztály példányát a bázis osztály pointerén keresztül szabadítjuk fel. Ha
  // nem virtuális a destruktor, akkor a következő esetben helytelen desktruktor
  // hívódik:
  //
  // Alakzat* kor = new Kor;
  // delete kor;
  //
  // Pont ebben a fájlban ilyet nem csinálunk, úgyhogy itt nem kell virtuális
  // destruktor.
  //
  //virtual ~Alakzat() {};
};

void Alakzat::eltol(double dx, double dy) {
  x += dx;
  y += dy;
}

class Kor : public Alakzat {
 public:
  double r;

  // Az override azt jelenti, hogy ez a metódus egy virtuális metódust ír felül.
  // Használata nem kötelező, jó sokáig nem is volt a nyelv része. A különböző
  // céges vagy projekt specifikus *style guide*-ok viszont gyakran kötelezővé
  // teszik a használatát.
  double terulet() override;
};

double Kor::terulet() {
  return 3.14 * r *r;
}

class Teglalap : public Alakzat {
 public:
  double top, left;

  // A példa kedvéért itt helyben adjuk meg a metódus implementációját. Ha egy
  // fájlban dolgozunk, akkor szinte csak így szokás. Akkor szokás külön bontani
  // a deklarációt és az implementációt, ha utóbbit szeretnénk külön lefordítani.
  // Na de milyen előnye van a külön fordításnak? Az egyik, hogy akkor csak egyszer
  // kell lefordítani magát a logikát, nem pedig mindenhol, ahol felhasználjuk.
  // Mára annyira gyorsak a processzorok, hogy ez nem mindenkit győzne meg arról,
  // hogy megéri vesződni a bonyolultsággal. (Noha elképzelhetjük, hogy egy nagy
  // vállalati kódban, sok tízmillió kódsorral már számít.) Egy másik előny lehet,
  // hogy a külön fordított kódról biztosak lehetünk abban, hogy mindenhol ahol
  // használjuk, pontosan ugyanaz a változat fut. A deklarációban megadott kód
  // ebből a szempontból mindig kicsit "gyanúsabb", mi van, ha már fordítottunk
  // egy library-t, amiben még más változata volt a deklarációban megadott kódnak,
  // és most a programunk is használja, de már egy újabb változatot? Emiatt általában
  // csak az egészen triviális függvényeket implementáljuk közvetlenül a deklarációban,
  // vagy azokat, amelyeket teljesítmény-optimalizálni szeretnénk (például "inline"-olni,
  // azaz arra kérni a fordítót, hogy másolja be közvetlenül az összes felhasználási
  // helyre).
  double terulet() {
    double szelesseg = x - left;
    double magassag = y - top;
    return szelesseg * magassag;
  }
};

// Az AlakzatTarolo egy olyan "Union like class" (https://en.cppreference.com/w/cpp/language/union),
// amelyik pontosan kétféle objektumot tud tárolni, vagy egy kört, vagy egy téglalapot.
// Az AlakzatTarolo mérete a memóriában annyi, hogy bármelyik beleférjen, tehát a jelen
// esetben egy téglalap mérete, plusz a "melyik" nevű enum mérete, amelyik tárolja, hogy
// az union-beli lehetőségekből melyik van tényleg kitöltve.
struct AlakzatTarolo {
  // A melyik nevű adattag egy névtelen enum, amelyik pontosan két értéket vehet fel, vagy
  // KOR-t, vagy TEGLALAP-ot. A valóságban ez egy int lesz, és vagy 0, vagy 1 lesz az
  // érték, de erre nincs garancia, a fordító dönthet úgy, hogy másik számok.
  enum {KOR, TEGLALAP} melyik;
  
  // Az union-t úgy kell érteni, hogy vagy-vagy. Lehet benne többféle is, nem csak kettő,
  // de egyszerre sosem tudunk egynél többet használni, mert ugyanaz a memória van nekik
  // lefoglalva.
  union {
    Kor kor;
    Teglalap teglalap;
  };

  // Általában egy "union like class" működik magától, de csak akkor, ha nem virtuális
  // metódusos osztályok vannak benne. Ha igen, és nekünk most igen, akkor sajnos muszáj
  // néhány dolgot magunk intéznünk. Például ebben az esetben meg kell oldanunk, hogy
  // meghívódjon a konstruktora annak a union tagnak, amelyiket éppen ki szeretnénk
  // tölteni. Erre több megoldás is létezik, itt létrehozunk két konstruktort, mindkettő
  // megfelelően delegálja a munkát a többi adattag konstruktorainak.
  AlakzatTarolo(Kor const& k) : melyik{KOR}, kor{k} {};
  AlakzatTarolo(Teglalap const& t) : melyik{TEGLALAP}, teglalap{t} {}
  
  // Az AlakzatTarolo-t akarjuk vektorban használni. A vektor néha szeretné reallokálni
  // a memóriát, amiben az adatok vannak (mindig, amikor nőni akar, de már nem tud).
  // Ezt úgy tudja megtenni, ha valahogy át tudja másolni az adatokat a régi helyről
  // az új helyre. Ehhez vagy egy copy, vagy egy move constructorra van szüksége. Mi
  // itt egy hagyományos copy constructort adunk neki.
  AlakzatTarolo(AlakzatTarolo const& at) : melyik{at.melyik} {
    if (at.melyik == KOR) {
      new (&kor) Kor;
      kor = at.kor;
      return;
    }
    new (&teglalap) Teglalap;
    teglalap = at.teglalap;
  }
  
  // Egy szimpla union-nak fogalma sincs, hogy melyik adattagját használjuk éppen.
  // Pont ezért használunk egy plusz enum-ot, hogy ezt eltároljuk. Mivel egy sima
  // union nem tudja, hogy épp melyik adattagját használjuk, ezért nem is tudja őket
  // lebontani, hiszen nem tudja, hogy melyik destructor-t hívja meg. Ezért ezt itt
  // kézzel kell.
  ~AlakzatTarolo() {
    if (melyik == KOR) {
      // A destructor egy sima metódus, furcsa névvel. Általában nem kell, sőt, általában
      // nem is szabad hívogatni, mert általában valami gondoskodik a meghívásáról. Ha
      // egy sima lokális változóról van szó, akkor például a nyelv maga garantálja,
      // hogy amikor "kimegy a scope-ból", akkor a destruktor meg legyen hívva. De
      // mivel a union-nak fogalma sincs arról, hogy melyik adattagját használjuk,
      // ezért nem is tudja maga meghívni a destructor-jukat, az nekünk kell kézzel.
      kor.~Kor();
      return;
    }
    teglalap.~Teglalap();
  }
  
  // Na ezért gimnasztikáztunk oly sokat: az alakzat() függvény visszaad egy referenciát
  // a bázis osztály típusával. Mivel a terulet() virtuális, ezért a bázis osztály
  // referenciáján hívva is a Kor vagy a Teglalap területét adja vissza.
  Alakzat& alakzat() {
    if (melyik == KOR) {
      return kor;
    }
    return teglalap;
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
      Kor kor;
      kor.x = x;
      kor.y = y;
      kor.r = r;
      alakzatok.push_back(AlakzatTarolo{kor});
    } else {
      double x, y, top, left;
      std::cin >> x >> y >> top >> left;
      if (!std::cin) {
        throw std::runtime_error("nem tudtam egy téglalapot beolvasni");
      }
      Teglalap teglalap;
      teglalap.x = x;
      teglalap.y = y;
      teglalap.top = top;
      teglalap.left = left;
      alakzatok.push_back(AlakzatTarolo{teglalap});
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