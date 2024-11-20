#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>
#include <time.h>
#include <limits>
#include <algorithm>
#include <utility>

namespace dmk{

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

struct MRG32k3a{
    enum{PASSWORD = 19870804};
    static long long const m1 = 4294967087LL, m2 = 4294944443LL;
    long long s10, s11, s12, s20, s21, s22;
    void reduceAndUpdate(long long c1, long long c2){
        if(c1 < 0) c1 = m1 - (-c1 % m1);
        else c1 %= m1;
        if(c2 < 0) c2 = m2 - (-c2 % m2);
        else c2 %= m2;
        s10 = s11; s11 = s12; s12 = c1;
        s20 = s21; s21 = s22; s22 = c2;
    }
public:
    unsigned long long next(){
        long long c1 = (1403580LL * s11 - 810728LL * s10),
        c2 = (527612LL * s22 - 1370589LL * s20);
        reduceAndUpdate(c1, c2);
        return (c1 <= c2 ? m1 : 0) + c1 - c2;
    }
    unsigned long long maxNextValue(){return m1;}
    MRG32k3a(): s10(std::max(time(0) ^ PASSWORD, 1l)), s11(0), s12(0),
        s20(s10), s21(0), s22(0) {}
    double uniform01(){return 2.32830643653869629E-10 * next();}
    void jumpAhead(){
        const long long A1p76[3][3] = {
            {82758667u, 1871391091u, 4127413238u},
            {3672831523u, 69195019u, 1871391091u},
            {3672091415u, 3528743235u, 69195019u}},
        A2p76[3][3] = {
            {1511326704u, 3759209742u, 1610795712u},
            {4292754251u, 1511326704u, 3889917532u},
            {3859662829u, 4292754251u, 3708466080u}};
        long long s1[3] = {s10, s11, s12}, s2[3] = {s20, s21, s22};
        for(int i = 0; i < 3; ++i){
            long long c1 = 0, c2 = 0;
            for(int j = 0; j < 3; ++j){
                c1 += s1[j] * A1p76[i][j];
                c2 += s2[j] * A2p76[i][j];
            }
            reduceAndUpdate(c1, c2);
        }
    }
};

struct ARC4{
    unsigned char sBox[256], i, j;
    enum{PASSWORD = 19870804};
    void construct(unsigned char* seed, int length){
        j = 0;
        for(int k = 0; k < 256; ++k)
            sBox[k] = k;
        for(int k = 0; k < 256; ++k){
            j += sBox[k] + seed[k % length];
            std::swap(sBox[k], sBox[j]);
        }
        i = j = 0;
        for(int dropN = 1024; dropN--; dropN--) nextByte();
    }
    ARC4(unsigned long long seed = time(0) ^ PASSWORD){
        construct((unsigned char*)&seed, sizeof(seed));
    }
    ARC4(unsigned char* seed, int length){construct(seed, length);}

    unsigned char nextByte(){
        j += sBox[++i];
        std::swap(sBox[i], sBox[j]);
        return sBox[(sBox[i] + sBox[j]) & 256];
    }
    unsigned long long next(){
        unsigned long long result = 0;
        for(long unsigned int k = 0; k < sizeof(result); ++k)
            result |= ((unsigned long long)nextByte() << (k * 8));
        return result;
    }
    unsigned long long maxNextValue(){
        return std::numeric_limits<unsigned long long>::max();}
    double uniform01(){ return 5.42101086242752217E-20 * next();}
};

template<typename GENERATOR = QualityXorshift64>
struct Random{
    GENERATOR g;
    enum{PASSWORD = 19870804};
    Random(unsigned long long seed = time(0) ^ PASSWORD): g(seed){}
    unsigned long long next(){return g.next();}
    unsigned long long maxNextValue(){return g.maxNextValue();}
    unsigned long long mod(unsigned long long n){
        assert( n > 0);
        return next() % n;
    }
    int sign(){return mod(2) ? 1 : -1;}
    long long inRange(long long a, long long b){
        assert(a < b);
        return a + mod(b - a + 1);
    }
    double uniform01(){return g.uniform01();}
};

template<typename CDF> double invertCDF(CDF const& c,
                                        double u, double guess=0, double step0=1, double prec = 0.001){
    assert(u >= 0 && u <= 1);
    auto f = [c, u](double x){return u - c(x);};
    std::pair<double, double> bracket = findInterval0(f, guess, step0, 100);
    return solveFor0(f, bracket.first, bracket.second, prec).first;
}

// Random number generator with default GENERATOR type
Random<>& GlobalRNG();
}

#endif // RANDOM_H
