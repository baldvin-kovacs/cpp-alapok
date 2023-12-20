# A modularizáció alapjai C++-ban

## Miért is kellene függvényre pointer?

Amikor egy függvényhívást írunk, akkor a fordítóprogram a függvényhívást
lefordítja gépi kódra úgy, hogy a processzor a következőket hajtja végre:

1. A függvény paramétereit eltárolja valahova (vagy regiszterekbe, vagy
   valamilyen memóriacímre.)
2. Ugrik arra a memóriacímre, ahol (gépi kódban) az adott függvény kezdődik.
3. Kiolvassa a paramétereket onnan, ahová el lettek tárolva.
4. Végrehajtja függvény törzsét, és az eredményt letárolja valahová.
5. Visszatér a függvényből.
6. Az eredményt kiolvassa onnan, ahová le lett tárolva.

Minden függvénynek van tehát egy memória címe, mégpedig az, ahová ugrani kell gépi
kódban, hogy a függvény lefusson.

Ha van a függvényeknek memória címe, akkor azt a címet el is lehet tárolni egy
változóban, hiszen ez nem más mint annak a bájtnak a sorszáma a memóriában, ahol
a függvény gépi kódú utasításai kezdődnek.

De mire kellhet nekünk az, hogy egy függvény címét eltároljuk? Hát ha egy függvényt
meg akarunk hívni, akkor meghívjuk és kész, nem?

Több példa is van arra, hogy nagyon hasznos változóba tárolni egy függvény memória
címét. Az egyik legfontosabb az úgynevezett *callback függvény* fogalma.

### Példa: státusz-jelentő callback függvény

Képzeljük el, hogy írunk egy programot, amelyik feldolgoz egy nagy adathalmazt.

Mondjuk tízmillió bemenő adatot dolgoz fel, és jó sokáig dolgozik rajta. Az első
változatban megírjuk úgy, hogy minden 1000. adat feldolgozása után írjon ki valamit
a standard errora, hogy lássuk, hogy halad. Később szeretnénk ezt a kódot úgy
használni, hogy a státuszt tudjuk egy grafikus progresszbáron mutatni.

Erre az egyenes megoldás valami ilyesmi pszeudokódú:

```C++
Progressbar progress_bar;

struct adat {...};

void beolvas(std::vector<adat>& adatok) {...}
void feldolgoz(std::vector<adat> const& adatok) {
    for (int i = 0; i < adatok.size(); ++i) {
        valamit_csinal(adatok[i]);
        if (i % 1000 == 0) {
            double statusz = static_cast<double>(i) / adatok.size();
            progress_bar.set(statusz);
        }
    }
}

int main() {
    std::vector<adat> adatok;
    beolvas(adatok);
    feldogoz(adatok);
}
```

Ezzel az a baj, hogy a `feldolgoz` függvénynek tudnia kell, hogy pontosan hogyan akarunk
majd státuszt jelezni. Ez azt jelenti, hogy nem tudjuk a `feldolgoz` függvényt egy
library-be tenni, és azt a libraryt felhasználni egyszerre két programból, egy olyanból, ami a
standard errorra ír státuszt, és egy másikból is, ami meg grafikus progresszbárt
használ.

Valami olyasmit szeretnénk kifejezni, hogy "kedves beolvas függvény, dolgozd fel ezeket
az adatokat, és időnként adj hírt a státuszról úgy, hogy meghívod ezt a függvényt, amit
itt mutatok neked".

Valahogy így, továbbra is pszeudokóddal:

```C++
void feldolgoz(std::vector<adat> const& adatok, valamilyen_alkalmas_tipus statusz_fuggveny) {
    for (int i = 0; i < adatok.size(); ++i) {
        valamit_csinal(adatok[i]);
        if (i % 1000 == 0) {
            double statusz = static_cast<double>(i) / adatok.size();
            statusz_fuggveny(statusz);
        }
    }
} 
```

Ezt utána felhasználjuk egy olyan fájlból, ami tudja, hogy hogyan akar státuszt jelezni:

```C++
void statusz_kiiro(double statusz) {
    std::cerr << "Statusz: " << statusz << std::endl;
}

int main() {
    std::vector<adat> adatok;
    beolvas(adatok);
    feldolgoz(adatok, &statusz_kiiro);
}
```

