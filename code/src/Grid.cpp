// Grid.cpp: implementation of the Grid class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Colline.h"
#include "Grid.h"
#include "Konst.h"
#include <iostream>
#include <conio.h> //for press key

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Grid::Grid(int gridHight, int rowWidth)
{
	grid.SetSize(gridHight,0);
	widthX = rowWidth;
	hightY = gridHight;
	if (widthX != hightY) {
		std::cout << "ERROR: current imp doesnt support non-square grid" << std::endl;
		pressSpaceToQuit();
	}
	GridRow* tempRow;
	GridCell* tc;
	for (int y=0; y<hightY; y++) {
		tempRow = new GridRow( widthX );
		for (int x=0; x<widthX; x++) {
			tc = new GridCell(x,y);
			tempRow->setCellAt(x,tc);
		}
		setRowAt(y, tempRow);
	}

}

Grid::~Grid()
{
	/*std::cout << "info: deleting all agents in grid..." << std::endl;
	CList<Observable*, Observable*>* allAgents = getNeighbours(getCellAt(widthX/2 - 1, hightY/2 - 1), widthX/2 - 1);
	int numOfAg = allAgents->GetCount();
	Observable* curr;
	for (int i=0; i<numOfAg; i++) {
		curr = allAgents->GetAt( allAgents->FindIndex(i) );
		delete curr;
	}
	delete allAgents;
	//delete grid;
	*/
}

void Grid::setRowAt(int pos, GridRow* row) {
	grid.SetAt(pos, row);
}

GridCell* Grid::getCellAt(int posX, int posY) {
	checkPos(posY);
	GridRow* row = rowAt(posY);
	GridCell* cell = row->getCellAt(posX);
	return cell;
}

void Grid::setCellAt(int posX, int posY, GridCell* cell) {
	GridRow* row = rowAt(posY);
	row->setCellAt(posX, cell);
}

void Grid::checkPos(int posY) {
	if (posY >= hightY) {
		std::cout << "ERROR: row index error!" << std::endl;
		pressSpaceToQuit();
	}
}

GridRow* Grid::rowAt(int posY) {
	GridRow* row = grid.ElementAt(posY);
	return row;
}

int Grid::getWidthX() {
	return widthX;
}

int Grid::getHightY() {
	return hightY;
}

//like getNeighbours, but limits return to collectors
CList<Observable*, Observable*>* Grid::getCollectors(GridCell* center, int vicinity, bool returnCenterAgent) {
	CList<Observable*, Observable*>* nbCollectors = new CList<Observable*, Observable*>(20);
	auto neighbours = getNeighbours(center, vicinity, returnCenterAgent);

	for (auto agent : neighbours) {
		if (agent->isCollector())
			nbCollectors->AddTail( agent );
	}

	return nbCollectors;
}

//like getNeighbours, but limits return to inpoders
CList<Observable*, Observable*>* Grid::getInpoders(GridCell* center, int vicinity, bool returnCenterAgent) {
	CList<Observable*, Observable*>* nbInpoders = new CList<Observable*, Observable*>(20);
	auto neighbours = getNeighbours(center, vicinity, returnCenterAgent);
	
	for (auto agent : neighbours) {
		if (agent->isInpoder())
			nbInpoders->AddTail( agent );
	}

	return nbInpoders;
}


//like getNeighbours, but limits return to effectors
CList<Observable*, Observable*>* Grid::getEffectors(GridCell* center, int vicinity, bool returnCenterAgent) {
	CList<Observable*, Observable*>* nbEffectors = new CList<Observable*, Observable*>(20);
	auto neighbours = getNeighbours(center, vicinity, returnCenterAgent);

	for(auto agent : neighbours) {
		if (agent->isEffector())
			nbEffectors->AddTail( agent );
	}

	return nbEffectors;
}

std::list<Observable*> Grid::getNeighbours(GridCell* center, int vicinity, bool returnCenterAgent) {
	int minX, minY, maxX, maxY;
	GridCell* tempCell;
	auto neighbours = std::list<Observable*>(10);
	int centerX = center->getPosX();
	int centerY = center->getPosY();
	minX = centerX - vicinity;
	minY = centerY - vicinity;
	maxX = centerX + vicinity;
	maxY = centerY + vicinity;
	if (minX < 0)
		minX = 0;
	if (minY < 0)
		minY = 0;
	if (maxX > (widthX - 1))
		maxX = widthX - 1;
	if (maxY > (hightY - 1))
		maxY = hightY - 1;
	for (int x=minX; x<=maxX; x++) {
		for (int y=minY; y<=maxY; y++) {
			if (x!=centerX || y!=centerY || returnCenterAgent) { //if retCenterAgent is false: don't return agent in center (the one calling); only when both x and y are center, skip
				tempCell = getCellAt(x,y);
				//std::cout << "testing cell at: " << tempCell->toString() << std::endl;
				if (tempCell->isOccupied())
						neighbours.emplace_back(tempCell->getOccupier()); //neighbours.push_back(tempCell->getOccupier());
			}
		}
	}
	return neighbours;
}


