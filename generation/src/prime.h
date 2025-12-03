#ifndef DEF_PRIME_H
#define DEF_PRIME_H

#include "mt.h"

#include <malloc.h>
#include <openssl/bn.h>
#include <time.h>
#include <stdio.h>

#define ITERATIONMAX 9

typedef struct node
{
    BIGNUM *nb;
    struct node *next;
} node;

typedef struct
{
    mt_state *states;
    node *head;
    BIGNUM *prime;
    BIGNUM *primeMinus3;
    BIGNUM *two;
    BIGNUM *three;
    BIGNUM *twoPower32k;
    BN_CTX *ctx;
    int k;
} PrimeTesteur;

int isPrime(BIGNUM *number);

int createPrime(int nbBits, int asHex);

#endif