A kérdés már csak az, hogy a `valamilyen_alkalmas_tipus` helyére mit is kell írnunk
ahhoz, hogy ez működjön? Ahhoz, hogy a processzor odaugorjon a gépi kódú végrehajtásban,
elég egy sima memória cím, más néven egy *pointer*. Igenám, de az elején láttuk,
hogy más dolga is van ám a gépnek, le kell tárolnia a paramétereket valahogyan, azt
utána pedig ki kell olvasnia a függvénynek.

Így aztán ha csak egy sima pointert használnánk, akkor könnyen el tudnánk rontani, hogy
egy olyan státusz kiiró függvényt adunk be a feldolgozónak, ami más paramétereket fogad,
vagy mást ad vissza. A program jó eséllyel hibával befejeződne, rosszabb esetben
előre nem tudható hülyeségeket csinálna.

A probléma kulcsa, hogy a fejezet legelején levő lépések közül az 1. és a 3. kompatibilisen
kell, hogy működjön. Márpedig az, hogy hogyan kell paramétereket átadni egy függvénynek az
attól függ, hogy milyen típusú, és hány paramétert fogad a függvény. Ha a kód, ami meghívja
a függvényt, az máshogy készíti elő a terepet, mint amire a függvény számít, abból bajok
lehetnek.

Ahhoz, hogy a fordítóprogram ellenőrizni tudja, hogy nem vétünk-e ilyen hibát, azaz tényleg
olyan típusú függvényre mutató pointert adunk át, mint amire épp ott szükség van, a C és
a C++ is ad megoldást. A C-ben elég nehézkes volt a dolog, ezért a C++ bevezetett saját, másik
megoldást. A következőkben végignézünk hármat.

## Függvény pointer C-ben

Egy függvény típusa két dologból áll össze: hogy a függvény milyen paramétereket vesz be, és hogy
milyen eredményt ad vissza. C-ben nagyon furcsán kell ezt írni.

Vegyünk például egy függvényt, ami egy `int` és egy `double` paramétert vesz be, és `char`-t ad vissza:

```C
char valamilyen_fuggveny(int a, double b) { ... }
```

Az erre a függvényre mutató pointer típusa:

```C
// Ez itt egy típus:
char (*)(int, double)
```

Azt hinnénk, hogy egy változót, amibe ilyen pointereket lehet tárolni, úgy kell létrehozni,
hogy leírjuk ezt a típust, és utána a változónevet, mint egyéb normális típusok esetén.
Sajnos nem: ez egy kivétel, a változónevet a típus kellős közepébe, a csillag után kell írni.

Azaz, ha létre szerenénk hozni egy `fuggvenyem` nevű változót, amibe ilyen típusú
függvénypointereket lehet beletenni, azt így kell:

```C
// Ez itt magának egy változónak a deklarálása, a változó neve "fuggvenyem":
char (*fuggvenyem)(int, double);
```

A [c_callback](modularizacio/c_callback/) példaprogram mutatja be ezt egy egyszerű esetre. A
[feldolgozo.h](modularizacio/c_callback/feldolgozo.h) fájl deklarálja a feldolgozó függvényt.
A [feldolgozo.c](modularizacio/c_callback/feldolgozo.c) implementálja a feldolgozó függvényt.
A `.h` fájlra azért van szükség, hogy a függvényt felhasználó programok (esetünkben a
[c_callback_example.c](modularizacio/c_callback/c_callback_example.c)) be tudják include-olni
csak azt, hogy hogyan néz ki a függvény, miközben az implementációt egy library-be
fordítjuk le, nem pedig magával a programmal együtt. A [Makefile](modularizacio/c_callback/Makefile)-ban
látható, hogy a `feldolgozo.o` külön fordítódik, és lefordított (object) fájlként
van hozzálinkelve a végső binárishoz (a `c_callback_exammple`-höz).

### Függvény pointerek struktúrában 

Képzeljük el, hogy be kell olvasnunk egy csomó műveletet, de nem rögtön végrehajtani, hanem
csak eltárolni, későbbi végrehajtáshoz. Valami ilyesmi inputunk van:

```txt
3 + 5
4 - 6
6 + 8
```

Ezt egyszerűen eltárolhatjuk egy olyan struktúrában, amelyik egy sima karakterben kódolja, hogy
mit is csinálunk a két számmal:

```C
struct muvelet {
    double a, b;
    char muveleti_jel;
};
```

Ez lehet egy teljesen jó megoldás, de nem jó akkor, ha ezeket a műveleteket egy library-nek
kell beadnunk, ami nem tudja, hogy mit is jelent a `+` és a `-`. Ilyenkor járhatunk
jobban egy olyan adatstruktúrával, ami "tudja magától", hogy hogyan kell a műveletet elvégezni:

