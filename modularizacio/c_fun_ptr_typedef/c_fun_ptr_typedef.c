#include <stdio.h>

typedef char (*feldolgozo_fuggveny)(int, double);

char csinalj_valamit(int a, double b) {
    printf("xxx: a=%d, b=%f\n", a, b);
    return 'x';
}

int main() {
    feldolgozo_fuggveny f;

    // Az f-be betesszük az csinalj_valamit függvény címét:
    f = &csinalj_valamit;

    // f egy függvény pointer, ezért a csillaggal dereferáljuk (hogy megkapjuk
    // a fuggvényt magat), majd a zárójelekkel meghívjuk.
    (*f)(3, 15.1);

    // Függvénypointerek eseten a cím képzése, és a dereferálás is elhagyható, ki
    // bírja találni a fordító, hogy mire gondoltunk:
    f = csinalj_valamit;
    f(3, 23.1);

    return 0;
}
