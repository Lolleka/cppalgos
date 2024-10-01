#include "utils.hpp"

long long ceiling(unsigned long long n, long long divisor){
    return n/divisor + bool(n % divisor);
}

