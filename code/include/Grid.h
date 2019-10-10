// Grid.h: interface for the Grid class.
//
//////////////////////////////////////////////////////////////////////

#pragma once


#include "GridRow.h"
#include "GridCell.h"
#include <list>
#include <vector>

class Grid  
{
public:
	Grid(int wX, int hY);
	virtual ~Grid();
	void setRowAt(int pos, GridRow*);
	void setCellAt(int posX, int posY, GridCell*);
	//void putBabyAgentAt(GridCell* freePosition);
	GridCell* getCellAt(int posX, int posY); //upper left corner is (0,0)
	std::list<Observable*> getNeighbours(GridCell* center, int vicinity, bool returnCenterAgent);
	std::list<Observable*> getObservers(GridCell* center, int vicinity, bool returnCenterAgent);
	int getNumOfNeighbours(GridCell* center, int vicinity, bool countCenterAgent);
	std::list<Observable*> getCollectors(GridCell* center, int vicinity, bool returnCenterAgent); //used for DNA sampling
	std::list<Observable*> getInpoders(GridCell* center, int vicinity, bool returnCenterAgent); //used for DNA sampling
	std::list<Observable*> getEffectors(GridCell* center, int vicinity, bool returnCenterAgent); //used for DNA sampling
	std::list<GridCell*> getFreeCells(GridCell* center, int vicinity);
	int getWidthX();
	int getHightY();

private:
	GridRow* rowAt(int posY);
	void checkPos(int pos);
	std::vector<GridRow*> grid;
	int widthX;
	int hightY;
};
