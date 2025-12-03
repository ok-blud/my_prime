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

Prime generation is slow and depends heavily on how lucky you get. It starts by generating an nbits-sized number, and if there happens to be a prime nearby, the process is fast, as in the example above. But if the nearest prime is far away, it can take more than a few minutes.

The primality test is probabilistic, and for large numbers it has an error probability of 4^(-ITERATIONMAX), which is set to 9 in prime.h.
So for very large numbers, roughly 1 in 262,144 generated “prime” numbers will actually be composite.

The quality of the code isn’t great; it’s my first time using OpenSSL/bn.h, so a lot of things aren’t implemented as well as they could be.

--hex should return a lowercase hexadecimal representation.

There is no documentation on what each function does, how each function works, who is responsible for freeing memory, etc.
