#include <string.h>
#include <stdio.h>

#include "prime.h"

int isOnlyHex(char *s) {
    for (int i = 0; s[i] != 0; i++) {
        if (s[i] < 48 || (s[i] > 57 && s[i] < 65) || (s[i] > 70 && s[i] < 97) || s[i] > 102) {
            return 0;
        }
    }
    return 1;
}

int isOnlyInt(char *s) {
    for (int i = 0; s[i] != 0; i++) {
        if (s[i] < 48 || s[i] > 57) {
            return 0;
        }
    }
    return 1;
}

int strToInt(char *s) {
    int r = 0;
    for (int i = 0; s[i] != 0; i++) {
        r = r * 10 + s[i] - 48;
    }
    return r;
}

int main(int argc, char **argv) {
    int flag = 0;
    int toHex = 0;
    char *prime = NULL;
    int nbBits = 0;
    if (argc < 3 || argc > 4) {
        printf("use with -g <nbBits> [--hex] or -t <hexNumber>\n");
        return -1;
    }
    if (!strcmp(argv[1], "-g") && isOnlyInt(argv[2])) {
        if (argc == 4 && !strcmp("--hex", argv[3])) {
            toHex = 1;
        }
        nbBits = strToInt(argv[2]);
    }
    else if (!strcmp(argv[1], "-t") && isOnlyHex(argv[2])) {
        if (argc == 4) {
            printf("use with -g <nbBits> [--hex] or -t <hexNumber>\n");
            return -1;
        }
        flag = 1;
        prime = argv[2];
    }
    else {
        printf("use with -g <nbBits> [--hex] or -t <hexNumber>\n");
        return -1;
    }
    if (flag) {
        BIGNUM *a = BN_new();
        BN_hex2bn(&a, prime);
        return isPrime(a);
    }
    return createPrime(nbBits, toHex);
}