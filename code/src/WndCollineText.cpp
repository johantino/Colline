// WndCollineText.cpp: implementation of the WndCollineText class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Konst.h"
#include "Colline.h"
#include "WndCollineText.h"
#include <iostream>
#include "math.h"
//#include "StatisticsHandler.h"
//#include <conio.h> //for press key

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WndCollineText::WndCollineText(StatisticsHandler* sh, UINT idResource, int x0, int y0, int x1, int y1 )
{
	LoadFrame( idResource );
	
	backgroundRect = new CRect(0,0, x1-x0, y1-y0);
	CRect moveTo = new CRect(x0,y0,x1,y1); 
	MoveWindow(moveTo,true);
	ShowWindow( 1 );
	statHandler = sh; 
}

WndCollineText::~WndCollineText()
{

}

void WndCollineText::updateInfoAgents(int cycNum/*CStringArray* info*/) {
	/*if (info->GetSize() != 34) {
		std::cout << "WndCollineText error" << std::endl;
		pressSpaceOrQuit();
	}*/	
	CPaintDC dc(this);
	// draw a rectangle in the background (window) color
	//CRect backRect(0, 0, 600,400); //use winRect
	CBrush brBackground(::GetSysColor(COLOR_WINDOW));
	dc.FillRect(backgroundRect, &brBackground);

	//draw table lines
	POINT top1;
	top1.x = 5;
	top1.y = 47;
	POINT top2;
	top2.x = 500;
	top2.y = 47;
	dc.MoveTo(top1);
	dc.LineTo(top2);
	POINT bott1;
	bott1.x = 5;
	bott1.y = 132;
	POINT bott2;
	bott2.x = 500;
	bott2.y = 132;
	dc.MoveTo(bott1);
	dc.LineTo(bott2);
	POINT vertL1;
	vertL1.x = 100;
	vertL1.y = 35;
	POINT vertL2;
	vertL2.x = 100;
	vertL2.y = 150;
	dc.MoveTo(vertL1);
	dc.LineTo(vertL2);
	POINT vertR1;
	vertR1.x = 400;
	vertR1.y = 35;
	POINT vertR2;
	vertR2.x = 400;
	vertR2.y = 150;
	dc.MoveTo(vertR1);
	dc.LineTo(vertR2);
	//display labels
	dc.TextOut(10, 30,"Cycle:");
	int vPos = 30;
	int hPos = 115;
	dc.TextOut(hPos, vPos,"Foun");		hPos += 85;
	dc.TextOut(hPos, vPos,"Coll");		hPos += 85;
	dc.TextOut(hPos, vPos,"Inp");		hPos += 80;
	dc.TextOut(hPos, vPos,"Eff");		hPos += 70;
	dc.TextOut(hPos, vPos,"Total");		vPos += 20;

	hPos = 10;
	dc.TextOut(hPos, vPos,"Processors:");	vPos += 20;
	dc.TextOut(hPos, vPos,"Observers:");	vPos += 20;
	dc.TextOut(hPos, vPos,"Drif (Bus):");	vPos += 20;
	dc.TextOut(hPos, vPos,"Drif (Mat):");	vPos += 25;
	dc.TextOut(hPos, vPos,"Total:");		

	int i;
	hPos = 10;
	vPos = 220;
	for (i=0; i<NUM_OF_SPECIES; i++) {
		std::string text = "Species " + numToString(i);
		dc.TextOut(hPos, vPos, text.c_str());
		vPos += 20;
	}
	
	//display values
	dc.SetTextAlign(TA_RIGHT );
	dc.TextOut(90, 30, numToString( cycNum ).c_str());   //info->GetAt(0));
	vPos = 50;
	hPos = 145;
	dc.TextOut(hPos, vPos, numToString(statHandler->numOfProc_foun).c_str());		vPos += 20;
	dc.TextOut(hPos, vPos, numToString(statHandler->numOfObs_foun).c_str());		vPos += 20;
	dc.TextOut(hPos, vPos, numToString(statHandler->numOfDrif_foun).c_str());		vPos += 20;
	dc.TextOut(hPos, vPos, "-");				vPos += 25; // info->GetAt(4)
	dc.TextOut(hPos, vPos, numToString(statHandler->numOfAgents_foun).c_str());
	vPos = 50;
	hPos = 225;
	dc.TextOut(hPos, vPos, numToString(statHandler->numOfProc_coll).c_str());			vPos += 20;
	dc.TextOut(hPos, vPos, numToString(statHandler->numOfObs_coll).c_str());			vPos += 20;
	dc.TextOut(hPos, vPos, numToString(statHandler->numOfDrif_coll - statHandler->numOfMaters_coll).c_str());			vPos += 20;
	dc.TextOut(hPos, vPos, numToString(statHandler->numOfMaters_coll).c_str());			vPos += 25;
	dc.TextOut(hPos, vPos, numToString(statHandler->numOfAgents_coll).c_str());
	vPos = 50;
	hPos = 305;
	dc.TextOut(hPos, vPos, numToString(statHandler->numOfProc_inp).c_str());		vPos += 20;
	dc.TextOut(hPos, vPos, numToString(statHandler->numOfObs_inp).c_str());		vPos += 20;
	dc.TextOut(hPos, vPos, numToString(statHandler->numOfDrif_inp - statHandler->numOfMaters_inp).c_str());		vPos += 20;
	dc.TextOut(hPos, vPos, numToString(statHandler->numOfMaters_inp).c_str());		vPos += 25;
	dc.TextOut(hPos, vPos, numToString(statHandler->numOfAgents_inp).c_str());
	vPos = 50;
	hPos = 385;
	dc.TextOut(hPos, vPos, numToString(statHandler->numOfProc_eff).c_str());		vPos += 20;
	dc.TextOut(hPos, vPos, numToString(statHandler->numOfObs_eff).c_str());		vPos += 20;
	dc.TextOut(hPos, vPos, numToString(statHandler->numOfDrif_eff - statHandler->numOfMaters_eff).c_str());		vPos += 20;
	dc.TextOut(hPos, vPos, numToString(statHandler->numOfMaters_eff).c_str());		vPos += 25;
	dc.TextOut(hPos, vPos, numToString(statHandler->numOfAgents_eff).c_str());
	vPos = 50;
	hPos = 470;
	dc.TextOut(hPos, vPos, numToString(statHandler->numOfProcessors).c_str());		vPos += 20;
	dc.TextOut(hPos, vPos, numToString(statHandler->numOfObservers).c_str());		vPos += 20;
	dc.TextOut(hPos, vPos, numToString(statHandler->numOfDrifters - statHandler->numOfMaters).c_str());		vPos += 20;
	dc.TextOut(hPos, vPos, numToString(statHandler->numOfMaters).c_str());		vPos += 25;
	dc.TextOut(hPos, vPos, numToString(statHandler->numOfAgents).c_str());

	hPos = 225;
	vPos = 220;
	for (i=0; i<NUM_OF_SPECIES; i++) {
		dc.TextOut(hPos, vPos, numToString(statHandler->currentSpeciesCount->readValue(i,0)).c_str());
		vPos += 20;
	}
	hPos = 305;
	vPos = 220;
	for (i=0; i<NUM_OF_SPECIES; i++) {
		dc.TextOut(hPos, vPos, numToString(statHandler->currentSpeciesCount->readValue(i,1)).c_str());
		vPos += 20;
	}
	hPos = 385;
	vPos = 220;
	for (i=0; i<NUM_OF_SPECIES; i++) {
		dc.TextOut(hPos, vPos, numToString(statHandler->currentSpeciesCount->readValue(i,2)).c_str());
		vPos += 20;
	}

	hPos = 10;
	vPos = 165;
	dc.SetTextAlign(TA_LEFT );
	dc.TextOut(hPos, vPos, "Fitness(x1000):");	
	dc.SetTextAlign(TA_RIGHT );
	dc.TextOut(140, vPos, "-");
	hPos = 225;
	dc.TextOut(hPos, vPos, numToString(statHandler->fitnessColline_coll/1000).c_str());		hPos += 80;
	dc.TextOut(hPos, vPos, numToString(statHandler->fitnessColline_inp/1000).c_str());		hPos += 80;
	dc.TextOut(hPos, vPos, numToString(statHandler->fitnessColline_eff/1000).c_str());		hPos += 85;
	dc.TextOut(hPos, vPos, numToString(statHandler->fitnessColline/1000).c_str());
	hPos = 10;
	vPos = 185;
	dc.SetTextAlign(TA_LEFT );
	dc.TextOut(hPos, vPos, " - avg:");	
	dc.SetTextAlign(TA_RIGHT );
	dc.TextOut(140, vPos, "-");
	hPos = 225;
	dc.TextOut(hPos, vPos, numToString( getAverage(statHandler->fitnessColline_coll, statHandler->numOfAgents_coll) ).c_str());		hPos += 80;
	dc.TextOut(hPos, vPos, numToString( getAverage(statHandler->fitnessColline_inp, statHandler->numOfAgents_inp) ).c_str());		hPos += 80;
	dc.TextOut(hPos, vPos, numToString( getAverage(statHandler->fitnessColline_eff, statHandler->numOfAgents_eff)).c_str());		hPos += 85;
	dc.TextOut(hPos, vPos, numToString( getAverage(statHandler->fitnessColline, (statHandler->numOfAgents - statHandler->numOfAgents_foun)) ).c_str());

	//delete info;
	InvalidateRect(backgroundRect, false);
	//pressSpaceOrQuit();
}


