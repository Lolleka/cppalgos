#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>
#include <time.h>
#include <limits>

namespace useful{
    uint32_t xorshiftTransform(uint32_t x);

    class QualityXorshift64{
        uint64_t state;
        enum{PASSWORD = 19870804};
    public:
        QualityXorshift64(uint64_t seed = time(0) ^ PASSWORD)
            {state = seed ? seed : PASSWORD;}
        static uint64_t transform(uint64_t x){
            x ^= x << 21;
            x ^= x >> 35;
            x ^= x << 4;
            return x * 2685821657736338717ull;
        } 
        uint64_t next(){return state = transform(state);}
        unsigned long long maxNextValue(){return std::numeric_limits<uint64_t>::max();}
        double uniform01(){return 5.42101086242752217E-20 * next();}
    };

    class MersenneTwister64 {
        enum{N=312, PASSWORD=19870804};
        uint64_t state[N];
        int i;
    public:
        MersenneTwister64(uint64_t seed = time(0) ^ PASSWORD){
            for(i = 1; i < N; ++i)
                state[i] = 6364136223846793005ULL * 
                    (state[i - 1] ^ (state[i -1] >> 62)) + 1;
            i = 0;
        }
        uint64_t next(){
            int j = (i + 1) % N, k = (i + 126) % N;
            uint64_t y = (state[i] & 0xFFFFFFFF80000000ULL) |
                (state[j] & 0x7FFFFFFUL);
            y = state[i] = 
                state[k] ^ (y>>1) ^ (y & 1 ? 0xB5026F5AA96619E9ULL : 0);
            i = j;
            y ^= (y >> 29) & 0x5555555555555555ULL;
            y ^= (y << 17) & 0x71D67FFFEDA60000ULL;
            y ^= (y << 37) & 0xFFF7EEE000000000ULL;
            y ^= (y >> 43);
            return y;
        }
        double uniform01(){return 5.42101086242752217E-20 * next();}
    };

}

#endif // RANDOM_H
