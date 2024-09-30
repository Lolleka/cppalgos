#include "utils/utils.h"
#include "utils/debug.h"
#include <iostream>

int main(){
    int* a = rawMemory<int>(8);
    std::cout << a[4] << std::endl;
    rawDestruct<int>(a, 8);
    
}
