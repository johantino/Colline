// WndColline.cpp: implementation of the WndColline class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Colline.h"
#include "WndColline.h"
//#include <conio.h> //for press key
#include <iostream>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WndColline::WndColline(StatisticsHandler* sh, int gWidth, int gHight)
{
	statHandler = sh;
	gridWidth = gWidth;
	gridHight = gHight;
	LPCTSTR title = "Colline view - Grid";
	Create(NULL, title, WS_OVERLAPPEDWINDOW, rectDefault, NULL,NULL, 0,NULL );
	winRect = new CRect(0,0,400,400);
	MoveWindow(winRect,true);
	ActivateFrame();

}

WndColline::~WndColline()
{

}


void WndColline::paintGrid( /*CUIntArray* gridView*/ /*CByteArray* gridView*/) {
	CPaintDC dc(this);
	//SetPixel(dc, 25,25, 0);
	int ZOOM = statHandler->GRID_ZOOM;
	int x,y, colorIndex;
	int startX, startY, addX, addY;
	UINT color;
	for (x=0; x<gridWidth; x++) {
		for (y=0; y<gridHight; y++) {
			colorIndex = y*gridHight + x;
			color = statHandler->nextViewFrame.at(colorIndex);
			startX = x*2*ZOOM; //ensures 'air' between agents
			startY = y*2*ZOOM;
			//paintGridCell(x, y, color);
			for (addX=0; addX<ZOOM; addX++) {
				for (addY=0; addY<ZOOM; addY++) {
					SetPixel(dc, startX+addX, startY+addY, color);
				}
			}
		}
	}
	
	InvalidateRect(winRect, false);
	ReleaseDC(&dc);
	//dc.DeleteDC();
	//pressSpaceOrQuit();
}

/*void WndColline::OnDraw( CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid ) {
	std::cout << "OnDraw called!!! " << std::endl;

}*/


int WndColline::getWidth() {
	return gridWidth;
}

int WndColline::getHight() {
	return gridHight;
}










