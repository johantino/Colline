// GridCell.h: interface for the GridCell class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRIDCELL_H__4F44EB84_0974_4BAE_AC22_DDFAEED9D49B__INCLUDED_)
#define AFX_GRIDCELL_H__4F44EB84_0974_4BAE_AC22_DDFAEED9D49B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Observable.h"

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

#endif // !defined(AFX_GRIDCELL_H__4F44EB84_0974_4BAE_AC22_DDFAEED9D49B__INCLUDED_)
