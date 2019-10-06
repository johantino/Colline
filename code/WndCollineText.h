// WndCollineText.h: interface for the WndCollineText class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDCOLLINETEXT_H__15903949_27C4_458E_A245_DED1904A6DDF__INCLUDED_)
#define AFX_WNDCOLLINETEXT_H__15903949_27C4_458E_A245_DED1904A6DDF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIntGrid.h"
#include "StatisticsHandler.h"

class WndCollineText : public CFrameWnd  
{
public:
	WndCollineText(StatisticsHandler* sh, UINT idResource, int x0, int y0, int x1, int y1);
	virtual ~WndCollineText();
	void updateInfoAgents(int cycNum /*CStringArray* info*/);
	void updateInfoTraining(int sesNum/* CStringArray* info, UIntGrid* respInfo, UIntGrid* paidInfo, UIntGrid* correctResp, UIntGrid* wrongResp, UIntGrid* estimatedOkResp, UIntGrid* rewOnLevels*/);
	void updateInfoVarStat();

private:
	void pressSpaceOrQuit();
	CString numToString(int number);
	int getAverage(int fValue, int agents);
	//void drawLabels();
	StatisticsHandler* statHandler;
	CRect backgroundRect; //used when invalidating window

};

#endif // !defined(AFX_WNDCOLLINETEXT_H__15903949_27C4_458E_A245_DED1904A6DDF__INCLUDED_)
