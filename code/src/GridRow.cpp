// GridRow.cpp: implementation of the GridRow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Colline.h"
#include "GridRow.h"
#include <iostream>
#include "Utilities.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GridRow::GridRow(int sz) :
	row(sz, nullptr)
{
	size = sz;
}

GridRow::~GridRow()
{
}

void GridRow::setCellAt(int pos, GridCell* c) {
	checkPos(pos);
	row.at(pos) = c;
}

GridCell* GridRow::getCellAt(int pos) {
	checkPos(pos);
	GridCell* cell = row.at(pos);
	return cell;
}

void GridRow::checkPos(int pos) {
	if (pos >= size) {
		std::cout << "ERROR: row index error!" << std::endl;
		Utilities::pressSpaceToQuit();
	}
}
