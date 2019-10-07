// Colline.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Colline.h"
#include <afxwin.h> //for wnd and bitmap
#include <afxcoll.h> //used for CUINTArray
//#include <conio.h> //for press key
#include "WndColline.h"
#include "WndCollineText.h"
#include "Fountain.h"
#include "dCollector.h"
#include "Inpoder.h"
#include "Effector.h"
#include "Environment.h"
#include "Message.h"
#include "GridCell.h"
#include "GridRow.h"
#include "Grid.h"
#include "UIntGrid.h"
#include "SyncManager.h"
#include "DecisionCategory.h"
#include "StatisticsHandler.h"
#include "Konst.h"
#include "AgentCreator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
		/*CString strHello;
		strHello.LoadString(IDS_HELLO);
		cout << (LPCTSTR)strHello << endl;*/
		//Message* test = new Message( 1982464, 27);
		/*Message* test = new Message( pow(2,12)+4, 27);
		cout << (LPCTSTR)test->toStringBits() << endl;
		while (true) {}*/
		SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
		srand( 12 ); //(unsigned)time( NULL ) );
		bool isNewOrganism = true;
		int fileIdVal = 137;
		CString filename_beg = "cdata137";
		CString fn_ses = "3739";
		CString filename_pop = filename_beg + FN_AGENTPOP + "_" + fn_ses + FN_EXT;
		CString filename_org = filename_beg + FN_ORGANISMVAR + "_" + fn_ses + FN_EXT;
		//cout << "pop: " << (LPCTSTR)filename_pop << endl;
		//cout << "org: " << (LPCTSTR)filename_org << endl;
		//while (true) {}
		int initCycNum, initSesNum, fitnEnv, initMatBonusBuffer, initFitnBuff0, initFitnBuff1, initSesRew, initDnaSamNum, initIdCounterVal;
		if (isNewOrganism) {
			cout << "Initializing new organism..." << endl;
			initCycNum = 0;
			initSesNum = -1;
			fitnEnv = 0; //is set later: equal total agent fitness 
			initMatBonusBuffer = 0;
			initFitnBuff0 = 0;
			initFitnBuff1 = 0;
			initSesRew = 0;
			initDnaSamNum=0;
			initIdCounterVal=0;
		} else {
			int restoredCycNum, restoredSesNum, restoredFitnEnv, restoredMatingBuff, restoredFitnBuff0, restoredFitnBuff1, restoredSesRew, restoredDnaSamNum, restoredIdCounterVal;
			FILE* organismVarFile;
			if ((organismVarFile = fopen( filename_org, "r")) == NULL) {
				cout << "ERROR: couldn't open file, file not found" << endl;
				cout << (LPCTSTR)filename_org << endl;
				exit(0); //pressSpaceToQuit();
			} else
				cout << "Loading variables..." << endl;
			fseek( organismVarFile, 0, SEEK_SET);
			fscanf(organismVarFile,"%d", &restoredCycNum); 
			fscanf(organismVarFile,"%d", &restoredSesNum); 
			fscanf(organismVarFile,"%d", &restoredFitnEnv); 
			fscanf(organismVarFile,"%d", &restoredMatingBuff);
			fscanf(organismVarFile,"%d", &restoredFitnBuff0);
			fscanf(organismVarFile,"%d", &restoredFitnBuff1);
			fscanf(organismVarFile,"%d", &restoredSesRew);
			fscanf(organismVarFile,"%d", &restoredDnaSamNum);
			fscanf(organismVarFile,"%d", &restoredIdCounterVal);
			fclose(organismVarFile);
			initCycNum = restoredCycNum;
			initSesNum = restoredSesNum;
			fitnEnv = restoredFitnEnv;
			initMatBonusBuffer = restoredMatingBuff;
			initFitnBuff0 = restoredFitnBuff0;
			initFitnBuff1 = restoredFitnBuff1;
			initSesRew = restoredSesRew;
			initDnaSamNum = restoredDnaSamNum;
			initIdCounterVal = restoredIdCounterVal;
			//cout << "Restored values: " << initCycNum << ", " << initSesNum << ", " << initFitnEnv << ", " << initFitnBuff << ", " << initSesRew << endl;
		}


		Grid* myGrid = new Grid(GRIDSIZE, GRIDSIZE);
		IdStamp* myStamp = new IdStamp(initIdCounterVal);
		Environment* myEnv = new Environment(fitnEnv, initMatBonusBuffer); // balance should be maintained		
		StatisticsHandler* myStatHandler = new StatisticsHandler(isNewOrganism, initSesRew, myGrid, fileIdVal, initDnaSamNum );
		//myStatHandler->setEnvironment(myEnv); //don't ask
		myStatHandler->adjustFitnessEnvironment(fitnEnv);
		WndColline* gView = new WndColline(myStatHandler, GRIDSIZE, GRIDSIZE);
		WndCollineText* agentView = new WndCollineText( myStatHandler, IDR_CINFO,400,0,900,500 );
		WndCollineText* trainingView = new WndCollineText( myStatHandler, IDR_TRAIN,0,450,900,900 );
		WndCollineText* varStatView = new WndCollineText( myStatHandler, IDR_CSTAT,0,900,900,1200 );
		
		SyncManager* mySync = new SyncManager(myStamp, myEnv, myStatHandler, gView, agentView, trainingView, varStatView, initCycNum, initSesNum);
		DecisionCategory* myCat = new DecisionCategory(initFitnBuff0, initFitnBuff1, myEnv, myStatHandler, myStamp, mySync, COLLECTION_TIME);
		AgentCreator* creator = new AgentCreator(myEnv, myStatHandler, mySync, myGrid, myStamp, myCat);
		if (isNewOrganism) {
			cout << "creating fountains..." << endl;
			creator->createFountains();
			cout << "creating collectors..." << endl;
			creator->createCollectors();
			cout << "creating inpoders..." << endl;
			creator->createInpoders();
			cout << "creating effectors..." << endl;
			creator->createEffectors();
			int initFitnEnv = (myStatHandler->getFitnessColline() *myStatHandler->IDEAL_NUM_OF_AGENTS_IN_GRID)/(INIT_NUM_OF_COLL + INIT_NUM_OF_INP + INIT_NUM_OF_EFF); //balance should be maintained
			myEnv->setFitness( initFitnEnv ); 
			myStatHandler->adjustFitnessEnvironment( initFitnEnv );
		} else
			creator->restorePopulation(filename_pop);
		mySync->run();
		/* DEBUG test begin:
		GridCell* center = myGrid->getCellAt(50,50); 

		CList<GridCell*, GridCell*>* freeCells;
		freeCells = myGrid->getFreeCells( center, 4 );
		GridCell* position = freeCells->GetAt( freeCells->FindIndex( 2 ));
		GridCell* position2 = freeCells->GetAt( freeCells->FindIndex( 3 ));
		GridCell* pos3 = freeCells->GetAt( freeCells->FindIndex( 4 ));

		int initFitness = 10;
		Message* appMat = new Message(1, APPEAR_SIZE_MAT);
		Message* appBusSmall = new Message(2, APPEAR_SIZE_BUS_SMALL);
		Message* DNA_ag = new Message(3, N_DNA_AG);
		Message* DNA_type_i = new Message(4, N_DNA_INP);
		Inpoder* inp = new Inpoder(myEnv, myStatHandler,myStamp, myGrid, position, mySync, initFitness, initFitness, 0, false, mySync->getSessionNum(), appMat, appBusSmall, DNA_ag, DNA_type_i);
		Inpoder* inp2 = new Inpoder(myEnv, myStatHandler,myStamp, myGrid, position2, mySync, initFitness, initFitness, 0, false, mySync->getSessionNum(), appMat, appBusSmall, DNA_ag, DNA_type_i);
		Fountain* f1 = new Fountain(myEnv, myStatHandler, myStamp, myGrid, pos3, mySync, appBusSmall, 0);

		f1->bidders->AddTail(inp2);
		DEBUG test end*/
		cout << "closing windows..." << endl;
		gView->DestroyWindow();
		agentView->DestroyWindow();
		trainingView->DestroyWindow();
		varStatView->DestroyWindow();

		/*WndColline* collineView = new WndColline(16,16);
		collineView->ActivateFrame();
		
		CUIntArray* testViewC = new CUIntArray;
		testViewC->SetSize(256);
		
		UINT randVal; //BYTE
		int i;		
		for (i=0; i<256; i++) {
			randVal = (rand() * (pow(2,32)-1)) / RAND_MAX; 
			testViewC->SetAt(i, RGB(i,0,i));
		}
		collineView->paintGrid( testViewC );
		for (i=0; i<256; i++) {
			randVal = (rand() * (pow(2,32)-1)) / RAND_MAX; 
			testViewC->SetAt(i, RGB(0,i,i)); //randVal);
		}
		collineView->paintGrid( testViewC );
		for (i=0; i<256; i++) {
			randVal = (rand() * (pow(2,32)-1)) / RAND_MAX; 
			testViewC->SetAt(i, RGB(i,i,0));
		}
		collineView->paintGrid( testViewC );*/
		/*for (i=0; i<256; i++) {
			randVal = (rand() * (pow(2,32)-1)) / RAND_MAX; 
			testViewC->SetAt(i, RGB(i,i,0));
		}
		collineView->paintGrid( testViewC );*/


	}

	return nRetCode;
}


