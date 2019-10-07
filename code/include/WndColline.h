// WndColline.h: interface for the WndColline class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDCOLLINE_H__1421D79B_9142_40CF_A18C_ED56F6D0D677__INCLUDED_)
#define AFX_WNDCOLLINE_H__1421D79B_9142_40CF_A18C_ED56F6D0D677__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StatisticsHandler.h"

class WndColline : public CFrameWnd  
{
public:
	WndColline(StatisticsHandler* sh, int gWidth, int gHight);
	virtual ~WndColline();
	void paintGrid(/*CUIntArray* gridView*/ /*CByteArray* gridView*/);
	//void OnDraw( CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid );
	int getWidth();
	int getHight();
	
private:
	void pressSpaceOrQuit();
	StatisticsHandler* statHandler;
	
	int gridWidth;
	int gridHight;
	CRect winRect; //used when invalidating window


};

#endif // !defined(AFX_WNDCOLLINE_H__1421D79B_9142_40CF_A18C_ED56F6D0D677__INCLUDED_)