```C
struct muvelet {
    double a, b;
    double (*vegrehajto)(double a, double b);
};
```

Ez jó hajmeresztően néz ki, de ha jól figyeltünk a fejezet eddigi részében, akkor nem fog ki
rajtunk: a `vegrehajto` a *neve* az adattagnak! A típusa pedig `double (*)(double a, double b)`,
tehát egy függvény pointer, ami olyan függvényekre tud mutatni, amelyek két double paramétert
kapnak, és egy double-t adnak vissza.

Erre egy példát a [c_struktura](modularizacio/c_struktura/) példaprogramban láthatunk.
 
### Függvény pointerek tömbje

C-ben nem csak a függvény pointereknek van fura szintaktikája, hanem a tömbnek is:

```C
double szamok[17];
```

Ez azt jelenti, hogy a `szamok` változó egy olyan memóriaterület, amelybe sorban 17 `double` fér
bele, azaz egy 17 hosszú tömb. Gondolhatnánk, hogy a `double[17]` a típus, és azt kéne előre írni,
majd utána a változó nevét, de nem így van. A változó nevét egy tömb, -- angolul *array* -- esetén
középre kell írni, és kész.

Ezek után már nem okozhat nagy nehézséget elfogadni, hogy függvény pointerek tömbjének a változóját
még furcsábban kell írni.

```C
char (*fuggvenyek[17])(double, int);
```

Itt a változó neve a `fuggvenyek`, és mivel a kellős közepén van egy függvényre mutató pointer
típusának, ezért a `fuggvenyek` egy pointer lenne egy függvényre, hacsak nem lenne a
`[17]` is. Emiatt a `[17]` miatt a `fuggvenyek` egy tömb lesz a memóriában, szépen egymás
mellett 17 darab mutatónak van hely benne.

A [c_fun_ptr_array](modularizacio/c_fun_ptr_array/) könyvtárban található egy minimalista
példa.

### Segít a typedef!

Noha jó ismerni a függvénypointerek furcsa szintaktikáját, valójában szinte sohasem használjuk
ebben a formában. Nekünk tudnunk kell, hogy megértsük, ha valaki más ilyen kódot írt, de
amikor mi írunk kódot, akkor mindig próbáljuk úgy, hogy egyszerűbben olvasható legyen.

Nézzük a szokásos következő példánkat:

```C
char (*x)(int, double);
```

Mi az `x`? Egy változó. Miféle változó? Egy pointer, tehát egy szám, ami megadja valaminek
a helyét a memóriában.

Mi az `x` típusa? Az `x` pontos típusa: *int-et és double-t paraméterként vevő, char-t visszaadó
függvényre mutató pointer*.

Ezt az utóbbi mondatot kódban is kifejezhetjük, adhatunk egy nevet ennek a típusnak. Ha mondjuk
valaminek a feldolgozására használunk ilyen függvényeket, akkor definiálhatjuk az ilyen függvényekre
mutató pointer típust:

```C
typedef char(*feldolgozo_fuggveny)(int, double);
```

Ez pontosan ugyanúgy néz ki, mintha a `feldolgozo_fuggveny` egy változó lenne, de mivel az egész
előtt ott van, hogy typedef, ezért a `feldolgozo_fuggveny` nem egy változó, hanem egy típus.
Egészen pontosan egy úgynevesett *alias*, egy másik név a függvénypointer típusra, amelyik egy ˙int˙-et
és egy `double`-t paraméterként vevő, `char`-t visszaadó függvényre tud mutatni.

Miután így definiáltunk egy saját nevet ennek a típusnak utána már a szokásos módon tudunk
ilyen változót definiálni:

```C
feldolgozo_fuggveny fuggvenyre_mutato_pointer;
```

Mivel ez egy pointer, ezért használhatjuk a normál szintaktikát, az `&` jelet hogy egy függvény
címét megkapjuk, majd a `*` jelet hogy abból visszakapjuk a függvényt, hogy meghívhassuk:

Feltéve, hogy van egy pont megfelelő típusú `char csinalj_valamit(int a, int b)` függvényünk is, ezek
után írhatjuk, hogy:

```C
fuggvenyre_mutato_pointer = &csinalj_valamit;
char c = (*fuggvenyre_mutato_pointer)(27, 1.2);
```

Mivel a függvényt C-ben úgyis csak pointerként tudjuk eltárolni, ezért elhagyhatjuk a
speciális karaktereket, azaz az `&`-t és a `*`-ot, hiszen úgyis tudja a fordító, hogy mire
gondolunk:

```C
fuggvenyre_mutato_pointer = csinalj_valamit;
char c = fuggvenyre_mutato_pointer(27, 1.2);
```
A [c_fun_ptr_typedef](modularizacio/c_fun_ptr_typedef/) mutatja ezeket fordítható kódban.

## Class-ok

It a class-oknak csak azt az icipici oldalát tekintjük át, ami szükséges lesz a későbbiekben
a modularizációs technikák ismertetéséhez.

### Egyszerű, virtuális metódus nélküli osztályok

Képzeljük el, hogy köröket akarunk ábrázolni, megvan a középpontjuk, meg a sugaruk. Be akarunk
olvasni ilyenből egy csomót. A programban néha szükségünk lesz a területükre, a kerületükre,
hogy milyen távol van a középpontjuk az origótól, esetleg más hasonló dolgokra is. Szükségünk
lehet néhány módosító függvényre is, például el akarhatjuk tolni a kört kicsit jobbrább és feljebb.

C-ben ehhez definiálunk egy struktúrát, majd írunk függvényeket, amik kiszámolják a dolgokat,
amikre szükségünk van:

```C

struct kor {
    double x, y;
    double r;
};

double terulet(struct kor* k) {
    return 3.14 * k->r * k->r;
}

void arrebb_tol(struct kor* k, double dx, double dy) {
    k->x += dx;
    k->y += dy;
}

int main() {
    struct kor k;
    k.x = 5.0;
    k.y = 7.0;
    k.r = 1.0;

    arrebb_tol(&k, 1, 3);
    printf("x=%f, y=%f, terulet=%f\n", k.x, k.y, terulet(&k));
}

```

Minél több ilyen függvényt írunk, annál jobban körvonalazódik egy minta: minden ilyen függvény
természetes első paramétere egy mutató (*pointer*) arra, hogy hol van a memóriában a kört
reprezentáló struktúra, és utána a többi esetleges paraméter, amelyek megmondják, hogy mit
csináljon vele a függvény.

A C++ osztály fogalma nem más, mint erre egy szép szintaktika. Sem nem több, sem nem jobb ennél,
pontosan így működik.

Az előző kódnak pontosan megfelelő kód, C++ osztályt használva:

```C++
class Kor {
 public:
  double x, y;
  double r;

  double terulet();
  void arrebb_tol(double dx, double dy);
};

double Kor::terulet() {
    return r * r;
}

void Kor::arrebb_tol(double dx, double dy) {
    x += dx;
    y += dy;
}

int main() {
    Kor k;
    k.x = 5.0;
    k.y = 7.0;
    k.r = 1.0;
    
    k.arrebb_tol(1, 3);
    std::cout << "x=" << k.x << ", y=" << k.y << ", terulet=" << k.terulet() << std::endl; 
}
```

Megjegyzések:

1. C++-ban `class` helyett írhatunk `struct`-ot, és akkor minden publikus lesz magától, akkor
   nem kell a `public:`. C++-ban `struct`-nak is lehetnek metódusai. Tehát a C++-os `struct`
   az közelebb van a `class`-hoz, mint a C-s `struct`-hoz.
2. C++-ban a metódusokat implementálhatjuk közvetlenül a `class {...}`-on belül is. Itt azért
   a külső implementációt választottam (a `::`-os változatot), hogy jobban hasonlítson a C-s
   kódhoz. A valóságban legtöbbször akkor használjük a külső implementációt, amikor a metódusokat
   egy külön fájlban szerenénk fordítani, hogy library-ként felhasználhatók legyenek.

Az osztályoknak még rengeteg egyéb képessége van, de azokról itt nem írok részletesen. Ennek a
fejezetnek az egyetlen lényeges üzenete, hogy az osztályok metódusaira gondoljunk mindig úgy,
ahogy a fenti C-s implementációban kinéznek: egy sima függvény, ami kap egy pointert arra az
osztály-példányra, amin a metódust meghívjuk. Tehát ha ezt látjuk

```C++
k.arrebb_tol(double dx, double dy);
```

akkor erre úgy gondolunk, hogy "egy dx, dy paraméterű metódus, amit a k osztálypéldányon meghívtunk",
de valójában úgy is gondolhatunk rá, hogy ez "egy egyszerű &k, dx, dy paraméterű függvény". Azaz
a metódus a háttérben nem más, mint egy függvény, ami kap egy pointert az osztály példányára, amin fut.


