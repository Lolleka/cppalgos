#ifndef DEBUG_H
#define DEBUG_H
#include <iostream>

namespace igmdk{

#define DEBUG(var) std::cout << #var " " << (var) << std::endl;

}
#endif // DEBUG_H
