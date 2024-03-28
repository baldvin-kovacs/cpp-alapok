# Game loop design minták

Egy egyszerű játékprogram magas szintű szervezésére tekintünk
át néhány ötletet.

Ahhoz, hogy már érdekes legyen, de még nem túl bonyolult, a
Commodore-os játékvilág egyik sztenderd struktúráját próbáljuk
megvalósítani:

1. Amikor a játék elindul, egy képernyőn bekér pár adatot, és
   egy gombbal el lehet indítani.
2. Utána van maga a játék.
3. Amikor vége van, akkor egy képernyőn be lehet adni pár
   karaktert, és esetleg egy új kört kezdeni, vagy kilépni.

![struktura](images/game-ux-structure.png)

## /01-kulon_kulon - A három képernyő, három külön programban

Ebben a könytárban a játék három fázisa három külön programban van
megírva. A cél szemléltetni, hogy milyen logika kell hozzájuk, és mit
kell majd egy programba összefésülni.

## /02-fuggetlen_main_loopok - Három külön main-loop.

Ebben a könyvtárban a fenti kód legegyszerűbben összefűzött változata
található. Egyszerűen a három main loop külön-külön implementálva, és
egy fővezér hívogatja őket.

## /03-scene - Egy main loop implementáció, de scene-enként futtatva.

Ebben már egy kicsit előre lépünk: rájövünk, hogy a main-loop-jaink
nagyon hasonlóak. Készítünk egy absztrakciót a *Scene*-nek, ami egy
játék-képernyőnek felel meg. Ez a *Scene* tartalmazza a main loop
logikáját. A leszármazott osztályok az egyes szabványosított main 
loop funkciókat specifikálják, virtuális metódusokon keresztül.

Ez a megoldás abból a szempontból előrelépés, hogy elég jól átláthatóvá
vált a kód, kicsit kevésbé spagetti, mert main loop részei szépen
külön függvényekben vannak. Az is hasznos, hogy bizonyos képességek
egy helyen vannak implementálva, például hogy az Escape billentyű
hatására fejezze be a program a futását.

Ez a megoldás annyiban különbözik egy modern framework-től, hogy egy
modern framework-ben valószínűleg egyetlen main loop futna a program
teljes élettartama alatt, és az egyes scene-ek beregisztrálhatók
illetve kivehetők lennének. Ehhez sokat kéne kódolni, anélkül hogy
fontos tanulsága lenne, ezért ezt itt nem demonstráljuk.

## Notes

- Akarjuk-e, hogy az egyes képernyők közötti váltás nagyon gyors
  lehessen? Ha igen, akkor kellhet tudni előtölteni a modelleket.