#include <stdio.h>
#include <math.h>

#include "feldolgozo.h"

int beolvas(double adatok[], int adatok_size) {
    int i = 0;
    double d;
    while (i < adatok_size && scanf("%lf", &d) == 1) {
        adatok[i] = d;
        ++i;
    }
    return i;
}

void statuszt_standard_errorra(double st) {
    fprintf(stderr, "Progress: %d%%\n", (int)round(st * 100));
}

int main() {
    double adatok[100];
    int n = beolvas(adatok, 100);
    fprintf(stderr, "Beolvasva %d szám.\n", n);

    void (*reportol)(double);
    reportol = &statuszt_standard_errorra;

    feldolgoz(adatok, n, reportol);
    return 0;
}
