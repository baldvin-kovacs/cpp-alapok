#include <stdio.h>

char xxx_fuggveny(int a, double b) {
    printf("xxx: a=%d, b=%f\n", a, b);
    return 'x';
}

char yyy_fuggveny(int a, double b) {
    printf("yyy: a=%d, b=%f\n", a, b);
    return 'y';
}

int main() {
    char (*fuggvenyek[2])(int a, double b);
    fuggvenyek[0] = &xxx_fuggveny;
    fuggvenyek[1] = &yyy_fuggveny;

    int melyik_fuggveny;
    int a;
    double b;
    while (scanf("%d %d %lf", &melyik_fuggveny, &a, &b) == 3) {
        (*fuggvenyek[melyik_fuggveny])(a, b);
    }
}
