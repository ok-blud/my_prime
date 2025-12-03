#include "prime.h"

PrimeTesteur *initPrimeTesteur() {
    PrimeTesteur *data = malloc(sizeof(PrimeTesteur));
    mt_state *states = malloc(sizeof(mt_state));
    if (!data || !states) {
        if (data) {
            free(data);
        }
        if (states) {
            free(states);
        }
        printf("Couldn't malloc in \"%s\".\n", "initPrimeTesteur");
        return NULL;
    }
    unsigned char nb[1] = {2};
    BIGNUM *two = BN_bin2bn(nb, 1, NULL);
    if (!two) {
        free(data);
        free(states);
        printf("Couldn't BIGNUM allocate in \"%s\".\n", "initPrimeTesteur");
        return NULL;
    }
    data->two = two;
    nb[0] = 3;
    BIGNUM *three = BN_bin2bn(nb, 1, NULL);
    if (!three) {
        free(data);
        free(states);
        BN_free(two);
        printf("Couldn't BIGNUM allocate in \"%s\".\n", "initPrimeTesteur");
        return NULL;
    }
    data->three = three;
    BN_CTX *ctx = BN_CTX_new();
    if (!ctx) {
        free(data);
        free(states);
        BN_free(two);
        BN_free(three);
        printf("Couldn't BIGNUM allocate in \"%s\".\n", "getRandomForMR");
        return NULL;
    }
    data->ctx = ctx;
    uint32_t t = (uint32_t) time(NULL);
    initialize_state(states, t);
    data->states = states;
    data->head = NULL;
    data->prime = NULL;
    data->primeMinus3 = NULL;
    data->twoPower32k = NULL;
    data->k = 0;

    return data;
}

void clearLinkedList(PrimeTesteur *testeur) {
    if (testeur) { 
        node *tmp = NULL;
        while (testeur->head) {
            tmp = testeur->head;
            BN_free(tmp->nb);
            testeur->head = testeur->head->next;
            free(tmp);
        }
    }
}

void clearPrime(PrimeTesteur *testeur) {
    if (testeur) {
        if (testeur->prime) {
            BN_free(testeur->prime);
            testeur->prime = NULL;
        }
        if (testeur->primeMinus3) {
            BN_free(testeur->primeMinus3);
            testeur->primeMinus3 = NULL;
        }
    }
}

void clearPrimeGenerated(PrimeTesteur *testeur) {
    if (testeur) {
        clearLinkedList(testeur);
        clearPrime(testeur);
        if (testeur->states) {
            free(testeur->states);
            testeur->states = NULL;
        }
        if (testeur->two) {
            BN_free(testeur->two);
            testeur->two = NULL;
        }
        if (testeur->three) {
            BN_free(testeur->three);
            testeur->three = NULL;
        }
        if (testeur->twoPower32k) {
            BN_free(testeur->twoPower32k);
            testeur->twoPower32k = NULL;
        }
        if (testeur->ctx) {
            BN_CTX_free(testeur->ctx);
            testeur->ctx = NULL;
        }
        free(testeur);
    }
}

int addElement(PrimeTesteur *testeur, BIGNUM* nb) {
    node *ele = malloc(sizeof(node));
    if (!ele) {
        printf("Couldn't malloc in \"%s\".\n", "addElement");
        return -1;
    }
    ele->next = testeur->head;
    ele->nb = nb;
    testeur->head = ele;
    return 0;
}

int preparePrimeTesteur(PrimeTesteur *testeur) {
    BIGNUM *prime = testeur->prime;

    BIGNUM *primeMinus3 = BN_new();
    if (!primeMinus3) {
        printf("Couldn't BIGNUM allocate in \"%s\".\n", "preparePrimeTesteur");
        return -1;
    }
    testeur->primeMinus3 = primeMinus3;
    if (!BN_sub(primeMinus3, prime, testeur->three)) {
        printf("Couldn't BIGNUM allocate in \"%s\".\n", "preparePrimeTesteur");
        return -1;
    }

    int nbBits = BN_num_bits(prime);

    unsigned char nb[1] = {0};
    BIGNUM *twoPower32k = BN_bin2bn(nb, 1, NULL);
    if (!twoPower32k) {
        printf("Couldn't BIGNUM allocate in \"%s\".\n", "preparePrimeTesteur");
        return -1;
    }
    testeur->twoPower32k = twoPower32k;
    testeur->k = ((nbBits / 32) + ((nbBits % 32) > 0));
    if (!BN_set_bit(twoPower32k, testeur->k * 32)) {
        printf("Couldn't BIGNUM allocate in \"%s\".\n", "preparePrimeTesteur");
        return -1;
    }

    return 0;
}

