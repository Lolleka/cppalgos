#include "random.hpp"

namespace useful{
    uint32_t xorshiftTransform(uint32_t x){
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        return x;
    }

}
