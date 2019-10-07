// IdStamp.cpp: implementation of the IdStamp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Colline.h"
#include "IdStamp.h"
#include "iostream.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IdStamp::IdStamp(int initCounter)
{
	counter = initCounter;
	locked = false;
}

IdStamp::~IdStamp()
{

}

int IdStamp::getCounter() {
	return counter;
}

int IdStamp::stamp() {
	while (locked) {
		cout << (LPCTSTR)"IdStamp locked!" << endl; //to test if it ever happens
	}
	locked = true;
	counter++;
	locked = false; //still possible error, use CCriticalSection
	return counter;
}