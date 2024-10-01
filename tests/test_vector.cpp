#include <catch2/catch_test_macros.hpp>
#include "../utils/vector.hpp"

TEST_CASE( "vectors can be sized and resized", "[vector]" ) {
    // This setup will be done 4 times in total, once for each section
    useful::Vector<int> v(5, 0);

    REQUIRE( v.getSize() == 5 );
    REQUIRE( v.getCapacity() >= 5 );

    SECTION( "resizing bigger changes size and capacity" ) {
        v.resize();
        REQUIRE( v.getCapacity() >= v.getSize() );
    }
/*     SECTION( "resizing smaller changes size but not capacity" ) { */
/*         v.resize( 0 ); */

/*         REQUIRE( v.size() == 0 ); */
/*         REQUIRE( v.capacity() >= 5 ); */
/*     } */
/*     SECTION( "reserving bigger changes capacity but not size" ) { */
/*         v.reserve( 10 ); */

/*         REQUIRE( v.size() == 5 ); */
/*         REQUIRE( v.capacity() >= 10 ); */
/*     } */
/*     SECTION( "reserving smaller does not change size or capacity" ) { */
/*         v.reserve( 0 ); */

/*         REQUIRE( v.size() == 5 ); */
/*         REQUIRE( v.capacity() >= 5 ); */
/*     } */
}
