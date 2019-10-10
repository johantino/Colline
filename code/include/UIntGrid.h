// UIntGrid.h: interface for the UIntGrid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UINTGRID_H__046E4690_2562_496E_9671_F9541666367B__INCLUDED_)
#define AFX_UINTGRID_H__046E4690_2562_496E_9671_F9541666367B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "UIntGridRow.h"

#include <vector>

//supports both 1d and 2d grids
class UIntGrid  
{
public:
	//UIntGrid(int w); //used for 1d grids
	UIntGrid(int h, int w); //used for 2d grids
	virtual ~UIntGrid();
	void setValue(unsigned int val, int row, int column);
	void addToValue(int change, int row, int column);
	//void increaseVal(int column); //add 1 to value in cell, row=1
	//void increaseVal(int row, int column); //add 1 to value in cell
	unsigned int readValue(int row, int column);
	void clear(); //sets all elements to 0
	std::string toString();
	int getHight();
	int getWidth();
	int getTotalSum();
	int getColumnSum(int col);
	int getRowSum(int row);
	std::string toStringValue(int row, int column);
	std::string toStringColumnSum(int column);
	std::string toStringRowSum(int row);
	std::string toStringTotalSum();
	std::string toStringColumnSum1000(int column);
	std::string toStringRowSum1000(int row);
	std::string toStringTotalSum1000();

	UIntGrid* clone();

private:
	std::string toStringRow(int row);
	int get2dIndex(int row, int column);
	void testPos(int row, int column);
	bool isOneDim; //true if the hight is equal to one
	int hight;
	int width;
	
	
	std::vector<unsigned int> gridArray;

};

#endif // !defined(AFX_UINTGRID_H__046E4690_2562_496E_9671_F9541666367B__INCLUDED_)
