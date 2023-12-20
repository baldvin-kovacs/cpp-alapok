#include <stdio.h>
#include <stdlib.h>

struct muvelet {
    double a, b;
    double (*vegrehajto)(double a, double b);
};

double osszead(double a, double b) {
    return a + b;
}

double kivon(double a, double b) {
    return a - b;
}

int beolvas(struct muvelet muveletek[], int muveletek_size) {
    int i = 0;
    double a, b;
    char muveleti_jel;
    while (i < muveletek_size && scanf("%lf %c %lf", &a, &muveleti_jel, &b) == 3) {
        double (*vh)(double a, double b);
        if (muveleti_jel == '-') {
            vh = &kivon;
        } else if (muveleti_jel == '+') {
            vh = &osszead;
        } else {
            exit(1);
        }
        muveletek[i].a = a;
        muveletek[i].b = b;
        muveletek[i].vegrehajto = vh;
        ++i;
    }
    return i;
}

void vegrehajt(struct muvelet muveletek[], int n) {
    for (int i = 0; i < n; ++i) {
        double eredmeny = (*(muveletek[i].vegrehajto))(muveletek[i].a, muveletek[i].b);
        printf("%f\n", eredmeny);
    }
}

int main() {
    struct muvelet muveletek[100];
    int n = beolvas(muveletek, 100);
    vegrehajt(muveletek, n);
}