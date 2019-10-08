// UIntGrid.cpp: implementation of the UIntGrid class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Colline.h"
#include "UIntGrid.h"
#include <iostream>
#include "Utilities.h"
#include "math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UIntGrid::UIntGrid(int h, int w)
{
	hight = h;
	width = w;
	if (hight == 1)
		isOneDim = true;
	else
		isOneDim = false;
	int arrayLength = hight*width;
	if (arrayLength == 0) {
		std::cout << "ERROR: UintGrid constructor" << std::endl;
		Utilities::pressSpaceToQuit();
	}

	gridArray.resize(arrayLength);
	clear();

}

UIntGrid* UIntGrid::clone() {
	UIntGrid* copy = new UIntGrid( hight, width);
	int r,c;
	for (c=0; c<width; c++) {
		for (r=0; r<hight; r++) {
			copy->setValue( readValue(r,c), r, c);
		}
	}
	return copy;
}

/*UIntGrid::UIntGrid(int w)
{
	hight = 1;
	width = w;
	isOneDim = true;
	int arrayLength = hight*width;
	if (arrayLength == 0) {
		std::cout << "ERROR: UintGrid constructor" << std::endl;
		while (true) {}
	}
	gridArray = new CUIntArray;
	gridArray->SetSize(arrayLength);
	//initialize gridArray:
	for (int i=0; i<arrayLength; i++)
		gridArray.at(i, 0);
}*/




UIntGrid::~UIntGrid()
{

}

std::string UIntGrid::toString() {
	std::string info;
	for (int r=0; r<hight; r++) {
		info += toStringRow(r);
	}	
	return info;
}

std::string UIntGrid::toStringRow(int row) {
	std::string info;
	char strVal[6];	
	int val;
	int index;
	info = "|";
	for (int i=0; i<width; i++) {
		index = get2dIndex(row, i);
		val = gridArray.at(index);
		_itoa(val, strVal, 10);
		if (val<100)
			info += " ";
		if (val<10)
			info += " ";
		info += strVal;
		info += "|";
	}
	info += "\n";
	int len = info.size();
	for (int i=0; i<len; i++) {
		info += "-";
	}
	info += "\n";
	return info;
}


int UIntGrid::getHight() {
	return hight;
}

int UIntGrid::getWidth() {
	return width;
}

void UIntGrid::clear() {
	int size = hight*width;
	for (int index=0; index<size; index++)
		gridArray.at(index) = 0;
}

int UIntGrid::getColumnSum(int col) {
	int sum=0;
	for (int r=0; r<hight; r++) {
		sum += readValue(r,col);
	}
	return sum;
}

int UIntGrid::getRowSum(int row) {
	int sum=0;
	for (int c=0; c<width; c++) {
		sum += readValue(row,c);
	}
	return sum;
}

int UIntGrid::getTotalSum() {
	int totalSum = 0;
	for (int row=0; row<hight; row++) {
		totalSum += getRowSum(row);
	}
	return totalSum;
}

std::string UIntGrid::toStringColumnSum(int column) {
	char str[5];
	int sum = getColumnSum(column);
	_itoa( sum, str, 10);
	std::string info = str;
	return info;
}

std::string UIntGrid::toStringColumnSum1000(int column) {
	char str[5];
	int sum = getColumnSum(column);
	int remain = fmod(sum,1000);
	sum = sum/1000;
	if (remain>=500)
		sum += 1;
	_itoa( sum, str, 10);
	std::string info = str;
	return info;
}


std::string UIntGrid::toStringRowSum(int row) {
	char str[5];
	int sum = getRowSum(row);
	_itoa( sum, str, 10);
	std::string info = str;
	return info;
}

std::string UIntGrid::toStringRowSum1000(int row) {
	char str[5];
	int sum = getRowSum(row);
	int remain = fmod(sum,1000);
	sum = sum/1000;
	if (remain>=500)
		sum += 1;
	_itoa( sum, str, 10);
	std::string info = str;
	return info;
}

std::string UIntGrid::toStringTotalSum() {
	char str[5];
	int sum = getTotalSum();
	_itoa( sum, str, 10);
	std::string info = str;
	return info;
}

std::string UIntGrid::toStringTotalSum1000() {
	char str[5];
	int sum = getTotalSum();
	int remain = fmod(sum,1000);
	sum = sum/1000;
	if (remain>=500)
		sum += 1;
	_itoa( sum, str, 10);
	std::string info = str;
	return info;
}


std::string UIntGrid::toStringValue(int row, int column) {
	char str[5];
	int value = readValue(row,column);
	_itoa( value, str, 10);
	std::string info = str;
	return info;
}

void UIntGrid::addToValue(int change, int row, int column) {
	testPos(row, column);
	int index = get2dIndex(row,column);
	int oldVal = gridArray.at(index);
	int newVal = oldVal + change;
	gridArray.at(index) = newVal;
}

/*void UIntGrid::increaseVal(int row, int column) {
	testPos(row,column);
	int index = get2dIndex(row, column);
	unsigned int val = gridArray.at(index);
	val++;
	gridArray.at(index, val);
}*/

void UIntGrid::setValue(unsigned int val, int row, int column) {
	//std::cout << "setvalue" << std::endl;
	testPos(row, column);
	//std::cout << "ok" << std::endl;
	int index = get2dIndex(row, column);
	gridArray.at(index) = val;
}

unsigned int UIntGrid::readValue(int row, int column) {
	//std::cout << "readvalku" << std::endl;
	testPos(row,column);
	//std::cout << "ok" << std::endl;
	int index = get2dIndex(row, column);
	unsigned int val = gridArray.at(index);
	return val;
}

int UIntGrid::get2dIndex(int row, int column) {
	int arrayIndex = row*width + column;
	return arrayIndex;
}

void UIntGrid::testPos(int row, int column) {
	if ((row<0) || (row > ( hight-1 ))) {
		std::cout << "ERROR: uintgrid, testPos error 1" << std::endl;
		std::cout << "hight = " << hight << std::endl;
		std::cout << "width = " << width << std::endl;
		std::cout << "tried to read row " << row <<", column " << column << std::endl;
		Utilities::pressSpaceToQuit();
	}
	if (column<0 || column > (width-1)) {
		std::cout << "ERROR: uintgrid, testPos error 2" << std::endl;
		std::cout << "hight = " << hight << std::endl;
		std::cout << "width = " << width << std::endl;
		std::cout << "tried to read row " << row <<", column " << column << std::endl;
		Utilities::pressSpaceToQuit();
	}
}
