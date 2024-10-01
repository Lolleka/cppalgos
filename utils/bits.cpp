#include "bits.hpp"

namespace useful{
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

    namespace bits{
        bool get(unsigned long long x, int i){ return x & twoPower(i);}
        bool flip(unsigned long long x, int i){ return x ^ twoPower(i);}

        unsigned long long upperMask(int n){return FULL<< n;}
        unsigned long long lowerMask(int n){return ~upperMask(n);}
        unsigned long long middleMask(int i, int n){return lowerMask(n)<<i;}
        unsigned long long getValue(unsigned long long x, int i, int n){
            return (x >>i) & lowerMask(n);
        }
    }

    int PopWordCount(unsigned long long x){
        static PopCount8 p8;
        int result = 0;
        for(; x; x >>= 8) result += p8(x);
        return result;
    }

    int rightmost0Count(unsigned long long x){return PopCount8(~x & (x - 1));}
}