void WndCollineText::updateInfoTraining( int sesNum/*CStringArray* info, UIntGrid* respInfo, UIntGrid* paidInfo, UIntGrid* correctResp, UIntGrid* wrongResp, UIntGrid* estimatedOkResp, UIntGrid* rewOnLevels*/) {
	CPaintDC dc(this);
	int col,row,hPos,vPos,level;
	char str[5];
	//int numOfCol = respInfo->getWidth();
	//int numOfRows = respInfo->getHight();
	//int strIndex = 0; //used to access 'info'

	// draw a rectangle in the background (window) color
	CBrush brBackground(::GetSysColor(COLOR_WINDOW));
	dc.FillRect(backgroundRect, &brBackground);

	//print info
	//hPos = 5;
	//vPos = 320;
	//dc.TextOut(hPos, vPos, "Now collecting data for current session...");

	//display session number
	hPos = 5;
	vPos = 5;
	dc.TextOut(hPos, vPos,"Session: ");
	hPos += 70;
	dc.TextOut(hPos,vPos, numToString( sesNum).c_str());
	hPos += 30;
	dc.TextOut(hPos, vPos, "Correct answer:");
	hPos += 105;
	dc.TextOut(hPos,vPos, numToString( statHandler->lastSesCorrectCategory).c_str());
	hPos += 30;
	dc.TextOut(hPos, vPos, "Colline answer:");
	hPos += 105;
	dc.TextOut(hPos,vPos, numToString( statHandler->lastSesCollineAnswer).c_str());
	hPos += 30;
	dc.TextOut(hPos, vPos, "pLevel:");
	hPos += 80;
	dc.TextOut(hPos,vPos, numToString( statHandler->lastSesChosenPickiLevel).c_str());
		
	//draw response table row labels:
	hPos = 5;
	vPos = 60;
	std::string rowHead;
	int numOfRows = statHandler->lastSesNumOfProcEff->getHight();
	for (row=0; row<numOfRows; row++) {
		rowHead = "Agr.lev ";
		_itoa( row, str, 10);
		rowHead += str;
		dc.TextOut(hPos, vPos, rowHead.c_str());
		vPos += 20;
	}
	vPos += 10;
	dc.TextOut(hPos, vPos, "Total");

	//draw response table values:
	std::string colHead;
	hPos = 5;
	vPos = 30;
	dc.SetTextAlign(TA_LEFT );
	dc.TextOut(hPos, vPos, "S.cycle:");
	dc.SetTextAlign(TA_RIGHT );
	hPos = 80;
	int numOfCol = statHandler->lastSesNumOfProcEff->getWidth();
	for (col=0; col<numOfCol; col++) {
		_itoa( (col + statHandler->COLL_MIN_PROCTIME ), str, 10);
		colHead = str;
		dc.TextOut(hPos, vPos, colHead.c_str());
		vPos += 30;
		for (row=0; row<numOfRows; row++) {
			dc.TextOut(hPos, vPos, statHandler->lastSesNumOfProcEff->toStringValue(row,col).c_str());
			vPos += 20;
		}
		vPos += 10; 
		dc.TextOut(hPos, vPos, statHandler->lastSesNumOfProcEff->toStringColumnSum(col).c_str());
		vPos = 30;
		hPos += 30;
	}
	//draw row sums:
	hPos += 10;
	vPos = 30;
	dc.TextOut(hPos,vPos,"Total");
	vPos += 30;
	for (row=0; row<numOfRows; row++) {
		dc.TextOut(hPos, vPos, statHandler->lastSesNumOfProcEff->toStringRowSum(row ).c_str());
		vPos += 20;
	}
	vPos += 10;
	dc.TextOut(hPos, vPos, statHandler->lastSesNumOfProcEff->toStringTotalSum().c_str());

	//draw Correct resp row-sum
	hPos += 40;
	vPos = 30;
	dc.TextOut(hPos,vPos,"Ok");
	vPos += 30;
	for (row=0; row<numOfRows; row++) {
		dc.TextOut(hPos, vPos, statHandler->lastSesCorrectResp->toStringRowSum(row).c_str());
		vPos += 20;
	}

	//draw wrong resp row-sum
	hPos += 30;
	vPos = 30;
	dc.TextOut(hPos,vPos,"!Ok");
	vPos += 30;
	for (row=0; row<numOfRows; row++) {
		dc.TextOut(hPos, vPos, statHandler->lastSesWrongResp->toStringRowSum(row).c_str());
		vPos += 20;
	}

	//draw paid fitness row-sum
	hPos += 30;
	vPos = 30;
	dc.TextOut(hPos,vPos,"Paid");
	vPos += 30;
	for (level=0; level<numOfRows; level++) { //(numOfRows = numOfLevels)
		dc.TextOut(hPos, vPos, statHandler->lastSesFitnessPaid->toStringRowSum1000(level).c_str()); //display paid fitness for level
		vPos += 20;
	}

	//draw estimated # of Ok's row sums
	hPos += 30;
	vPos = 30;
	dc.TextOut(hPos,vPos,"E.Ok");
	vPos += 30;
	for (level=0; level<numOfRows; level++) { //(numOfRows = numOfLevels)
		dc.TextOut(hPos, vPos, statHandler->lastSesNumOfCorr_EstForNext->toStringRowSum(level).c_str()); //
		vPos += 20;
	}

	//draw rewards on levels
	hPos += 45;
	vPos = 30;
	dc.TextOut(hPos,vPos,"Rew");
	vPos += 30;
	for (level=0; level<numOfRows; level++) { //(numOfRows = numOfLevels)
		dc.TextOut(hPos, vPos, statHandler->lastSesRewardOnLevels->toStringRowSum(level).c_str()); //display paid fitness for level
		vPos += 20;
	}

	//draw correct resp col-sum
	hPos = 5;
	vPos += 30;
	dc.SetTextAlign(TA_LEFT );
	dc.TextOut(hPos, vPos, "Ok:");
	dc.SetTextAlign(TA_RIGHT );
	hPos = 80;
	for (col=0; col<numOfCol; col++) {
		dc.TextOut(hPos, vPos, statHandler->lastSesCorrectResp->toStringColumnSum(col).c_str());
		hPos += 30;
	}
	hPos += 50;
	dc.TextOut(hPos, vPos, statHandler->lastSesCorrectResp->toStringTotalSum().c_str());

	//draw wrong resp col-sum
	hPos = 5;
	vPos += 20;
	dc.SetTextAlign(TA_LEFT );
	dc.TextOut(hPos, vPos, "!Ok:");
	dc.SetTextAlign(TA_RIGHT );
	hPos = 80;
	for (col=0; col<numOfCol; col++) {
		dc.TextOut(hPos, vPos, statHandler->lastSesWrongResp->toStringColumnSum(col).c_str());
		hPos += 30;
	}
	hPos += 80;
	dc.TextOut(hPos, vPos, statHandler->lastSesWrongResp->toStringTotalSum().c_str());
		
	//draw paid fitness col-sum
	hPos = 5;
	vPos += 20;
	dc.SetTextAlign(TA_LEFT );
	dc.TextOut(hPos, vPos, "Paid:");
	dc.SetTextAlign(TA_RIGHT );
	
	hPos = 80;
	for (col=0; col<numOfCol; col++) {
		dc.TextOut(hPos, vPos, statHandler->lastSesFitnessPaid->toStringColumnSum1000(col).c_str());
		hPos += 30;
	}
	hPos += 110;
	dc.TextOut(hPos, vPos, statHandler->lastSesFitnessPaid->toStringTotalSum1000().c_str());

	//display percent processed
	dc.SetTextAlign(TA_LEFT );
	hPos = 5;
	vPos += 30;
	dc.TextOut(hPos, vPos, "Bits from env:");
	hPos = 110;
	dc.TextOut(hPos, vPos, numToString(statHandler->lastSesBitsFromEnv).c_str());
	hPos = 5;
	vPos += 20;
	dc.TextOut(hPos, vPos, "Bits 'eaten' :");
	hPos = 110;
	dc.TextOut(hPos, vPos, numToString(statHandler->lastSesBitsEaten).c_str());
	hPos = 5;
	vPos += 20;
	dc.TextOut(hPos, vPos, "Bits disc.(P):");
	hPos = 110;
	dc.TextOut(hPos, vPos, numToString(statHandler->lastSesBitsDiscP).c_str());

	//fitnessBalance
	vPos -= 40;
	hPos = 150;
	dc.TextOut(hPos, vPos, "Fitn (env/sys):");
	hPos += 140;
	//dc.SetTextAlign(TA_RIGHT );
	std::string text = numToString(statHandler->getEnvFitnIndex()) + "/" + numToString(statHandler->getSysFitnIndex());
	dc.TextOut(hPos, vPos, text.c_str() );

	//lastSesFromEnvToBuff
	vPos += 20;
	hPos = 150;
	dc.TextOut(hPos, vPos, "From env to buff:"); 
	hPos += 140;
	dc.TextOut(hPos, vPos, numToString(statHandler->lastSesFromEnvToBuff ).c_str());

	//lastSesFitnessBuffer0
	vPos += 20;
	hPos = 150;
	dc.TextOut(hPos, vPos, "Fitn.buf_0:"); 
	hPos += 140;
	dc.TextOut(hPos, vPos, numToString(statHandler->lastSesFitnessBuffer0_cp ).c_str());

	//lastSesFitnessBuffer1
	vPos += 20;
	hPos = 150;
	dc.TextOut(hPos, vPos, "Fitn.buf_1:"); 
	hPos += 140;
	dc.TextOut(hPos, vPos, numToString(statHandler->lastSesFitnessBuffer1_cp ).c_str());

	//lastSesMatingBonusBuffer
	/*vPos += 20;
	hPos = 150;
	dc.TextOut(hPos, vPos, "M.Bonus buf:"); 
	hPos += 140;
	dc.TextOut(hPos, vPos, info->GetAt(strIndex)); strIndex++;*/

	//perf_cat0
	vPos -= 60;
	hPos = 355;
	dc.TextOut(hPos, vPos, "Perf.cat0:"); 
	hPos += 140;
	dc.TextOut(hPos, vPos, numToString(floor(statHandler->performance_cat0)).c_str());
	
	//perf_cat1
	vPos += 20;
	hPos = 355;
	dc.TextOut(hPos, vPos, "Perf.cat1:"); 
	hPos += 140;
	dc.TextOut(hPos, vPos, numToString(floor(statHandler->performance_cat1)).c_str());

	//delete info;
	InvalidateRect(backgroundRect, false);
}


