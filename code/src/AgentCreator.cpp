// AgentCreator.cpp: implementation of the AgentCreator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Colline.h"
#include "AgentCreator.h"
#include "Konst.h"
#include <iostream>
#include <conio.h> //for press key
#include "Fountain.h"
#include "dCollector.h"
#include "Inpoder.h"
#include "Effector.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgentCreator::AgentCreator(Environment* e, StatisticsHandler* sh, SyncManager* sm, Grid* aGrid, IdStamp* iStamp, DecisionCategory* dc)
{
	env = e;
	statHandler = sh;
	syncMan = sm;
	agentGrid = aGrid;
	stamp = iStamp;
	decCat = dc;
}

AgentCreator::~AgentCreator()
{

}

void AgentCreator::restorePopulation( CString fname_agentpop ) {
	Message* appMat;
	Message* appBusSmall;
	Message* dnaAgPart;
	Message* dnaTypePart;
	//Message* obsPart;
	int numOfEntries, type, posX, posY, loadingSlot, appMat_val, appBusSmall_val, dna_ag_val, dna_type_val;
	int initFitn, currFitn, age, matStat, lastPrSess;
	bool matStatBool;
	FILE* agentPopFile;
	if ((agentPopFile = fopen(fname_agentpop, "r")) == NULL) {
		std::cout << "ERROR: couldn't open restore population, file not found" << std::endl;
		std::cout << (LPCTSTR)fname_agentpop << std::endl;
		pressSpaceToQuit();
	} else
		std::cout << "Restoring population..." << std::endl;
	fseek( agentPopFile, 0, SEEK_SET);
	fscanf(agentPopFile,"%d", &numOfEntries); //first value in file gives num of agents
	std::cout << "num of entries: " << numOfEntries << std::endl;
	
	for (int i=0; i<numOfEntries; i++) {
		fscanf(agentPopFile,"%d", &type);
		//std::cout << type  << " ";
		if (type == TYPE_FOUNTAIN) {
			fscanf(agentPopFile,"%d", &appBusSmall_val);
			fscanf(agentPopFile,"%d", &posX);
			fscanf(agentPopFile,"%d", &posY);
			fscanf(agentPopFile,"%d", &loadingSlot);
			appBusSmall = new Message(appBusSmall_val, APPEAR_SIZE_BUS_SMALL);
			//std::cout << posX << " ";
			//std::cout << posY << " ";
			//std::cout << loadingSlot << " " << std::endl;
			new Fountain(env, statHandler, stamp, agentGrid, agentGrid->getCellAt(posX,posY), syncMan, appBusSmall, loadingSlot);
			//delete obsPart;
			//read fountain values
		} else if (type == TYPE_COLLECTOR || type == TYPE_INPODER || type == TYPE_EFFECTOR) {
			fscanf(agentPopFile,"%d", &appMat_val);
			fscanf(agentPopFile,"%d", &appBusSmall_val);
			fscanf(agentPopFile,"%d", &dna_ag_val);
			fscanf(agentPopFile,"%d", &dna_type_val);
			fscanf(agentPopFile,"%d", &initFitn);
			fscanf(agentPopFile,"%d", &currFitn);
			fscanf(agentPopFile,"%d", &posX);
			fscanf(agentPopFile,"%d", &posY);
			fscanf(agentPopFile,"%d", &age);
			fscanf(agentPopFile,"%d", &matStat);
			if (matStat == 0)
				matStatBool = false;
			else if (matStat == 1)
				matStatBool = true;
			else {
				std::cout << "Error: rest pop. mating state" << std::endl;
				pressSpaceToQuit();
			}
			fscanf(agentPopFile,"%d", &lastPrSess);
			appMat = new Message(appMat_val, APPEAR_SIZE_MAT);
			appBusSmall = new Message(appBusSmall_val, APPEAR_SIZE_BUS_SMALL);
			dnaAgPart = new Message(dna_ag_val, N_DNA_AG);
			//Message* dnaTypePart;
			/*std::cout << dna_obs_val << " ";
			std::cout << dna_ag_val << " ";
			std::cout << dna_type_val << " ";
			std::cout << initFitn << " ";
			std::cout << currFitn << " ";
			std::cout << posX << " ";
			std::cout << posY << std::endl;*/
			if (type == TYPE_COLLECTOR) {
				dnaTypePart = new Message(dna_type_val, N_DNA_COLL);
				new Collector(env, statHandler, stamp, agentGrid, agentGrid->getCellAt(posX,posY), syncMan, initFitn, currFitn, age, matStatBool, lastPrSess, appMat, appBusSmall, dnaAgPart, dnaTypePart);
			} else if (type == TYPE_INPODER) {
				dnaTypePart = new Message(dna_type_val, N_DNA_INP);
				new Inpoder(env, statHandler,stamp, agentGrid, agentGrid->getCellAt(posX,posY), syncMan, initFitn, currFitn, age, matStatBool, lastPrSess, appMat, appBusSmall, dnaAgPart, dnaTypePart);
			} else {
				dnaTypePart = new Message(dna_type_val, N_DNA_EFF);
				new Effector(env, statHandler, stamp, agentGrid, agentGrid->getCellAt(posX,posY), syncMan, initFitn, currFitn, age, matStatBool, lastPrSess, appMat, appBusSmall, dnaAgPart, dnaTypePart, decCat);
			}
			//delete dnaObsPart;
			//delete dnaAgPart;
			//delete dnaTypePart;
		} else {
			std::cout << "ERROR: reading file in rest.pop." << std::endl;
			pressSpaceToQuit();
		}
	}
	fclose(agentPopFile);
}

