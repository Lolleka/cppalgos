#include "../../utils/vector.hpp"

#include <iostream>

int main (int argc, char *argv[]) {
    dmk::Vector<int> v(5,0);
    v[0] = 1;
    std::cout << v.getSize() << std::endl;
    return 0;
}
