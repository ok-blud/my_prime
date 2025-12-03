#include "mt.h"

#define _m 397
#define _w 32
#define _r 31
#define _UMASK (0xffffffffUL << _r)
#define _LMASK (0xffffffffUL >> (_w-_r))
#define _a 0x9908b0dfUL
#define _u 11
#define _s 7
#define _t 15
#define _l 18
#define _b 0x9d2c5680UL
#define _c 0xefc60000UL
#define _f 1812433253UL


void initialize_state(mt_state* state, uint32_t seed) 
{
    uint32_t* state_array = &(state->state_array[0]);
    
    state_array[0] = seed;                          // suggested initial seed = 19650218UL
    
    for (int i=1; i<_n; i++)
    {
        seed = _f * (seed ^ (seed >> (_w-2))) + i;    // Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier.
        state_array[i] = seed; 
    }
    
    state->state_index = 0;
}


uint32_t random_uint32(mt_state* state)
{
    uint32_t* state_array = &(state->state_array[0]);
    
    int k = state->state_index;      // point to current state location
                                     // 0 <= state_index <= n-1   always
    
//  int k = k - n;                   // point to state n iterations before
//  if (k < 0) k += n;               // modulo n circular indexing
                                     // the previous 2 lines actually do nothing
                                     //  for illustration only
    
    int j = k - (_n-1);               // point to state n-1 iterations before
    if (j < 0) j += _n;               // modulo n circular indexing

    uint32_t x = (state_array[k] & _UMASK) | (state_array[j] & _LMASK);
    
    uint32_t xA = x >> 1;
    if (x & 0x00000001UL) xA ^= _a;
    
    j = k - (_n-_m);                   // point to state n-m iterations before
    if (j < 0) j += _n;               // modulo n circular indexing
    
    x = state_array[j] ^ xA;         // compute next value in the state
    state_array[k++] = x;            // update new state value
    
    if (k >= _n) k = 0;               // modulo n circular indexing
    state->state_index = k;
    
    uint32_t y = x ^ (x >> _u);       // tempering 
             y = y ^ ((y << _s) & _b);
             y = y ^ ((y << _t) & _c);
    uint32_t z = y ^ (y >> _l);
    
    return z; 
}