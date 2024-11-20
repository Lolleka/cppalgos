#include "../utils.hpp"
#include "../bits.hpp"
#include "../random.hpp"
#include "../sorting.hpp"

namespace dmk{
// ----- utils.hpp functions implementation -----
void rawDelete(void* array){
    ::operator delete(array);
}
// ----- bits.hpp functions implementation -----
unsigned long long twoPower(int x){return 1ull << x;}
bool isPowerOfTwo(unsigned long long x){return !(x & (x - 1));}

// lg = binary logarithm
int lgFloor(unsigned long long x){
    assert(x);
    int result = 0;
    while(x >>= 1) ++result;
    return result;
}
int lgCeiling(unsigned long long x){return lgFloor(x) + !isPowerOfTwo(x);}
unsigned long long nextPowerOfTwo(unsigned long long x){
    return isPowerOfTwo(x) ? x : twoPower(lgFloor(x) + 1);}

bool bits::get(unsigned long long x, int i){ return x & twoPower(i);}
bool bits::flip(unsigned long long x, int i){ return x ^ twoPower(i);}
unsigned long long bits::upperMask(int n){return FULL<< n;}
unsigned long long bits::lowerMask(int n){return ~upperMask(n);}
unsigned long long bits::middleMask(int i, int n){return lowerMask(n)<<i;}
unsigned long long bits::getValue(unsigned long long x, int i, int n){
    return (x >>i) & lowerMask(n);
}

int popCountWord(unsigned long long x){
    static PopCount8 p8;
    int result = 0;
    for(; x; x >>= 8) result += p8(x);
    return result;
}

int rightmost0Count(unsigned long long x){
    return popCountWord(~x & (x - 1));
}

// ----- utils.hpp functions implementation -----
uint32_t xorshiftTransform(uint32_t x){
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return x;
}

// Random number generator with default GENERATOR type
Random<>& GlobalRNG(){
    static Random<> r;
    return r;
}

// ----- sorting.hpp functions implementation -----

void countingSort(int* vector, int n, int N){
    Vector<int> counter(N, 0);
    for (int i = 0; i < n; ++i) ++counter[vector[i]]; // count
    for (int i = 0, index =0; i < N; ++i) // creeate in order
        while(counter[i]-- > 0) vector[index++] = i;
}

}
