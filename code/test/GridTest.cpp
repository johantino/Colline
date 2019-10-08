#include "catch.hpp"
#include "Grid.h"

TEST_CASE("Grid_Ctr_001") 
{
	Grid grid1(5, 5);
	REQUIRE(grid1.getWidthX() == 5);
	REQUIRE(grid1.getHightY() == 5);
}
