#include "catch.hpp"
#include "Grid.h"

TEST_CASE("Grid_Ctr_001") 
{
	Grid grid1(5, 5);
	REQUIRE(grid1.getWidthX() == 5);
	REQUIRE(grid1.getHightY() == 5);
}

TEST_CASE("getObservers_001")
{
	Grid grid1(3, 3);
	REQUIRE(grid1.getWidthX() == 3);
	REQUIRE(grid1.getHightY() == 3);

	GridCell cell1(1,1);
	Observable observable1;
	cell1.placeOccupier(&observable1);
	grid1.setCellAt(1, 1, &cell1);

	auto res1 = grid1.getObservers(&cell1, 1, false);
	REQUIRE(res1.size() == 0);
}
