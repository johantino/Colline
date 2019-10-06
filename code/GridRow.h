// GridRow.h: interface for the GridRow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRIDROW_H__36D36351_3543_42D0_8E40_D9B10CD4C6D1__INCLUDED_)
#define AFX_GRIDROW_H__36D36351_3543_42D0_8E40_D9B10CD4C6D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GridCell.h"
#include <afxtempl.h>

class GridRow : public CObject  
{
public:
	GridRow(int sz);
	virtual ~GridRow();
	void setCellAt(int posX, GridCell*);
	GridCell* getCellAt(int posX);

private:
	void pressSpaceToQuit();
	void checkPos(int pos);
	CTypedPtrArray<CObArray, GridCell*> row;
	int size;
};

#endif // !defined(AFX_GRIDROW_H__36D36351_3543_42D0_8E40_D9B10CD4C6D1__INCLUDED_)
