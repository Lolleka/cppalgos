#include "utils/utils.hpp"
#include "utils/vector.hpp"
#include "utils/debug.h"
#include <iostream>

using namespace useful;

int main(){
    int* a = rawMemory<int>(8);
    std::cout << a[4] << std::endl;
    rawDestruct<int>(a, 8);

    Vector<double> v;
    
}
