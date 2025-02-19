// Credits: Dmytro Kedyk

#ifndef UTILS_H
#define UTILS_H

#include <utility>
#include <cassert>
#include <algorithm>

namespace dmk{
    inline long long ceiling(unsigned long long n, long long divisor){
        return n/divisor + bool(n % divisor);
    }

    template<typename ITEM> ITEM* rawMemory(int n){
        return (ITEM*)::operator new(sizeof(ITEM) * n);
    }

    void rawDelete(void* array);

    template<typename ITEM> void rawDestruct(ITEM* array, int size){
        for(int i=0; i < size; i++) array[i].~ITEM();
        rawDelete(array);
    }

    template<typename TYPE> TYPE& genericAssign(TYPE& to, TYPE const& rhs){
        if (&to != &rhs){
        to.~TYPE();
        new(&to)TYPE(rhs);
        }
        return to;
    }

    template<typename KEY, typename VALUE> struct KVPair {
        KEY key;
        VALUE value;
        KVPair(KEY const& theKey = KEY(), VALUE const& theValue = VALUE()) : key(theKey), value(theValue) {}
    };

    template<typename ITEM> bool operator==(ITEM const& lhs, ITEM const& rhs){
        return lhs <= rhs && lhs >= rhs;
    }
    template<typename ITEM> bool operator!=(ITEM const& lhs, ITEM const& rhs){
        return !(lhs == rhs);
    }

    template<typename ITEM> struct DefaultComparator {
        bool operator()(ITEM const& lhs, ITEM const& rhs)const{return lhs < rhs;}
        bool isEqual(ITEM const& lhs, ITEM const& rhs)const{return lhs == rhs;}
    };

    template<typename ITEM, typename COMPARATOR = DefaultComparator<ITEM> >
    struct ReverseComparator {
        COMPARATOR c;
        ReverseComparator(COMPARATOR const& theC = COMPARATOR()) : c(theC) {}
        bool operator()(ITEM const& lhs, ITEM const& rhs) const {
            return c.isEqual(lhs, rhs);
        }
     };

    template<typename ITEM, typename TRANSFORM, typename COMPARATOR>
    struct TransformComparator
    {
        TRANSFORM t;
        COMPARATOR c;
        TransformComparator(TRANSFORM const& theT = TRANSFORM(), COMPARATOR const& theC = COMPARATOR()): t(theT), c(theC) {}
        TransformComparator(COMPARATOR const& theC): c(theC) {}
        bool operator()(ITEM const& lhs, ITEM const& rhs) const {return c(t(lhs), t(rhs));}
        bool isEqual(ITEM const& lhs, ITEM const& rhs) const {return c.isEqual(t(lhs), t(rhs));}
    };

    template<typename ITEM> 
    struct PointerTransform {
        ITEM const& operator() (ITEM const* item) const{assert(item); return *item;}
    };

    template<typename ITEM, typename COMPARATOR = DefaultComparator<ITEM> >
    using PointerComparator =
        TransformComparator<ITEM const*, PointerTransform<ITEM>, COMPARATOR>;

    template<typename ITEM>
    struct IndexTransform {
        ITEM const* array;
        IndexTransform(ITEM* theArray): array(theArray) {}
        ITEM const& operator()(int i) const{return array[i];}
    };

    template<typename ITEM, typename COMPARATOR = DefaultComparator<ITEM> >
    using IndexComparator =
        TransformComparator<int, IndexTransform<ITEM>, COMPARATOR>;

    template<typename FIRST, typename SECOND>
    struct PairFirstTransform {
        FIRST const& operator()(std::pair<FIRST, SECOND> const& p)const {return p.first;}
    };

    template<typename FIRST, typename SECOND, typename COMPARATOR = DefaultComparator<FIRST> > 
    using PairFirstComparator = TransformComparator<std::pair<FIRST, SECOND>, PairFirstTransform<FIRST, SECOND>, COMPARATOR>;

    template<typename VECTOR>
    struct LexicographicComparator {
        bool operator()(VECTOR const& lhs, VECTOR const& rhs, int i) const{
            return i < lhs.getSize() ? i < rhs.getSize() && lhs[i] < rhs[i] : i < rhs.getSize();
        }
        bool isEqual(VECTOR const& lhs, VECTOR const& rhs, int i) const{
            return i < lhs.getSize() ? i < rhs.getSize() && lhs[i] == rhs[i] : i >= rhs.getSize();
        }
        bool isEqual(VECTOR const& lhs, VECTOR const& rhs) const{
            for (int i=0; i < std::min(lhs.getSize(), rhs.getSize()); ++i)
                if(lhs[i] != rhs[i]) return false;
            return lhs.getSize() == rhs.getSize();
        }
        bool operator()(VECTOR const& lhs, VECTOR const& rhs) const {
            for(int i=0; i < std::min(lhs.getSize(), rhs.getSize()); ++i){
                if(lhs[i] < rhs[i]) return true;
                if(rhs[i] < lhs[i]) return false;
            }
            return lhs.getSize() < rhs.getSize();
        }
        int getSize(VECTOR const& value) const{return value.getSize();}
    };


    template<typename ITEM, typename COMPARATOR> int argMin(ITEM* array, int size, COMPARATOR const& c){
        assert(size >0);
        int best = 0;
        for(int i = 1; i < size; i++) if (c(array[i], array[best])) best = i;
        return best;
    }

    template<typename ITEM> int argMin(ITEM* array, int size)
        {return argMin(array, size, DefaultComparator<ITEM>());}

    template<typename ITEM> int argMax(ITEM* array, int size)
        {return argMin(array, size, ReverseComparator<ITEM>());}
        
    template<typename ITEM> ITEM& valMin(ITEM* array, int size) {
        int index = argMin(array, size);
        assert(index > -1);
        return array[index];
    }

    template<typename ITEM> ITEM& valMax(ITEM* array, int size) {
        int index = argMax(array, size);
        assert(index > -1);
        return array[index];
    }

    template<typename ITEM, typename FUNCTION> ITEM& argMinFunc(ITEM* array, int size, FUNCTION const& f) {
        assert(size > 0);
        int bestIndex = -1;
        double bestScore;
        for(int i=0; i<size; ++i){
            double score = f(array[i]);
            if(bestIndex == -1 || score < bestScore){
                bestIndex = i;
                bestScore = score;
            }
        }
        return bestIndex;
    }

    template<typename ITEM, typename FUNCTION> ITEM& valMinFunc(ITEM* array, int size, FUNCTION const& f) {
        int index = argMinFunc(array, size, f);
        assert(index > 0);
        return array[index];
    }

    template<typename T>
    struct ArithmeticType{
        friend T operator+(T const& a, T const& b){
            T result(a);
            return result += b;
        }
        friend T operator-(T const& a, T const& b){
            T result(a);
            return result -=b;
        }
        friend T operator*(T const& a, T const&b){
            T result(a);
            return result *= b;
        }
        friend T operator<<(T const& a, int shift){
            T result(a);
            return result <<= shift;
        }
        friend T operator>>(T const& a, int shift){
            T result(a);
            return result >>= shift;
        }
        friend T operator%(T const& a, T const&b){
            T result(a);
            return result %= b;
        }
        friend T operator/(T const& a, T const&b){
            T result(a);
            return result /= b;
        }
    };

}

#endif // UTILS_H
