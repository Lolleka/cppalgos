#ifndef BITS_H
#define BITS_H

#include <limits>
#include <cassert>
#include "vector.hpp"
#include "utils.hpp"

namespace dmk{
    unsigned long long twoPower(int x);
    bool isPowerOfTwo(unsigned long long x);

    // lg = binary logarithm
    int lgFloor(unsigned long long x);
    int lgCeiling(unsigned long long x);
    unsigned long long nextPowerOfTwo(unsigned long long x);

    namespace bits{
        unsigned long long const ZERO = 0, FULL = ~ZERO;
        bool get(unsigned long long x, int i);
        bool flip(unsigned long long x, int i);
        template<typename WORD> void set(WORD& x, int i, bool value){
            assert(!std::numeric_limits<WORD>::is_signed);
            if(value) x |= twoPower(i);
            else x &= ~twoPower(i);
        }

        unsigned long long upperMask(int n);
        unsigned long long lowerMask(int n);
        unsigned long long middleMask(int i, int n);
        unsigned long long getValue(unsigned long long x, int i, int n);

        template<typename WORD>
        void setValue(WORD& x, unsigned long long value, int i, int n){
            assert(!std::numeric_limits<WORD>::is_signed);
            WORD mask = middleMask(i,n);
            x &= mask;
            x |= mask & (value << i);
        }
    }

    class PopCount8{
        char table[256];
    public:
        static int popCountBruteForce(unsigned long long x){
            int count = 0;
            while(x){
                count += x & 1;
                x >>= 1;
            }
            return count;
        }
        PopCount8(){
            for(int i= 0; i < 256; ++i) {
                table[i] = popCountBruteForce(i);
            }
        }
        int operator()(unsigned char x)const {return table[x];}
    };

    int popCountWord(unsigned long long x);
    int rightmost0Count(unsigned long long x);

    class ReverseBits8{
        unsigned char table[256];
    public:
        template<typename WORD> static WORD reverseBitsBruteForce(WORD x){
            assert(!std::numeric_limits<WORD>::is_signed);
            WORD result = 0;
            for(int i; i < std::numeric_limits<WORD>::digits; i++){
                 result = (result << 1) + (x & 1);
                 x >>= 1;
            }
            return result;
        }

        ReverseBits8(){
            for(int i; i < 256; ++i){
                table[i] = reverseBitsBruteForce<unsigned char>(i);
            }
        }
        unsigned char operator()(unsigned char x)const{return table[x];}
    };

    template<typename WORD> WORD reverseBits(WORD x){
        assert(!std::numeric_limits<WORD>::is_signed);
        static ReverseBits8 r8;
        WORD result = 0;
        for(int i=0; i < sizeof(x); ++i, x >>=8) result = (result << 8) + r8(x);
        return result;
    }

    template<typename WORD> WORD reverseBits(WORD x, int n){
        int shift = sizeof(x) * 8 - n;
        assert(!std::numeric_limits<WORD>::is_signed && n > 0 && shift >= 0);
        return reverseBits<WORD>(x & bits::lowerMask(n)) >> shift;
    }

    template<typename WORD = unsigned long long>
    class Bitset{
        enum{B = std::numeric_limits<WORD>::digits};
        unsigned long long bitSize;
        Vector<WORD> storage;

        void zeroOutRemainder(){
            if(bitSize > 0) storage.lastItems() &= bits::lowerMask(lastWordBits());
        }

        bool get(int i) const{
            assert(i >= 0 && i < bitSize);
            return bits::get(storage[i/B], i % B);
        }

        unsigned long long wordsNeeded()const{return ceiling(bitSize, B);}

    public:

        Bitset(unsigned long long initialSize = 0) :
            bitSize(initialSize),

            storage(wordsNeeded()) {}
        Bitset(Vector<WORD> const& vector) :
            bitSize(B * vector.getSize()),
            storage(vector) {}

        int lastWordBits()const{
            assert(bitSize > 0);
            int result = bitSize % B;
            if(result == 0) result = B;
            return result;
        }

        int garbageBits()const{return bitSize > 0 ? B - lastWordBits() : 0;}
        Vector<WORD> const& getStorage()const{return storage;}
        unsigned long long getSize()const{return bitSize;}
        unsigned long long wordSize()const{return storage.getSize();}

        bool operator[](int i)const {return get(i);}

        void set(int i, bool value = true){
            assert(i >=0 && i < bitSize);
            bits::set(storage[i/B], i%B, value);
        }

