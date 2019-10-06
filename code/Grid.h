// Grid.h: interface for the Grid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRID_H__5031B39F_870B_43AA_9F5A_8175899C46B3__INCLUDED_)
#define AFX_GRID_H__5031B39F_870B_43AA_9F5A_8175899C46B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <afxtempl.h>
#include "GridRow.h"
#include "GridCell.h"
//#include "Effector.h"
//#include "Agent.h"

class Grid  
{
public:
	Grid(int wX, int hY);
	virtual ~Grid();
	void setRowAt(int pos, GridRow*);
	void setCellAt(int posX, int posY, GridCell*);
	//void putBabyAgentAt(GridCell* freePosition);
	GridCell* getCellAt(int posX, int posY); //upper left corner is (0,0)
	CList<Observable*, Observable*>* getNeighbours(GridCell* center, int vicinity, bool returnCenterAgent);
	CList<Observable*, Observable*>* getObservers(GridCell* center, int vicinity, bool returnCenterAgent);
	int getNumOfNeighbours(GridCell* center, int vicinity, bool countCenterAgent);
	CList<Observable*, Observable*>* getCollectors(GridCell* center, int vicinity, bool returnCenterAgent); //used for DNA sampling
	CList<Observable*, Observable*>* getInpoders(GridCell* center, int vicinity, bool returnCenterAgent); //used for DNA sampling
	CList<Observable*, Observable*>* getEffectors(GridCell* center, int vicinity, bool returnCenterAgent); //used for DNA sampling
	CList<GridCell*, GridCell*>* getFreeCells(GridCell* center, int vicinity);
	int getWidthX();
	int getHightY();
	void pressSpaceToQuit();

private:
	GridRow* rowAt(int posY);
	void checkPos(int pos);
	CTypedPtrArray<CObArray, GridRow*> grid;
	int widthX;
	int hightY;
	
	
};

#endif // !defined(AFX_GRID_H__5031B39F_870B_43AA_9F5A_8175899C46B3__INCLUDED_)