void AgentCreator::createFountains() {
	GridCell* f_CT_Pos; //center
	GridCell* f_UR_Pos; //upper left fountain in 5-group
	GridCell* f_UL_Pos;
	GridCell* f_LL_Pos;
	GridCell* f_LR_Pos;
		
	Message* f_CT_appear;
	Message* f_UR_appear;
	Message* f_UL_appear;
	Message* f_LL_appear;
	Message* f_LR_appear;
	int f_CT_timeSlot, f_UR_timeSlot, f_UL_timeSlot, f_LL_timeSlot, f_LR_timeSlot;
	int CY;
	int CX = 25;
	int xPlace, yPlace;
	for (xPlace=0; xPlace<5; xPlace++) {
		CY=25;
		for (yPlace=0; yPlace<5; yPlace++) {
			f_CT_Pos = agentGrid->getCellAt(CX,CY);
			f_UR_Pos = agentGrid->getCellAt(CX+2,CY-2);
			f_UL_Pos = agentGrid->getCellAt(CX-2,CY-2);
			f_LL_Pos = agentGrid->getCellAt(CX-2,CY+2);
			f_LR_Pos = agentGrid->getCellAt(CX+2,CY+2);
			f_CT_timeSlot = 0; //+ 2*xPlace;
			f_UR_timeSlot = 1; //+ 2*xPlace;
			f_UL_timeSlot = 2; //+ 2*xPlace;
			f_LL_timeSlot = 3; //+ 2*xPlace;
			f_LR_timeSlot = 4; //+ 2*xPlace;
			f_CT_appear = new Message(0, APPEAR_SIZE_BUS_SMALL);
			f_UR_appear = new Message(15, APPEAR_SIZE_BUS_SMALL);
			f_UL_appear = new Message(12, APPEAR_SIZE_BUS_SMALL);
			f_LL_appear = new Message(3, APPEAR_SIZE_BUS_SMALL);
			f_LR_appear = new Message(6, APPEAR_SIZE_BUS_SMALL);

			new Fountain(env, statHandler, stamp, agentGrid, f_CT_Pos, syncMan, f_CT_appear, f_CT_timeSlot);
			new Fountain(env, statHandler, stamp, agentGrid, f_UR_Pos, syncMan, f_UR_appear, f_UR_timeSlot);
			new Fountain(env, statHandler, stamp, agentGrid, f_UL_Pos, syncMan, f_UL_appear, f_UL_timeSlot);
			new Fountain(env, statHandler, stamp, agentGrid, f_LL_Pos, syncMan, f_LL_appear, f_LL_timeSlot);
			new Fountain(env, statHandler, stamp, agentGrid, f_LR_Pos, syncMan, f_LR_appear, f_LR_timeSlot);
			CY += 8;
		}
		CX+=8;
	}
	/*OLD (6 f-groups middle process first)
		if (xPlace==2 || xPlace==3) {
			f_center_timeSlot = 0;
			f_UR_timeSlot = 1;
			f_UL_timeSlot = 2;
			f_LL_timeSlot = 1;
			f_LR_timeSlot = 2;
			f_center_appear = new Message(0, APPEAR_SIZE_BUS_SMALL);
			f_UR_appear = new Message(1, APPEAR_SIZE_BUS_SMALL);
			f_UL_appear = new Message(2, APPEAR_SIZE_BUS_SMALL);
			f_LL_appear = new Message(3, APPEAR_SIZE_BUS_SMALL);
			f_LR_appear = new Message(4, APPEAR_SIZE_BUS_SMALL);
		} else if (xPlace==1 || xPlace==4) {
			f_center_timeSlot = 3;
			f_UR_timeSlot = 4;
			f_UL_timeSlot = 5;
			f_LL_timeSlot = 4;
			f_LR_timeSlot = 5;
			f_center_appear = new Message(5, APPEAR_SIZE_BUS_SMALL);
			f_UR_appear = new Message(6, APPEAR_SIZE_BUS_SMALL);
			f_UL_appear = new Message(7, APPEAR_SIZE_BUS_SMALL);
			f_LL_appear = new Message(8, APPEAR_SIZE_BUS_SMALL);
			f_LR_appear = new Message(9, APPEAR_SIZE_BUS_SMALL);
		} else if (xPlace==0 || xPlace==5) {
			f_center_timeSlot = 6;
			f_UR_timeSlot = 7;
			f_UL_timeSlot = 8;
			f_LL_timeSlot = 7;
			f_LR_timeSlot = 8;
			f_center_appear = new Message(10, APPEAR_SIZE_BUS_SMALL);
			f_UR_appear = new Message(11, APPEAR_SIZE_BUS_SMALL);
			f_UL_appear = new Message(12, APPEAR_SIZE_BUS_SMALL);
			f_LL_appear = new Message(13, APPEAR_SIZE_BUS_SMALL);
			f_LR_appear = new Message(14, APPEAR_SIZE_BUS_SMALL);
		}
	*/

	/* OLD code (places 8 fountain groups of five in a circle, group appearance rotate with the circle)
	GridCell* f_center_Pos;
	GridCell* f_UR_Pos; //upper left fountain in 5-group
	GridCell* f_UL_Pos;
	GridCell* f_LL_Pos;
	GridCell* f_LR_Pos;
		
	Message* f_center_appear;
	Message* f_UR_appear;
	Message* f_UL_appear;
	Message* f_LL_appear;
	Message* f_LR_appear;

	int f0_timeSlot, f1_timeSlot, f2_timeSlot, f3_timeSlot, f4_timeSlot, founApp_add;
	int founApp_UR_val = 999;
	int founApp_UL_val = 999;
	int founApp_LL_val = 999;
	int founApp_LR_val = 999;
	
	int CX=27;
	int CY;
	int xPlace,yPlace;
	for (xPlace=0; xPlace<3; xPlace++) {
		CY=27;
		for (yPlace=0; yPlace<3; yPlace++) {
			if (xPlace !=1 || yPlace!=1) {
				
			f_center_Pos = agentGrid->getCellAt(CX,CY);
			f_UR_Pos = agentGrid->getCellAt(CX+2,CY-2);
			f_UL_Pos = agentGrid->getCellAt(CX-2,CY-2);
			f_LL_Pos = agentGrid->getCellAt(CX-2,CY+2);
			f_LR_Pos = agentGrid->getCellAt(CX+2,CY+2);
			if ((xPlace+yPlace)%2==1) { //'+' positions
				f0_timeSlot = 6-3; 
				f1_timeSlot = 8-3;
				f2_timeSlot = 7-3;
				f3_timeSlot = 8-3;
				f4_timeSlot = 7-3;
				//make sure that val=1 and val=2 points towards center:
				if (xPlace==1 && yPlace==2) { //south
					founApp_UR_val = 1;
					founApp_UL_val = 2;
					founApp_LL_val = 3;
					founApp_LR_val = 4;
				} else if (xPlace==1 && yPlace==0) {//north
					founApp_UR_val = 3;
					founApp_UL_val = 4;
					founApp_LL_val = 1;
					founApp_LR_val = 2;
				} else if (xPlace==0 && yPlace==1) {//west
					founApp_UR_val = 2;
					founApp_UL_val = 3;
					founApp_LL_val = 4;
					founApp_LR_val = 1;
				} else if (xPlace==2 && yPlace==1) {//east
					founApp_UR_val = 4;
					founApp_UL_val = 1;
					founApp_LL_val = 2;
					founApp_LR_val = 3;
				} else {
					std::cout << "ERROR: agentCreator rotate fountain groups" << std::endl;
					pressSpaceToQuit();
				} //end point to center (purpose: agents can learn to propragate (grow) towards center)
				founApp_add = 11;
			} else if (xPlace==1 && yPlace==1) {//center position
				f0_timeSlot = 0; 
				f1_timeSlot = 2;
				f2_timeSlot = 1;
				f3_timeSlot = 2;
				f4_timeSlot = 1;
				founApp_add = 0;
			} else {//'X' positions
				f0_timeSlot = 3-3; 
				f1_timeSlot = 5-3;
				f2_timeSlot = 4-3;
				f3_timeSlot = 5-3;
				f4_timeSlot = 4-3;
				//make sure that val=1 points towards center:
				if (xPlace==0 && yPlace==2) { //lower-right corner (LL)
					founApp_UR_val = 1;
					founApp_UL_val = 2;
					founApp_LL_val = 3;
					founApp_LR_val = 4;
				} else if (xPlace==2 && yPlace==2) {//LR
					founApp_UR_val = 4;
					founApp_UL_val = 1;
					founApp_LL_val = 2;
					founApp_LR_val = 3;
				} else if (xPlace==2 && yPlace==0) {//UR
					founApp_UR_val = 3;
					founApp_UL_val = 4;
					founApp_LL_val = 1;
					founApp_LR_val = 2;
				} else if (xPlace==0 && yPlace==0) {//UL
					founApp_UR_val = 2;
					founApp_UL_val = 3;
					founApp_LL_val = 4;
					founApp_LR_val = 1;
				} else {
					std::cout << "ERROR: agentCreator rotate fountain groups" << std::endl;
					pressSpaceToQuit();
				} //end point to center (purpose: agents can learn to propragate (grow) towards center)

				founApp_add = 5;
			}
			f_center_appear = new Message(0+founApp_add,APPEAR_SIZE_BUS_SMALL);
			f_UR_appear = new Message(founApp_UR_val + founApp_add,APPEAR_SIZE_BUS_SMALL);
			f_UL_appear = new Message(founApp_UL_val + founApp_add,APPEAR_SIZE_BUS_SMALL);
			f_LL_appear = new Message(founApp_LL_val + founApp_add,APPEAR_SIZE_BUS_SMALL);
			f_LR_appear = new Message(founApp_LR_val + founApp_add,APPEAR_SIZE_BUS_SMALL);
			new Fountain(env, statHandler, stamp, agentGrid, f_center_Pos, syncMan, f_center_appear, f0_timeSlot);
			new Fountain(env, statHandler, stamp, agentGrid, f_UR_Pos, syncMan, f_UR_appear, f1_timeSlot);
			new Fountain(env, statHandler, stamp, agentGrid, f_UL_Pos, syncMan, f_UL_appear, f2_timeSlot);
			new Fountain(env, statHandler, stamp, agentGrid, f_LL_Pos, syncMan, f_LL_appear, f3_timeSlot);
			new Fountain(env, statHandler, stamp, agentGrid, f_LR_Pos, syncMan, f_LR_appear, f4_timeSlot);
			}//end if
			CY += 23;
		}
		CX += 23;
	}*/
}

