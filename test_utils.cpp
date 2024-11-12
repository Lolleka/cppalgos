#include "utils/utils.hpp"
#include "utils/vector.hpp"
#include "utils/debug.h"
#include <iostream>

using namespace dmk;

int main(){
    /* int* a = rawMemory<int>(8); */
    /* std::cout << a[4] << std::endl; */
    /* rawDestruct<int>(a, 8); */
    unsigned long long b = 0xABCDEF00112233ull;
    unsigned char c = 0;
    c = b;
    std::cout << std::hex <<  (unsigned long long)b << std::endl;
    std::cout << std::hex <<  (unsigned long long)c << std::endl;
    std::cout << sizeof(unsigned long long) << std::endl;
    std::cout << sizeof(unsigned char) << std::endl;

}
