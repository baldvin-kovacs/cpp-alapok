#include "feldolgozo.h"

#include <stdio.h>

void feldolgoz(double adatok[], int adat_size, void(*statusz_report)(double) ) {
    double osszeg = 0;
    for (int i = 0; i < adat_size; ++i) {
        osszeg += adatok[i];
        double statusz = (double)(i + 1) / adat_size;
        (*statusz_report)(statusz);
    }
    printf("Az összeg: %f\n", osszeg);
}