void AgentCreator::createCollectors() {
	GridCell* center = agentGrid->getCellAt(50,50); 
	int scanRadius = 49;
	int valAppMat;
	int valAppBus;
	int valAg;
	int valType;
	int mlChange;
	int initFitness;
	Message* appMat;
	Message* appBusSmall;
	Message* DNA_ag;
	Message* DNA_type_c;
	GridCell* position;
	int loopCheck = 0;
	CList<GridCell*, GridCell*>* freeCells;
	freeCells = agentGrid->getFreeCells( center, scanRadius);
	position = freeCells->GetAt( freeCells->FindIndex( getRandNumBetwZeroAnd( freeCells->GetCount()-1 ) ));
	Collector* coll;
	int num = 0;
	for (; num < INIT_NUM_OF_COLL; num++) {
		//freeCells = myGrid->getFreeCells( myGrid->getCellAt(35,35), 30);
		valAppMat = getRandNumBetwZeroAnd( pow(2,APPEAR_SIZE_MAT)-1 ); 
		valAppBus = getRandNumBetwZeroAnd( pow(2,APPEAR_SIZE_BUS_SMALL)-1 );
		valAg =     getRandNumBetwZeroAnd( pow(2,N_DNA_AG)-1 );
		valType =   getRandNumBetwZeroAnd( pow(2,N_DNA_COLL)-1 );
		initFitness = INIT_FITN_COL_BASE + getRandNumBetwZeroAnd( INIT_FITN_COL_RAND );
		appMat = new Message(valAppMat, APPEAR_SIZE_MAT);
		appBusSmall = new Message(valAppBus, APPEAR_SIZE_BUS_SMALL);
		DNA_ag = new Message(valAg, N_DNA_AG);
		DNA_type_c = new Message(valType, N_DNA_COLL);
		while (position->isOccupied() && loopCheck<10000) {
			position = freeCells->GetAt( freeCells->FindIndex( getRandNumBetwZeroAnd( freeCells->GetCount()-1 ) ));
			loopCheck++;
		}			
		coll = new Collector(env, statHandler, stamp, agentGrid, position, syncMan, initFitness, initFitness, 0, false, syncMan->getSessionNum(), appMat, appBusSmall, DNA_ag, DNA_type_c);
		mlChange = getRandNumBetwZeroAnd( statHandler->AGENT_LIFETIME_MAX/2 );
		mlChange -= statHandler->AGENT_LIFETIME_MAX/4;
		coll->TESTscatterMaxLifeTime( mlChange );
	}
	std::cout << num << " collectors created..." << std::endl;
	delete freeCells;
}