std::string WndCollineText::numToString(int number) {
	char str[5];
	_itoa(number, str, 10);
	std::string info = str;
	return info;
}

int WndCollineText::getAverage(int fValue, int agents) {
	int avg;
	if (agents == 0)
		avg = 0;
	else
		avg =  fValue/agents;
	return avg;
}

void WndCollineText::updateInfoVarStat() {
	CPaintDC dc(this);
	CBrush brBackground(::GetSysColor(COLOR_WINDOW));
	dc.FillRect(backgroundRect, &brBackground);

	int hPos = 5;
	int vPos = 30;
	//int row,col;
	dc.SetTextAlign(TA_LEFT );
	dc.TextOut(hPos, vPos, "Species:");
	vPos += 50;
	dc.TextOut(hPos, vPos, "Col");
	vPos += 50;
	dc.TextOut(hPos, vPos, "Inp");
	vPos += 50;
	dc.TextOut(hPos, vPos, "Eff");
	dc.SetTextAlign(TA_RIGHT );
	hPos = 110;
	/*for (col=0; col<NUM_OF_SPECIES; col++) {
		dc.TextOut(hPos, vPos, numToString(col));
		vPos += 30;
		for (row=0; row<NUM_STAT_SPECIES; row++) {
			dc.TextOut(hPos, vPos, statHandler->speciesStat_col->toStringValue(row,col));
			vPos += 50;
			dc.TextOut(hPos, vPos, statHandler->speciesStat_inp->toStringValue(row,col));
			vPos += 50;
			dc.TextOut(hPos, vPos, statHandler->speciesStat_eff->toStringValue(row,col));
		}
		hPos += 30;
		vPos = 30;
	}*/
}