int getRandomForMR(PrimeTesteur *testeur) {
    /** 
     * Par principe pour millerRabin on a besoin de tester la primalité d'un nombre premié p par rapport
     * à des bases comprise dans [2, p-1[. On va donc chercher a generer un nombre entre [0, n-3[ auquel on rajoutera 2.
     * Vu que notre PRNG genere des entiers de 32 bits maximum on va donc generer un entier dans [0, 2 ^ (32 * k)[.
     * Où k est le nombre de bits composant notre prime diviser par 32 arondi au prochain entier.
     * Depuis notre entier generer on va utiliser une regle de 3 pour le rapporter dans [0, p-3[.
     */

    unsigned char *nb = malloc(4 * testeur->k * sizeof(char));
    if (!nb) {
        printf("Couldn't malloc in \"%s\".\n", "getRandomForMR");
        return -1;
    }
    
    uint32_t gen;
    for (int i = 0; i < testeur->k; i++) {
        gen = random_uint32(testeur->states);
        nb[i * 4]     = (gen >> 24) & 0xff;
        nb[(i * 4) + 1] = (gen >> 16) & 0xff;
        nb[(i * 4) + 2] = (gen >> 8) & 0xff;
        nb[(i * 4) + 3] = gen & 0xff;
    }

    BIGNUM *randomNumber = BN_bin2bn(nb, testeur->k * 4, NULL);
    free(nb);
    if (!randomNumber) {
        printf("Couldn't BIGNUM allocate in \"%s\".\n", "getRandomForMR");
        return -1;
    }
    
    BIGNUM *scaledRandom = BN_new();
    if (!scaledRandom || !BN_mul(scaledRandom, testeur->primeMinus3, randomNumber, testeur->ctx)) {
        if (scaledRandom) {
            BN_free(scaledRandom);
        }
        BN_free(randomNumber);
        printf("Couldn't BIGNUM allocate in \"%s\".\n", "getRandomForMR");
        return -1;
    }
    BN_free(randomNumber);
    BIGNUM *finalRandom = BN_new();
    if (!finalRandom || !BN_div(finalRandom, NULL, scaledRandom, testeur->twoPower32k, testeur->ctx)) {
        if (finalRandom) {
            BN_free(finalRandom);
        }
        BN_free(scaledRandom);
        printf("Couldn't BIGNUM allocate in \"%s\".\n", "getRandomForMR");
        return -1;
    }
    BN_free(scaledRandom);

    if (!BN_add(finalRandom, testeur->two, finalRandom)) {
        BN_free(finalRandom);
        printf("Couldn't BIGNUM allocate in \"%s\".\n", "getRandomForMR");
        return -1;
    }

    if (addElement(testeur, finalRandom)) {
        BN_free(finalRandom);
        printf("Couldn't addElement in \"%s\".\n", "getRandomForMR");
        return -1;
    }
    return 0;
}