void AgentCreator::createInpoders() {
	GridCell* center = agentGrid->getCellAt(50,50); 
	int scanRadius = 49;
	int valAppMat;
	int valAppBus;
	int valAg;
	int valType;
	int mlChange;
	int initFitness;
	Message* appMat;
	Message* appBusSmall;
	Message* DNA_ag;
	Message* DNA_type_i;
	GridCell* position;
	int loopCheck = 0;
	CList<GridCell*, GridCell*>* freeCells;
	freeCells = agentGrid->getFreeCells( center, scanRadius );
	position = freeCells->GetAt( freeCells->FindIndex( getRandNumBetwZeroAnd( freeCells->GetCount()-1) ));
	Inpoder* inp;
	int num = 0;
	for (; num < INIT_NUM_OF_INP; num++) {
		//freeCells = myGrid->getFreeCells( myGrid->getCellAt(35,35), 30);
		valAppMat = getRandNumBetwZeroAnd( pow(2,APPEAR_SIZE_MAT)-1 );
		valAppBus = getRandNumBetwZeroAnd( pow(2,APPEAR_SIZE_BUS_SMALL)-1);
		valAg =     getRandNumBetwZeroAnd( pow(2,N_DNA_AG)-1 );
		valType =   getRandNumBetwZeroAnd( pow(2,N_DNA_INP)-1);
		initFitness = INIT_FITN_INP_BASE + getRandNumBetwZeroAnd(INIT_FITN_INP_RAND);
		appMat = new Message(valAppMat, APPEAR_SIZE_MAT);
		appBusSmall = new Message(valAppBus, APPEAR_SIZE_BUS_SMALL);
		DNA_ag = new Message(valAg, N_DNA_AG);
		DNA_type_i = new Message(valType, N_DNA_INP);
		while (position->isOccupied() && loopCheck<10000) {
			position = freeCells->GetAt( freeCells->FindIndex( getRandNumBetwZeroAnd( freeCells->GetCount()-1 ) ));
			loopCheck++;
		}			
		inp = new Inpoder(env, statHandler,stamp, agentGrid, position, syncMan, initFitness, initFitness, 0, false, syncMan->getSessionNum(), appMat, appBusSmall, DNA_ag, DNA_type_i);
		mlChange = getRandNumBetwZeroAnd( statHandler->AGENT_LIFETIME_MAX/2 );
		mlChange -= statHandler->AGENT_LIFETIME_MAX/4;
		inp->TESTscatterMaxLifeTime( mlChange );
	}
	std::cout << num << " inpoders created..." << std::endl;
	delete freeCells;
}

