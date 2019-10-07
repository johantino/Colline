// GridRow.cpp: implementation of the GridRow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Colline.h"
#include "GridRow.h"
#include "iostream.h"
#include <conio.h> //for press key

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GridRow::GridRow(int sz)
{
	size = sz;
	row.SetSize(sz,0);
}

GridRow::~GridRow()
{

}


void GridRow::setCellAt(int pos, GridCell* c) {
	checkPos(pos);
	row.SetAt(pos,c);
}

GridCell* GridRow::getCellAt(int pos) {
	checkPos(pos);
	GridCell* cell = row.ElementAt(pos);
	return cell;
}

void GridRow::checkPos(int pos) {
	if (pos >= size) {
		cout << "ERROR: row index error!" << endl;
		pressSpaceToQuit();
	}
}


void GridRow::pressSpaceToQuit() {
	int ch;
	cout << "press space..." << endl;
	while (ch != ' ') {
		ch = _getch();
	}
	exit(0);
}