int millerRabin(PrimeTesteur *testeur) {
    

    BIGNUM *pMinus1 = BN_new();
    if (!pMinus1 || !BN_add(pMinus1, testeur->primeMinus3, testeur->two)) {
        if (pMinus1) {
            BN_free(pMinus1);
        }
        printf("Couldn't BIGNUM allocate in \"%s\".\n", "millerRabin");
        return -1;
    }
    
    unsigned int s = 0;
    while (!BN_is_bit_set(pMinus1, s)) {
        s++;
    }

    BIGNUM *d = BN_new();
    if (!d || !BN_rshift(d, pMinus1, s)) {
        if (d) {
            BN_free(d);
        }
        BN_free(pMinus1);
        printf("Couldn't BIGNUM allocate in \"%s\".\n", "millerRabin");
        return -1;
    }

    // Check if p - 3 is superior to iterationMax
    unsigned int iterationMax = ITERATIONMAX;
    int nbBits = BN_num_bits(testeur->primeMinus3);
    if (nbBits < 4) {
        iterationMax = (unsigned int) BN_get_word(testeur->primeMinus3);
    }
    else if (nbBits == 4) {
        unsigned int tmp = (unsigned int) BN_get_word(testeur->primeMinus3);
        if (tmp < iterationMax) {
            iterationMax = tmp;
        }
    }

    BIGNUM *random;
    BIGNUM *a;
    BIGNUM *expvalue;
    node *page;
    node* tmp;
    int rvalue = 0;
    for (unsigned int i = 0; i < iterationMax; i++) {
        rvalue = 0;
        // Generate a random number and verify it wasn't already use
        if (getRandomForMR(testeur)) {
            BN_free(pMinus1);
            BN_free(d);
            printf("Couldn't getRandomForMR in \"%s\".\n", "millerRabin");
            return -1;
        }
        random = testeur->head->nb;
        page = testeur->head->next;
        while (page && BN_cmp(page->nb, random)) {
            page = page->next;
        }
        if (page) {
            tmp = testeur->head;
            testeur->head = testeur->head->next;
            BN_free(tmp->nb);
            free(tmp);
            i--;
            continue;
        }

        a = BN_dup(random);
        // Calculate random ** d mod n
        BIGNUM *exp = d;
        for (unsigned int r = 0; r < s; r++) {
            if (r == 1) {
                exp = testeur->two;
            }
            expvalue = BN_new();
            if (!expvalue || !BN_mod_exp(expvalue, a, exp, testeur->prime, testeur->ctx)) {
                if (expvalue) {
                    BN_free(expvalue);
                }
                BN_free(pMinus1);
                BN_free(a);
                BN_free(d);
                printf("Couldn't BIGNUM allocate in \"%s\".\n", "millerRabin");
                return -1;
            }
            BN_free(a);
            a = expvalue;
            if (BN_is_one(a) || !BN_cmp(pMinus1, a)) {
                rvalue = 1;
                break;
            }
        }
        BN_free(a);
    }
    
    BN_free(pMinus1);
    BN_free(d);
    clearLinkedList(testeur);
    return rvalue;
}

int isPrime(BIGNUM *number) {
    if (!BN_is_bit_set(number, 0)) {
        int r = BN_num_bits(number) == 2;
        BN_free(number);
        return r;
    }
    else if(BN_num_bits(number) <= 2) {
        int r = BN_num_bits(number) != 1;
        BN_free(number);
        return r;
    }
    PrimeTesteur *testeur = initPrimeTesteur();
    if (!testeur) {
        BN_free(number);
        printf("Couldn't initPrimeTesteur in \"%s\".\n", "isPrime");
        return -1;
    } 
    testeur->prime = number;
    if (preparePrimeTesteur(testeur)) {
        clearPrimeGenerated(testeur);
        printf("Couldn't prepare in \"%s\".\n", "isPrime");
        return -1;
    }
    int r = millerRabin(testeur);
    clearPrimeGenerated(testeur);
    return r;
}