//returns observers in neighbourhood, minus fountains
CList<Observable*, Observable*>* Grid::getObservers(GridCell* center, int vicinity, bool returnCenterAgent) {
	CList<Observable*, Observable*>* observersInNB = new CList<Observable*, Observable*>(10);
	int minX, minY, maxX, maxY;
	GridCell* tempCell;
	Observable* tempAgent;
	int centerX = center->getPosX();
	int centerY = center->getPosY();
	minX = centerX - vicinity;
	minY = centerY - vicinity;
	maxX = centerX + vicinity;
	maxY = centerY + vicinity;
	if (minX < 0)
		minX = 0;
	if (minY < 0)
		minY = 0;
	if (maxX > (widthX - 1))
		maxX = widthX - 1;
	if (maxY > (hightY - 1))
		maxY = hightY - 1;
	for (int x=minX; x<=maxX; x++) {
		for (int y=minY; y<=maxY; y++) {
			if (x!=centerX || y!=centerY || returnCenterAgent) { //only skip when both x and y are center and retCenAgent=true (calling agent is in center)
				tempCell = getCellAt(x,y);
				if (tempCell->isOccupied()) {
					tempAgent = tempCell->getOccupier();
					if ((tempAgent->getStatusAndActive() == ST_OBSERVER) && (tempAgent->getType() != TYPE_FOUNTAIN))
						observersInNB->AddTail(tempAgent);
				}
					
			}
		}
	}
	//std::cout << "info: num of obs in nb: " << observersInNB->GetCount() << " , vicinity=" << vicinity << std::endl;
	return observersInNB;
}


int Grid::getNumOfNeighbours(GridCell* center, int vicinity, bool countCenterAgent) {
	int minX, minY, maxX, maxY;
	GridCell* tempCell;
	int numOfNeighbours=0;
	int centerX = center->getPosX();
	int centerY = center->getPosY();
	minX = centerX - vicinity;
	minY = centerY - vicinity;
	maxX = centerX + vicinity;
	maxY = centerY + vicinity;
	if (minX < 0)
		minX = 0;
	if (minY < 0)
		minY = 0;
	if (maxX > (widthX - 1))
		maxX = widthX - 1;
	if (maxY > (hightY - 1))
		maxY = hightY - 1;
	for (int x=minX; x<=maxX; x++) {
		for (int y=minY; y<=maxY; y++) {
			if (x!=centerX || y!=centerY || countCenterAgent) { //if retCenterAgent is false: don't return agent in center (the one calling); only when both x and y are center, skip
				tempCell = getCellAt(x,y);
				//std::cout << "testing cell at: " << tempCell->toString() << std::endl;
				if (tempCell->isOccupied())
						numOfNeighbours++;
			}
		}
	}
	return numOfNeighbours;
}



CList<GridCell*, GridCell*>* Grid::getFreeCells(GridCell* center, int vicinity) {
	CList<GridCell*, GridCell*>* freeCells = new CList<GridCell*, GridCell*>(10);
	int minX, minY, maxX, maxY;
	GridCell* tempCell;
	//vector<GridCell*> freeCells;
	int centerX = center->getPosX();
	int centerY = center->getPosY();
	minX = centerX - vicinity;
	minY = centerY - vicinity;
	maxX = centerX + vicinity;
	maxY = centerY + vicinity;
	if (minX < 0)
		minX = 0;
	if (minY < 0)
		minY = 0;
	if (maxX > (widthX - 1))
		maxX = widthX - 1;
	if (maxY > (hightY - 1))
		maxY = hightY - 1;
	//std::cout << "getFreeCells: minX = " << minX << ",  minY = " << minY << std::endl;
	//std::cout << "maxX = " << maxX << ",  maxY = " << maxY << std::endl;
	//std::cout << "centerX = " << centerX << ",   centerY=" << centerY << std::endl;
	for (int x=minX; x<=maxX; x++) {
		for (int y=minY; y<=maxY; y++) {
			if (x!=centerX || y!=centerY) { //only when both x and y are center, skip
				tempCell = getCellAt(x,y);
				if (!(tempCell->isOccupied())) 
					freeCells->AddTail(tempCell);
			}
		}
	}
	return freeCells;
}

void Grid::pressSpaceToQuit() {
	int ch;
	std::cout << "press space..." << std::endl;
	while (ch != ' ') {
		ch = _getch();
	}
	exit(0);
}

/*void Grid::putBabyAgentAt(GridCell* freePosition) {
*/