void AgentCreator::createEffectors() {
	GridCell* center = agentGrid->getCellAt(50,50); 
	int scanRadius = 49;
	int valAppMat;
	int valAppBus;
	int valAg;
	int valType;
	int mlChange;
	int initFitness;
	Message* appMat;
	Message* appBusSmall;
	Message* DNA_ag;
	Message* DNA_type_e;
	GridCell* position;
	int loopCheck = 0;
	CList<GridCell*, GridCell*>* freeCells;

	freeCells = agentGrid->getFreeCells( center , scanRadius);
	position = freeCells->GetAt( freeCells->FindIndex( getRandNumBetwZeroAnd( freeCells->GetCount()-1) ));
	Effector* eff;
	int num = 0;
	for (; num<INIT_NUM_OF_EFF; num++) {
		//freeCells = myGrid->getFreeCells( myGrid->getCellAt(35,35), 30);
		valAppMat = getRandNumBetwZeroAnd( pow(2,APPEAR_SIZE_MAT)-1 );
		valAppBus = getRandNumBetwZeroAnd( pow(2,APPEAR_SIZE_BUS_SMALL)-1 );
		valAg =     getRandNumBetwZeroAnd( pow(2,N_DNA_AG)-1 );
		valType =   getRandNumBetwZeroAnd( pow(2,N_DNA_EFF)-1 );
		initFitness = INIT_FITN_EFF_BASE + getRandNumBetwZeroAnd( INIT_FITN_EFF_RAND );
		appMat =  new Message(valAppMat, APPEAR_SIZE_MAT);
		appBusSmall = new Message(valAppBus, APPEAR_SIZE_BUS_SMALL);
		DNA_ag = new Message(valAg, N_DNA_AG);
		DNA_type_e = new Message(valType, N_DNA_EFF);
		//std::cout << DNA_type_e->toStringBits() << std::endl;
		//pressSpaceOrQuit();
		while (position->isOccupied() && loopCheck<10000) {
			position = freeCells->GetAt( freeCells->FindIndex( getRandNumBetwZeroAnd( freeCells->GetCount()-1) ));
			loopCheck++;
		}			
		eff = new Effector(env, statHandler, stamp, agentGrid, position, syncMan, initFitness, initFitness, 0, false, syncMan->getSessionNum(), appMat, appBusSmall, DNA_ag, DNA_type_e, decCat);
		/*std::cout << eff->toString() << std::endl;
		std::cout << "ideal bus2: " << eff->getAppIdealBus2()->toStringBits() << std::endl;
		std::cout << "picky bus2: " << eff->getAppPickiBus2()->toStringBits() << std::endl;
		pressSpaceOrQuit();*/
		mlChange = getRandNumBetwZeroAnd( statHandler->AGENT_LIFETIME_MAX/2 );
		mlChange -= statHandler->AGENT_LIFETIME_MAX/4;
		eff->TESTscatterMaxLifeTime( mlChange );
	}
	std::cout << num << " effectors created..." << std::endl;
	delete freeCells;
}

void AgentCreator::pressSpaceToQuit() {
	int ch;
	std::cout << "press space..." << std::endl;
	while (ch != ' ') {
		ch = _getch();
	}
	exit(0);
}

void AgentCreator::pressSpaceOrQuit() {
	int ch;
	std::cout << "press space or 'q' to quit..." << std::endl;
	while ((ch != ' ') && (ch != 'q')) {
		ch = _getch();
	}
	if (ch == 'q')
		exit(0);
}


int AgentCreator::getRandNumBetwZeroAnd(int maximum) {
	int randomNum;
	if (maximum<65536)  //no overflow risk
		randomNum = (rand()*(maximum+1)) / RAND_MAX;
	else //large number: use other method
		randomNum = ( ((double)rand()) / ((double)RAND_MAX)  ) *maximum;
	if (randomNum<0) {
		std::cout << "ERROR: getRandomNum" << std::endl;
		pressSpaceToQuit();
	}
	if (randomNum>maximum)
		randomNum=maximum; //happens when rand() = RAND_MAX
	return randomNum;
}