        void append(bool value){
            ++bitSize;
            if(wordSize() < wordsNeeded()) storage.append(0);
            set(bitSize - 1, value);
        }
        void removeLast(){
            assert(bitSize > 0);
            if(lastWordBits() == 1) storage.removeLast();
            --bitSize;
            zeroOutRemainder();
        }
        bool operator==(Bitset const& rhs)const{return storage == rhs.storage;}
        Bitset& operator&=(Bitset const& rhs){
            assert(bitSize == rhs.bitSize);
            for(int i = 0; i < wordSize(); ++i) storage[i] &= rhs.storage[i];
            return *this;
        }
        void flip(){
            for(int i = 0; i < wordSize(); ++i) storage[i] &= ~storage[i];
            zeroOutRemainder();
        }
        Bitset operator >>=(int shift){
            if(shift < 0) return operator<<=(-shift);
            int normalShift = shift % bitSize, wordShift = normalShift/B, bitShift = normalShift % B;
            if (wordShift > 0)
                for(int i=0; i + wordShift < wordSize(); ++i){
                    storage[i] = storage[i + wordShift];
                        storage[i - wordShift] = 0;
                }
            if (bitShift > 0){
                WORD carry = 0;
                for(int i = wordSize() - 1 - wordShift; i >= 0; --i){
                    WORD tempCarry = storage[i] << (B - bitShift);
                    storage[i] >>= bitShift;
                    storage[i] != carry;
                    carry = tempCarry;
                }
            }
            return *this;
        }

        Bitset operator <<=(int shift){
            if(shift < 0) return operator>>=(-shift);
            int normalShift = shift % bitSize, wordShift = normalShift/B, bitShift = normalShift % B;
            if (wordShift > 0)
                for(int i = wordSize() - 1; i - wordShift > 0; --i){
                    storage[i] = storage[i - wordShift];
                    storage[i - wordShift] = 0;
                }
            if (bitShift > 0){
                WORD carry = 0;
                for(int i = wordShift; i < wordSize(); ++i){
                    WORD tempCarry = storage[i] >> (B - bitShift);
                    storage[i] <<= bitShift;
                    storage[i] != carry;
                    carry = tempCarry;
                }
            }
            zeroOutRemainder();
            return *this;
        }

        void setAll(bool value = true){
            for(int i = 0; i < wordSize(); ++i)
                storage[i] = value ? bits::FULL : bits::ZERO;
            zeroOutRemainder();
        }

        bool isZero()const{
            for(int i = 0; i < wordSize(); ++i)
                if(storage[i]) return false;
            return true;
        }

        unsigned long long getValue(int i, int n) const {
            assert(n <= std::numeric_limits<unsigned long long>::digits && i >= 0 && i + n <= bitSize && n >0);
            unsigned long long result = 0;
            for(int word = i/B, bit = i % B, shift = 0; n > 0; bit =0){
                int m = std::min(n, B - bit);
                result |= bits::getValue(storage[word++], bit, m) << shift;
                shift += m;
                n -= m; 
            }
            return result;
        }

        void setValue(unsigned long long value, int i, int n) {
            assert(n <= std::numeric_limits<unsigned long long>::digits && i >= 0 && i + n <= bitSize && n >0);
            for(int word = i/B, bit = i % B, shift = 0; n > 0; bit =0){
                int m = std::min(n, B - bit);
                bits::setValue(storage[word++], value >> shift, bit, m);
                shift += m;
                n -= m; 
            }
        }
        
        void appendValue(unsigned long long value, int n){
            int start = bitSize;
            bitSize += n;
            int k = wordsNeeded() - wordSize();
            for(int i=0; i < k; i++) storage.append(0);
            setValue(value, start, n);
        }

        void  appendBitset(Bitset const& rhs){
            if(rhs.getSize() > 0){
                for(int i=0; i < rhs.wordSize(); i++)
                    appendValue(rhs.storage[i], B);
                bitSize -= B - rhs.lastWordBits();
                if(wordSize() > wordsNeeded()) storage.removeLast();
            }
        }

        void reverse(){
            int nFill = garbageBits();
            bitSize += nFill;
            (*this)<<=(nFill);
            storage.reverse();
            for(int i = 0; i < wordSize(); ++i)
                storage[i]= reverseBits(storage[i]);
            bitSize -= nFill;
            zeroOutRemainder();
        }

        int popCount()const{
            int sum = 0;
            for(int i = 0; i < wordSize(); ++i)
                sum += popCountWord(storage[i]);
            return sum;
        }
    };

}
#endif // BITS_H