BIGNUM *getNbWithBits(mt_state **state, int nbBits) {

    int k = nbBits / 8 + ((nbBits % 8) > 0);
    unsigned char *nb = calloc(sizeof(char), k);
    if (!nb) {
        printf("Couldn't allocate calloc in \"%s\".\n", "getNbWithBits");
        return NULL;
    }
    int decalage = k % 4;
    if (!decalage) {
        decalage = 4;
    }
    int stop = k / 4 + ((k % 4) > 0);
    uint32_t rand;
    for (int i = 0; i < stop - 1; i++) {
        rand = random_uint32(*state);
        nb[i * 4 + decalage]       = (rand >> 24) & 0xff;
        nb[(i * 4) + 1 + decalage] = (rand >> 16) & 0xff;
        nb[(i * 4) + 2 + decalage] = (rand >> 8) & 0xff;
        nb[(i * 4) + 3 + decalage] = rand & 0xff;
    }
    rand = random_uint32(*state);
    for (int i = 0; i < decalage; i++) {
        nb[decalage - 1 - i] = rand & 0xff;
        rand = rand >> 8;
    }
    decalage = nbBits % 8;
    if (!decalage) {
        decalage = 8;
    }
    nb[0] = (nb[0] & (0xff >> (8 - decalage))) | (1 << (decalage - 1));
    nb[k - 1] = nb[k - 1] | 1;

    BIGNUM *prime = BN_bin2bn(nb, k, NULL);
    if (!prime) {
        free(nb);
        printf("Couldn't allocate BN in \"%s\".\n", "getNbWithBits");
        return NULL;
    }
    free(nb);

    return prime;
}

int createPrime(int nbBits, int asHex) {
    if (nbBits < 2) {
        printf("There is not prime number with less than 2 bits\n");
        return -1;
    }
    PrimeTesteur *testeur = initPrimeTesteur();
    if (!testeur) {
        printf("Couldn't initPrimeTesteur in \"%s\".\n", "createPrime");
        return -1;
    }
    if (nbBits == 2) {
        printf("%d\n", 2 | (random_uint32(testeur->states) & 1));
        clearPrimeGenerated(testeur);
        return 1;
    }

    BIGNUM *prime = getNbWithBits(&(testeur->states), nbBits);
    if (!prime) {
        clearPrimeGenerated(testeur);
        printf("Couldn't getNbWithBits in \"%s\".\n", "createPrime");
        return -1;
    }
    testeur->prime = prime;
    if (preparePrimeTesteur(testeur)) {
        clearPrimeGenerated(testeur);
        printf("Couldn't preparePrimetesteur in \"%s\".\n", "isPrime");
        return -1;
    }

    int r = millerRabin(testeur);
    BIGNUM *primeMinus3;
    while (!r) {
        if (!BN_add(testeur->prime, testeur->prime, testeur->two)) {
            clearPrimeGenerated(testeur);
            printf("Couldn't BN Allocate in \"%s\".\n", "createPrime");
            return -1;
        }

        if (BN_is_bit_set(testeur->prime, nbBits)) {
            clearPrime(testeur);
            prime = getNbWithBits(&(testeur->states), nbBits);
            if (!prime) {
                clearPrimeGenerated(testeur);
                printf("Couldn't getNbWithBits in \"%s\".\n", "createPrime");
                return -1;
            }
            testeur->prime = prime;
            primeMinus3 = BN_new();
            if (!primeMinus3) {
                clearPrimeGenerated(testeur);
                printf("Couldn't BIGNUM allocate in \"%s\".\n", "createPrime");
                return -1;
            }
            testeur->primeMinus3 = primeMinus3;
            if (!BN_sub(primeMinus3, prime, testeur->three)) {
                clearPrimeGenerated(testeur);
                printf("Couldn't BIGNUM allocate in \"%s\".\n", "createPrime");
                return -1;
            }
        }
        else {
            if (!BN_add(testeur->primeMinus3, testeur->primeMinus3, testeur->two)) {
                clearPrimeGenerated(testeur);
                printf("Couldn't BN Allocate in \"%s\".\n", "createPrime");
                return -1;
            }
        }
        // printf("p   = %s\n", BN_bn2dec(testeur->prime));
        // printf("p-3 = %s\n", BN_bn2dec(testeur->primeMinus3));

        r = millerRabin(testeur);
        // printf("r = %d\n\n", r);
    }
    if (r == 1) {
        char *s;
        if (asHex) {
            s = BN_bn2hex(testeur->prime);
        }
        else {
            s = BN_bn2dec(testeur->prime);
        }
        if (s) {
            printf("%s\n", s);
            free(s);
        }
        else {
            printf("Couldn't BN Allocate in \"%s\".\n", "createPrime");
            r = -1;
        }
    }
    clearPrimeGenerated(testeur);
    return r;
}
