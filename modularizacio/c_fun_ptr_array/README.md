# C function pointer array példa

Egy minimalista példa arra, hogy hogyan lehet egy változót deklarálni, amelyik
C függvény-pointerek tömbjét tartalmazza. Két függvény, az `xxx(...)` és az
`yyy(...)` függvények pointereit letárolja egy `fuggvenyek` nevű tömbbe. Ezután
a bemenetről hármasával olvas számokat: az első szám 0 vagy 1 lehet, és az
választja ki, hogy melyik függvény hívódjon meg, a második és a harmadik számok
pedig paraméterek a híváshoz (lásd a [bemenet.txt](bemenet.txt) példafájlt).
