// IdStamp.h: interface for the IdStamp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IDSTAMP_H__45EF2846_EBCC_4E8A_87FA_462910EA3706__INCLUDED_)
#define AFX_IDSTAMP_H__45EF2846_EBCC_4E8A_87FA_462910EA3706__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class IdStamp
{
public:
	IdStamp(int initCounter);
	virtual ~IdStamp();
	int stamp(); //returns current counter value and increase value by 1
	int getCounter(); //returns current counter value
private:
	int counter;
	bool locked; //semaphor
};

#endif // !defined(AFX_IDSTAMP_H__45EF2846_EBCC_4E8A_87FA_462910EA3706__INCLUDED_)
