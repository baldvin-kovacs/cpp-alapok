# A modularizáció alapja C++-ban

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

## Function pointer C-ben

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

Itt a változó neve a `fuggvenyek`, és mivel a kellős közepén van egy függvény típusának, egy
csillagtól jobbra, ezéert a `fuggvenyek` egy pointer lenne egy függvényre, hacsak nem lenne a
`[17]` is. Így a `fuggvenyek` egy tömb a memóriában, szépen egymás mellett 17 darab mutatónak
van hely benne.

A [c_fun_ptr_array](modularizacio/c_fun_ptr_array/) könyvtárban található egy minimalista
példa.







### Segít a typedef!


