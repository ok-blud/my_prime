## Exemple output

### $ ./my_prime -g 256
104947343392682083168725584306604597653981817423698177403586078421105038662487

### $ ./my_prime -g 256 --hex
BB4A92C361E2A32BBC0ECC35037C4647A4D107E91548143F97D197C0D53D2C27

### $ time ./my_prime -g 2048
17637197874826882301137847420040...2841470165394441

real	0m3.704s
user	0m3.701s
sys	0m0.003s

### $ time ./my_prime -g 2048
24623285044306260724612565002603...4036940617979433519

real	0m1.053s
user	0m1.050s
sys	0m0.004s

### $ time ./my_prime -g 4096
96329804774808309050431141359...1466413295590787460861

real	0m16.304s
user	0m16.301s
sys	0m0.001s

### $ ./my_prime -t 94ab675d8d079fe539ab9d6b3b0f617fc63949c8082c54ce6a9973576b1904bf ; echo \$?
1

### $ ./my_prime -t 3acf ; echo \$?
0

### Thought on those result

The prime generation is slow, it depend a lot on how lucky you get.
Because it start with the generation of nbits number and if you have a prime close to you it will be fast like for the exemple above.
But if you are far away it can take moore than a few minutes.

The primality test is probabilistic and for big number it has a probabilty of 4^(-ITERATIOMMAX)
Which is set to 9 it prime.h. So for huge number around 1 in 262144 prime number generated are not prime.

The quality of the code isn't good, it's my first time using openssl/bn.h so a lot of thing aren't exactly great.

--hex should return an hexadecimal format without capital letter.

There is no documentation on what each function does, how a function work, who need to free what, ect...
