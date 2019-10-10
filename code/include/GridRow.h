// GridRow.h: interface for the GridRow class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "GridCell.h"
#include <vector>


class GridRow : public CObject  
{
public:
	GridRow(int sz);
	virtual ~GridRow();
	void setCellAt(int posX, GridCell*);
	GridCell* getCellAt(int posX);

private:
	void checkPos(int pos);
	std::vector<GridCell*> row;
	int size;
};
