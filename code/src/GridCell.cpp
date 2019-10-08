// GridCell.cpp: implementation of the GridCell class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Colline.h"
#include "GridCell.h"
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

GridCell::GridCell() {
	posX = -1;
	posY = -1; //unknown position (not important)
	occupied = false;
}

GridCell::GridCell(int pX, int pY)
{
	posX = pX;
	posY = pY;
	occupied = false;
}

GridCell::~GridCell()
{

}

void GridCell::freeCell() {
	occupied = false;
	occupier = NULL;
}

bool GridCell::isOccupied() {
	return occupied;
}

void GridCell::placeOccupier(Observable* occ) {
	if (isOccupied()) {
		std::cout << "ERROR: GridCell, attempt to place two agents in cell!" << std::endl;
		pressSpaceToQuit();
	}
	occupied = true;
	occupier = occ;
}

int GridCell::getDistance(GridCell* cell) {
	int distance = 0;
	distance = max( abs(posX - cell->getPosX()), abs(posY - cell->getPosY()));
	return distance;
}

std::string GridCell::toString() {
	std::string tPos;
	char tX[3];
	char tY[3];
	_itoa(posX,tX,10);
	_itoa(posY,tY,10);
	tPos = "(";
	tPos += tX;
	tPos += ",";
	tPos += tY;
	tPos += ")";
	return tPos;
}

int GridCell::getPosX() {
	return posX;
}

int GridCell::getPosY() {
	return posY;
}

Observable* GridCell::getOccupier() {
	return occupier;
}

void GridCell::pressSpaceToQuit() {
	int ch;
	std::cout << "press space..." << std::endl;
	while (ch != ' ') {
		ch = _getch();
	}
	exit(0);
}