// GridCell.h: interface for the GridCell class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Observable.h"

#include <string>

class GridCell : public CObject  
{
public:
	GridCell();
	GridCell(int posX, int posY);
	virtual ~GridCell();
	bool isOccupied();
	std::string toString();
	void placeOccupier(Observable* occ);
	void freeCell(); //called when agent moves or dies
	int getPosX();
	int getPosY();
	int getDistance(GridCell* cell); //returns distance to cell 
	Observable* getOccupier(); //agent or fountain

private:
	bool occupied;
	Observable* occupier;
	int posX;
	int posY;
};
