#ifndef HASHING_H
#define HASHING_H

#include "bits.hpp"

namespace dmk{

template<typename HASHER>
class MHash {
    unsigned long long m;
    HASHER h;
public:
    MHash(unsigned long long theM): m(theM)
        {assert(theM > 0 && theM <= h.max());}
    typedef typename HASHER::WORD_TYPE WORD_TYPE;
    unsigned long long max() const{return m-1;}
    unsigned long long operator()(WORD_TYPE const& x)const{return h(x) % m;)}
    typedef typename HASHER::Builder Builder;
    Builder makeBuilder() const{return h.makeBuilder();}
    unsigned long long operator()(Builder b) const{return h(b) % m;}
};

template<typename HASHER>
class BHash{
    unsigned long long mask;
    HASHER h;
public:
    BHash(unsigned long long m): mask(m -1)
        {assert(m > 0 && isPowerOfTwo(m));}
    typedef typename HASHER::WORD_TYPE WORD_TYPE;
    unsigned long long max() const{return mask;}
    unsigned long long operator()(WORD_TYPE const& x)const{return h(x) & mask;)}
    typedef typename HASHER::Builder Builder;
    Builder makeBuilder() const{return h.makeBuilder();}
    unsigned long long operator()(Builder b) const{return h(b) & mask;}
};


}

#endif // !HASHING_H
