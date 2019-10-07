// SyncManager.cpp: implementation of the SyncManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Colline.h"
#include "SyncManager.h"
#include "Agent.h"
#include "DecisionCategory.h"
#include "Fountain.h"
#include "iostream.h"
#include "UIntGrid.h"
#include <conio.h> //for press key
#include "Konst.h"
//#include "math.h"
//using namespace jf_w3_agent;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//using namespace jf_w3_sync;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//the SyncManager is responsible for syncronizing cycle activities (initiating agents and updating statistics)

SyncManager::SyncManager(IdStamp* stamp, Environment* e, StatisticsHandler* sh, WndColline* gView, WndCollineText* aView, WndCollineText* tView, WndCollineText* vsView, int restCycNum, int restSesNum)
{
	systemIdStamp = stamp;
	env = e;
	statHandler = sh;
	gridView = gView;
	agentView = aView;
	trainingView = tView;
	varStatView = vsView;
	
	//-----------initialize values:-------------
	cycleNum = restCycNum; //is 0 if new organism
	sessionNum = restSesNum; //is -1 if new organism

	isLoadingFountains1st = false;
	isLoadingFountains2nd = false;
	isLoadingFountains3rd = false;
	loadingBeginCycle1st = -1000;
	loadingBeginCycle2nd = -1000;
	loadingBeginCycle3rd = -1000;

	currentProcessors = new CTypedPtrList<CObList, Observable*>(100);      //new CList<Observable*, Observable*>(100);
	currentDrifters = new CTypedPtrList<CObList, Observable*>(300); //CList<Observable*, Observable*>(300);
	nextProcessors = new CTypedPtrList<CObList, Observable*>(100);  //new CList<Observable*, Observable*>(100);
	nextDrifters = new CTypedPtrList<CObList, Observable*>(300); //CList<Observable*, Observable*>(300);
	nextDead = new CTypedPtrList<CObList, Observable*>(50);      //CList<Observable*, Observable*>(50);
	fountains = new CTypedPtrList<CObList, Observable*>(50);     //CList<Observable*, Observable*>(50);

	//stopSystem = false;

	//idTags_currentAgents = new CList<int,int>(100); //UIntArray();
	//testIdList_hasNotBeenDrifters = new CList<int,int>(100);
	list_currentAgents = new CTypedPtrList<CObList, Observable*>(300); //CList<Observable*, Observable*>(100);
	//list_hasNotBeenDrifters = new CList<Observable*, Observable*>(100);
	
	
}

SyncManager::~SyncManager()
{
	
}

/*void SyncManager::addIdToIdTagList(int newbornAgentId) {
	idTags_currentAgents->AddTail(newbornAgentId);
}*/

void SyncManager::addAgentToList(Observable* newbornAgent) {
	list_currentAgents->AddTail(newbornAgent);
}


/*void SyncManager::removeIdFromIdTagList(int deadAgentId) {
	POSITION pos = idTags_currentAgents->Find( deadAgentId);
	if (pos==NULL) {
		cout << "ERROR: removeIdTag, id not in list" << endl;
		pressSpaceToQuit();
	}
	idTags_currentAgents->RemoveAt( pos );
}*/

void SyncManager::removeAgentFromList(Observable* deadAgent) {
	POSITION pos = list_currentAgents->Find( deadAgent);
	if (pos==NULL) {
		cout << "ERROR: removeIdTag, agent not in list" << endl;
		pressSpaceToQuit();
	}
	list_currentAgents->RemoveAt( pos );
}



/*void SyncManager::removeAgentFromHasNotBeenDrifterList(Observable* drifter) {
	POSITION pos = list_hasNotBeenDrifters->Find( drifter );
	if (pos!=NULL) 
		testIdList_hasNotBeenDrifters->RemoveAt( pos );
}*/

void SyncManager::isDrifterNextCycle(Observable* ag) {
	nextDrifters->AddTail(ag); // nextDrifters.push_back(ag);
}

void SyncManager::isProcessorNextCycle(Observable* ag) {
	nextProcessors->AddTail(ag); //nextProcessors.push_back(ag);
}

//Remember: add cycle num test
void SyncManager::isDeadNextCycle(Observable* ag) { 
	//cout << "Agent: " << ag->toStringId() << " dies in next cycle";
	nextDead->AddTail(ag); // nextDead.push_back(ag);
}

int SyncManager::getCycleNum() {
	return cycleNum;
}

int SyncManager::getSessionNum() {
	return sessionNum;
}

//returns an index for where in response-collection-period the system is (between 0 and COLLECTION_TIME-1)
int SyncManager::getCollectionIndex() {
	int collectionIndex;
	//note collectionIndex is calculated from 'loadingBeginCycle1st' ...2nd is not used/necessary
	if ((cycleNum >= (loadingBeginCycle1st + statHandler->COLL_MIN_PROCTIME)) && (cycleNum <= (loadingBeginCycle1st + statHandler->COLL_MAX_PROCTIME)))
		collectionIndex = cycleNum - loadingBeginCycle1st - statHandler->COLL_MIN_PROCTIME;
	else 
		collectionIndex = -1;
	return collectionIndex; 
}
 
void SyncManager::run() {
	//int lastSessionChosenLevel = decCat->getChosenPickiLevel();
	//prepareForNextSession(); //´copies values for last training session
	//delete currentDrifters;	
	//currentDrifters = nextDrifters; //nextDrifters are 'loaded' under agent construction
	//nextDrifters = new CList<Observable*, Observable*>(100);
	shakeFrozenAgents(100); //set all agents to drifters
	cout << "num of drifters: " << nextDrifters->GetCount() << endl;
	nextStopSession = sessionNum+1;
	cout << " num of agents total: " << statHandler->getNumOfAgents() << endl;
	pressSpaceOrQuit();
	while (statHandler->getNumOfAgents() > NUM_OF_FOUNTAINS) {
		prepareForNextCycle();
		if (isLoadingFountains1st || isLoadingFountains2nd || isLoadingFountains3rd) 
			loadFountains( ); //1
		initiateProcessors(); //2
		//cout << "syncman: initiates drifters" << endl;
		initiateDrifters(); //3
		deleteDeadAgents(); //4
		updateAllViews();
	}
	cout << "All living agents are dead! ..." << endl;
	//statHandler->closeFiles();
	pressSpaceToQuit();

}

void SyncManager::prepareForNextCycle() {
	statHandler->saveCycleData(cycleNum); //very first cycle activity is to save data from the previous cycle
	//deleteDeadAgents(); //delete agents that are dead in this cycle
	cycleNum++;
	//cout << "\n*********************************************" << endl;
	//cout << "cycle num = " << cycleNum  << endl;
	delete currentDrifters;
	currentDrifters = nextDrifters;
	nextDrifters = new CTypedPtrList<CObList, Observable*>(300); //CList<Observable*,Observable*>(100); //fix use constant
	delete currentProcessors;
	currentProcessors = nextProcessors;
	nextProcessors =  new CTypedPtrList<CObList, Observable*>(100);// new CList<Observable*,Observable*>(100); //fix use constant
	if ((cycleNum % statHandler->SESSION_LENGTH) == 0) { //show result of session and begin fountain-inflow for new session			
		prepareForNextSession(); //clear variables, what to pay effectors, etc.		
		isLoadingFountains1st = true;
		loadingBeginCycle1st = cycleNum;
	}
	if ((cycleNum-loadingBeginCycle1st)==BEGIN_2ND_LOAD_TIME && SECOND_LOAD_ON) {
		bool chosenCatFirstLoad = env->getLoadedCategory();
		env->fillPipesCircleOrNotCircle(chosenCatFirstLoad);
		isLoadingFountains2nd = true;
		loadingBeginCycle2nd = cycleNum;
	}
	if ((cycleNum-loadingBeginCycle1st)==BEGIN_3RD_LOAD_TIME && THIRD_LOAD_ON) {
		bool chosenCatFirstLoad = env->getLoadedCategory();
		env->fillPipesCircleOrNotCircle(chosenCatFirstLoad);
		isLoadingFountains3rd = true;
		loadingBeginCycle3rd = cycleNum;
	}

	if (isLoadingFountains1st && (cycleNum-loadingBeginCycle1st) == LOADING_TIME) {
		isLoadingFountains1st = false; //stop first fountain inflow 
	}
	if (isLoadingFountains2nd && (cycleNum-loadingBeginCycle2nd) == LOADING_TIME) {
		isLoadingFountains2nd = false; //stop second fountain inflow 
	}
	if (isLoadingFountains3rd && (cycleNum-loadingBeginCycle3rd) == LOADING_TIME) {
		isLoadingFountains3rd = false; //stop third fountain inflow 
	}

}

void SyncManager::setHasBeenDrifterFlags() {
	int numOfAgents = list_currentAgents->GetCount();
	Agent* tempAgent;
	for (int i=0; i<numOfAgents; i++) {
		tempAgent = (Agent*)list_currentAgents->GetAt( list_currentAgents->FindIndex(i));
		tempAgent->setHasBeenActiveInSession(false);
	}	
}

void SyncManager::shakeFrozenAgents(int chanceToDrift) {
	if (chanceToDrift>0)
		cout << "Shaking frozen agents..." << endl;
	int numOfFrozen = 0; 
	int numOfShaken = 0;
	Agent* tempAgent;
	for (int i=0; i < list_currentAgents->GetCount(); i++) {
		tempAgent = (Agent*)list_currentAgents->GetAt( list_currentAgents->FindIndex(i));
		if (!tempAgent->getHasBeenActiveInSession() && (tempAgent->getType() != TYPE_FOUNTAIN)) {
			numOfFrozen++;
			if (getRandNumBetwZeroAnd(99) < chanceToDrift) {
				//tempAgent->clearNumOfObservedAgents();
				tempAgent->setStatusForNextDrifter();
				numOfShaken++;
			}
		}
	}
	cout << "INFO: " << numOfFrozen << " agents were 'frozen' in this session, " << numOfShaken << " of them is now drifters " << endl;
}

void SyncManager::prepareForNextSession() {
	shakeFrozenAgents(SHAKE_PERCENT);
	setHasBeenDrifterFlags(); //set all flags to false
	//test num of agents with taglist:
	if (list_currentAgents->GetCount() != statHandler->getNumOfAgents()) {
		cout << "ERROR: idTag_list and numOfAgents mismatch!" << endl;
		cout << "list_currentAgents size: " << list_currentAgents->GetCount() << endl;
		cout << "numOfAgents:     " << statHandler->getNumOfAgents() << endl;
		pressSpaceToQuit();
	} //end test numOfAgents
	statHandler->saveSessionData();
	DecisionCategory* decCat= (DecisionCategory*)decCategory; //Not nice style
	//decCat->giveRemainCatBuffToMatBonusBuff(env->getLoadedCategory());
	cout << "Trainingsession number " << sessionNum << " has just ended" << endl;
	//cout << env->getLastMessageString() << endl; //  "\nMessage from env: " << (LPCTSTR)tempMess->toStringBits() << " , answer = " << env->getLastAnswer() << endl;
	//pressSpaceOrQuit();
	sessionNum++;
	//place saveDNAinfo here?
	if (sessionNum == nextStopSession) {
		int choice = 'a';
		while (choice == 'a') {
			choice = press12345SaveOrQuit();
			if (choice == '1')
				nextStopSession = sessionNum+1;
			else if (choice == '2')
				nextStopSession = sessionNum+10;
			else if (choice == '3')
				nextStopSession = sessionNum+100;
			else if (choice == '4')
				nextStopSession = sessionNum+1000;
			else if (choice == 'a')
				adjustParameters();
			else if (choice == 's') {
				statHandler->closeFiles();
				statHandler->storePopulation(cycleNum, sessionNum, env->getMatingBonusBuffer(), decCat->getFitnessBuffer_env(0), decCat->getFitnessBuffer_env(1), systemIdStamp->getCounter());
				exit(0);
			} else {
				cout << "ERROR: menu choice" << endl;
				pressSpaceToQuit();
			}
		}
	}
	bool newLoadedCategory = env->fillPipesCircleOrNotCircle(sessionNum, statHandler->getPerformance(0), statHandler->getPerformance(1));
	statHandler->resetSessionData( newLoadedCategory );

	if (sessionNum % SESS_BETW_REWARD_UPDATE == 0) {
		statHandler->updateSessionReward();
	}
	if (sessionNum % SESS_BETW_AGENT_SAMPLE == 0) {
		statHandler->saveDNAinfo();
	}
	
	decCat->fillFitnessBuffer_env( env->getLoadedCategory() ); //used to reward effectors responding in this session
	//decCat->chooseLevel(); //set pickiness level for effector responses (the appearances that gets bids)
	statHandler->setThisSesChosenPickiLevel( decCat->getChosenPickiLevel());
	decCat->setRewardsOnLevels( ); //determine level rewards based on number of resp. effectors last session

}

void SyncManager::loadFountains() {
	//cout << "DEBUG: loading fountains..."<< endl;
	if (isLoadingFountains1st + isLoadingFountains2nd + isLoadingFountains3rd > 1) {
		cout << "ERROR: 1st,2nd or 3rd fountain inflow can not overlap!" << endl;
		pressSpaceToQuit();
	}
	int loadingSlot;
	if (isLoadingFountains1st)
		loadingSlot = cycleNum - loadingBeginCycle1st; //loadingSlot is between 0 and LOADING_TIME - 1
	else if (isLoadingFountains2nd)
		loadingSlot = cycleNum - loadingBeginCycle2nd;
	else if (isLoadingFountains3rd)
		loadingSlot = cycleNum - loadingBeginCycle3rd;
	else {
		cout << "ERROR: loadFountains" << endl;
		pressSpaceToQuit();
	}
	if (loadingSlot < 0 || loadingSlot > (LOADING_TIME-1)) {
		cout << "ERROR: loadingSlot error" << endl;
		pressSpaceToQuit();
	}
	Fountain* currentFountain;
	for (int fNum=0; fNum < NUM_OF_FOUNTAINS; fNum++) {
		currentFountain = (Fountain*)fountains->GetAt( fountains->FindIndex(fNum));
		if (loadingSlot == currentFountain->getLoadingSlot())
			loadFountain(fNum);
	}

}

void SyncManager::loadFountain(int fNum) {
	//cout << "loading fountain " << fNum << endl;
	Fountain* currFountain = (Fountain*)fountains->GetAt( fountains->FindIndex(fNum)); //(Fountain*)fountains.at(fNum);
	//cout << "syncman: Fountain " << currFountain->getId() << " 'loaded' (processor next cycle)" << endl;
	//if (fNum>19 && fNum<25) {//middle fountain group
	//	Message* ignoredMess = env->getNextMessageFromPipe( fNum );
	//	delete ignoredMess;
	//}else
		currFountain->receiveMessage( env->getNextMessageFromPipe( fNum ) );
}

void SyncManager::initiateProcessors() {
	Observable* currentAgent;
	int numOfProcessors = currentProcessors->GetCount();
	if (numOfProcessors > 0) {
		//cout << "syncman: Initiates " << numOfProcessors << " processors" << endl;
		//pressSpaceOrQuit();
	}
	int i;
	for (i=0; i<currentProcessors->GetCount(); i++) {
		currentAgent = currentProcessors->GetAt( currentProcessors->FindIndex(i));
		currentAgent->process(); //Process and sell. Collect nextDrifters, nextProcessors and nextDead
	}
	/* OLD slow method
	while (numOfProcessors>0) {
		currentAgent = currentProcessors->RemoveTail();
		currentAgent->process();
		numOfProcessors--;
	}*/	
}

void SyncManager::initiateDrifters() {
	Observable* currentAgent;
	int numOfDrifters = currentDrifters->GetCount();
	//cout << "syncman: Initiates " << numOfDrifters << " drifters" << endl;
	int i;
	for (i=0; i<currentDrifters->GetCount(); i++) {
		currentAgent = currentDrifters->GetAt( currentDrifters->FindIndex(i)); //collect nextDrifters (newborn) and nextObservers
		currentAgent->drift();
	}
	/* OLD (slower) method
	while (numOfDrifters>0) {
		currentAgent = currentDrifters->RemoveTail();
		currentAgent->drift();
		numOfDrifters--;
	}*/
}

void SyncManager::deleteDeadAgents() {
	Observable* deadAgent;
	int numOfDead = nextDead->GetCount();
	if (numOfDead > 0) {
		//cout << "syncman: Deletes " << numOfDead << " dead agents" << endl;
		//pressSpaceOrQuit();
	}	
	int i;
	for (i=0; i<numOfDead; i++) {
		deadAgent = nextDead->GetAt( nextDead->FindIndex(i));
		delete deadAgent;
	}
	nextDead->RemoveAll();
	/* OLD method , slow
	while (numOfDead > 0) {
		deadAgent = nextDead->RemoveTail();
		delete deadAgent;
		numOfDead--;
	}*/
}

void SyncManager::addFountain(Observable* f) {
	fountains->AddTail(f);
}

void SyncManager::pressSpaceToQuit() {
	int ch;
	cout << "press space..." << endl;
	while (ch != ' ') {
		ch = _getch();
	}
	quit();
}

void SyncManager::pressSpaceOrQuit() {
	int ch;
	cout << "press space or 'q' to quit..." << endl;
	while ((ch != ' ') && (ch != 'q')) {
		ch = _getch();
	}
	if (ch == 'q')
		quit();
}

int SyncManager::press12345SaveOrQuit() {
	int ch = 0;
	cout << "Choice..." << endl;
	cout << " '1': run 1 session" << endl;
	cout << " '2': run 10 sessions" << endl;
	cout << " '3': run 100 sessions" << endl;
	cout << " '4': run 1000 sessions" << endl;
	cout << " 'a': adjust parameters" << endl;
	cout << " 's': close files and save population" << endl;
	cout << "  ....or 'Q' to quit" << endl;
	while ((ch != '1') && (ch != '2') && (ch != '3') && (ch != '4') && (ch != 'a') && (ch != 's') && (ch != 'Q')) {
		ch = _getch();
	}
	if (ch == 'Q')
		quit();
	return ch;
}

void SyncManager::adjustParameters() {
	int ch = 0;
	int increment = 0;
	cout << "\n-----------------------------------------\nAdjustable parameters:" << endl;
	cout << " '1': MAX_STATE_BUYERS_INP        = " << statHandler->MAX_STATE_BUYERS_INP << endl;
	cout << " '4': BIDP_BASE_INP               = " << statHandler->BIDP_BASE_INP << endl;
	cout << " '5': BIDP_STEP_INP               = " << statHandler->BIDP_STEP_INP << " (" << pow(2,BIDLEVELS_LENGTH) << " steps)" << endl;
	cout << " '6': BIDP_BASE_EFF               = " << statHandler->BIDP_BASE_EFF << endl;
	cout << " '7': BIDP_STEP_EFF               = " << statHandler->BIDP_STEP_EFF << " (" << pow(2,BIDLEVELS_LENGTH) << " steps)" << endl;
	cout << " '8': SESSION_LENGTH              = " << statHandler->SESSION_LENGTH << endl;
	cout << " '9': AGENT_LIFETIME_MAX          = " << statHandler->AGENT_LIFETIME_MAX << " , (avg: " << (int)floor(statHandler->avgAgentAge) << ", %hitMax: " << (int)floor(statHandler->percThatReachMaxAge) << ")" << endl;
	cout << " 'z': COLL_MIN_PROCTIME           = " << statHandler->COLL_MIN_PROCTIME << endl;
	cout << " 'x': GRID_ZOOM                   = " << statHandler->GRID_ZOOM << endl;
	cout << " 'c': C_MATSTART_PRC              = " << statHandler->C_MATSTART_PRC << endl;
	cout << " 's': DC_REW_FROM_BID_AGR_LEVEL   = " << statHandler->DC_REW_FROM_BID_AGR_LEVEL << endl;
	cout << " 'd': MAX_INACTIVE_SESS           = " << statHandler->MAX_INACTIVE_SESS << endl;
	cout << " 'f': MUTATION_RATE               = " << statHandler->MUTATION_RATE << endl;
	cout << " 'k': HIGH_MUT_AGENTS_PRC         = " << statHandler->HIGH_MUT_AGENTS_PRC << endl;	
	cout << " 'g': MIN_NUM_OF_AGENTS_IN_GRID   = " << statHandler->MIN_NUM_OF_AGENTS_IN_GRID << endl;
	cout << " 'j': IDEAL_NUM_OF_AGENTS_IN_GRID = " << statHandler->IDEAL_NUM_OF_AGENTS_IN_GRID << endl;
	cout << " 'h': GRAVITY_CENTER              = " << statHandler->GRAVITY_CENTER << endl;
	cout << " 'l': DC_EARLY_RESP_REW_I0_PRC    = " << statHandler->DC_EARLY_RESP_REW_I0_PRC << endl;
	cout << " 'o': MAX_NUM_OF_OBS_INP          = " << statHandler->MAX_NUM_OF_OBS_INP << endl;
	cout << " 'p': MAX_NUM_OF_OBS_EFF(each inp)= " << statHandler->MAX_NUM_OF_OBS_EFF << endl;
	
	cout << " 'e': End adjusting" << endl;

	while ((ch != '1') && (ch != '4') && (ch != '5') && (ch != '6') && (ch != '7') && (ch != '8') && (ch != '9') && (ch != 'z') && (ch != 'x') && (ch != 'c') && (ch != 's') && (ch != 'd') && (ch != 'f') && (ch != 'g') && (ch != 'h') && (ch != 'j') && (ch != 'k') && (ch != 'l') && (ch != 'p') && (ch != 'o') && (ch != 'e') ) {
		ch = _getch();
	}
	if (ch!='e') {
		cout << "\nPress '+' or '-' to increase/decrease value of ";
		if (ch == '1') {
			increment = 1;
			cout << "MAX_STATE_BUYERS_INP" << endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+')
				statHandler->MAX_STATE_BUYERS_INP += increment;
			else {
				statHandler->MAX_STATE_BUYERS_INP -= increment;
				if (statHandler->MAX_STATE_BUYERS_INP < 0) {
					cout << "\n***MAX_STATE_BUYERS_INP has reached mimimum! (no change)***" << endl;
					statHandler->MAX_STATE_BUYERS_INP += increment;
				}
			}
			adjustParameters();
		}
		if (ch == '4') { // adjust BIDP_BASE_INP
			increment = 5;
			cout << "BIDP_BASE_INP" << endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->BIDP_BASE_INP += increment;
				if (statHandler->BIDP_BASE_INP +  statHandler->BIDP_STEP_INP * (pow(2,BIDLEVELS_LENGTH)-1) > 100 ) {
					cout << "\n***BIDP_BASE_INP has reached maximum! (no change)***" << endl;
					statHandler->BIDP_BASE_INP -= increment;
				}
			} else {
				statHandler->BIDP_BASE_INP -= increment;
				if (statHandler->BIDP_BASE_INP < 5) {
					cout << "\n***BIDP_BASE_INP has reached mimimum! (no change)***" << endl;
					statHandler->BIDP_BASE_INP += increment;
				}
			}
			adjustParameters();
		}
		if (ch == '5') { //adjust BIDP_STEP_INP
			increment = 1;
			cout << "BIDP_STEP_INP" << endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->BIDP_STEP_INP += increment;
				if (statHandler->BIDP_STEP_INP * (pow(2,BIDLEVELS_LENGTH)-1) + statHandler->BIDP_BASE_INP > 100 ) {
					cout << "\n***BIDP_STEP_INP has reached maximum! (no change)***" << endl;
					statHandler->BIDP_STEP_INP -= increment;
				}
			} else {
				statHandler->BIDP_STEP_INP -= increment;
				if (statHandler->BIDP_STEP_INP < 0) {
					cout << "\n***BIDP_STEP_INP has reached minimum! (no change)***" << endl;
					statHandler->BIDP_STEP_INP += increment;
				}
			}
			adjustParameters();
		}
		if (ch == '6') { // adjust BIDP_BASE_EFF
			increment = 2;
			cout << "BIDP_BASE_EFF" << endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->BIDP_BASE_EFF += increment;
				if (statHandler->BIDP_BASE_EFF +  statHandler->BIDP_STEP_EFF * (pow(2,BIDLEVELS_LENGTH)-1) > 100 ) {
					cout << "\n***BIDP_BASE_EFF has reached maximum! (no change)***" << endl;
					statHandler->BIDP_BASE_EFF -= increment;
				}
			} else {
				statHandler->BIDP_BASE_EFF -= increment;
				if (statHandler->BIDP_BASE_EFF < 5) {
					cout << "\n***BIDP_BASE_EFF has reached mimimum! (no change)***" << endl;
					statHandler->BIDP_BASE_EFF += increment;
				}
			}
			adjustParameters();
		}
		if (ch == '7') { //adjust BIDP_STEP_EFF
			increment = 1;
			cout << "BIDP_STEP_EFF" << endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->BIDP_STEP_EFF += increment;
				if (statHandler->BIDP_STEP_EFF * (pow(2,BIDLEVELS_LENGTH)-1) + statHandler->BIDP_BASE_EFF > 100 ) {
					cout << "\n***BIDP_STEP_EFF has reached maximum! (no change)***" << endl;
					statHandler->BIDP_STEP_EFF -= increment;
				}
			} else {
				statHandler->BIDP_STEP_EFF -= increment;
				if (statHandler->BIDP_STEP_EFF < 0) {
					cout << "\n***BIDP_STEP_EFF has reached minimum! (no change)***" << endl;
					statHandler->BIDP_STEP_EFF += increment;
				}
			}
			adjustParameters();
		}
		if (ch == '8') { //adjust SESSION_LENGTH
			increment = 1;
			cout << "SESSION_LENGTH" << endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->SESSION_LENGTH += increment;
			} else {
				statHandler->SESSION_LENGTH -= increment;
				if (statHandler->SESSION_LENGTH < (LOADING_TIME+1)) {
					cout << "\n***SESSION_LENGTH has reached minimum! (no change)***" << endl;
					statHandler->SESSION_LENGTH += increment;
				}
			}
			adjustParameters();
		}
		if (ch == '9') { //adjust AGENT_LIFETIME
			increment = statHandler->SESSION_LENGTH;
			cout << "AGENT_LIFETIME_MAX" << endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->AGENT_LIFETIME_MAX += increment;
				if (statHandler->AGENT_LIFETIME_MAX > (statHandler->SESSION_LENGTH*20)) {
					cout << "\n***AGENT_LIFETIME_MAX has reached maximum! (no change)***" << endl;
					statHandler->AGENT_LIFETIME_MAX -= increment;
				}

			} else {
				statHandler->AGENT_LIFETIME_MAX -= increment;
				if (statHandler->AGENT_LIFETIME_MAX < 10) {
					cout << "\n***AGENT_LIFETIME_MAX has reached minimum! (no change)***" << endl;
					statHandler->AGENT_LIFETIME_MAX += increment;
				}
			}
			adjustParameters();
		}
		
		if (ch == 'z') { //adjust COLL_MIN_PROCTIME
			increment = 1;
			cout << "COLL_MIN_PROCTIME" << endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->COLL_MIN_PROCTIME += increment;
				if (statHandler->COLL_MIN_PROCTIME > statHandler->SESSION_LENGTH-COLLECTION_TIME ) {
					cout << "\n***COLL_MIN_PROCTIME has reached maximum! (no change)***" << endl;
					statHandler->COLL_MIN_PROCTIME -= increment;
				}
				statHandler->COLL_MAX_PROCTIME = statHandler->COLL_MIN_PROCTIME + (COLLECTION_TIME-1);
			} else {
				statHandler->COLL_MIN_PROCTIME -= increment;
				if (statHandler->COLL_MIN_PROCTIME < 0) {
					cout << "\n***COLL_MIN_PROCTIME has reached minimum! (no change)***" << endl;
					statHandler->COLL_MIN_PROCTIME += increment;
				}
				statHandler->COLL_MAX_PROCTIME = statHandler->COLL_MIN_PROCTIME + (COLLECTION_TIME-1);
			}
			adjustParameters();
		}
		if (ch == 'x') { //adjust GRID_ZOOM
			increment = 1;
			cout << "GRID_ZOOM" << endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->GRID_ZOOM += increment;
				if (statHandler->GRID_ZOOM > 6 ) {
					cout << "\n***GRID_ZOOM has reached maximum! (no change)***" << endl;
					statHandler->GRID_ZOOM -= increment;
				}
			} else {
				statHandler->GRID_ZOOM -= increment;
				if (statHandler->GRID_ZOOM < 1) {
					cout << "\n***GRID_ZOOM has reached minimum! (no change)***" << endl;
					statHandler->GRID_ZOOM += increment;
				}
			}
			adjustParameters();
		}
		if (ch == 'c') { //adjust C_MATSTART_PRC
			increment = 10;
			cout << "C_MATSTART_PRC" << endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->C_MATSTART_PRC += increment;
				if (statHandler->C_MATSTART_PRC > 10000 ) {
					cout << "\n***C_MATSTART_PRC has reached maximum! (no change)***" << endl;
					statHandler->C_MATSTART_PRC -= increment;
				}
			} else {
				statHandler->C_MATSTART_PRC -= increment;
				if (statHandler->C_MATSTART_PRC < 101) {
					cout << "\n***C_MATSTART_PRC has reached minimum! (no change)***" << endl;
					statHandler->C_MATSTART_PRC += increment;
				}
			}
			adjustParameters();
		}
		
		if (ch == 's') { //adjust DC_REW_FROM_BID_AGR_LEVEL
			increment = 1;
			cout << "DC_REW_FROM_BID_AGR_LEVEL" << endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->DC_REW_FROM_BID_AGR_LEVEL += increment;
				if (statHandler->DC_REW_FROM_BID_AGR_LEVEL > (pow(2,BIDLEVELS_LENGTH)-1) ) {
					cout << "\n***DC_REW_FROM_BID_AGR_LEVEL has reached maximum! (no change)***" << endl;
					statHandler->DC_REW_FROM_BID_AGR_LEVEL -= increment;
				}
			} else {
				statHandler->DC_REW_FROM_BID_AGR_LEVEL -= increment;
				if (statHandler->DC_REW_FROM_BID_AGR_LEVEL < 0) {
					cout << "\n***DC_REW_FROM_BID_AGR_LEVEL has reached minimum! (no change)***" << endl;
					statHandler->DC_REW_FROM_BID_AGR_LEVEL += increment;
				}
			}
			adjustParameters();
		}
		if (ch == 'd') { //adjust MAX_INACTIVE_SESS
			increment = 1;
			cout << "MAX_INACTIVE_SESS" << endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->MAX_INACTIVE_SESS += increment;
				if (statHandler->MAX_INACTIVE_SESS > 20 ) {
					cout << "\n***MAX_INACTIVE_SESS has reached maximum! (no change)***" << endl;
					statHandler->MAX_INACTIVE_SESS -= increment;
				}
			} else {
				statHandler->MAX_INACTIVE_SESS -= increment;
				if (statHandler->MAX_INACTIVE_SESS < 0) {
					cout << "\n***MAX_INACTIVE_SESS has reached minimum! (no change)***" << endl;
					statHandler->MAX_INACTIVE_SESS += increment;
				}
			}
			adjustParameters();
		}

		if (ch == 'f') { //adjust MUTATION_RATE
			increment = 1;
			cout << "MUTATION_RATE" << endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->MUTATION_RATE += increment;
				if (statHandler->MUTATION_RATE > 1000 ) {
					cout << "\n***MUTATION_RATE has reached maximum! (no change)***" << endl;
					statHandler->MUTATION_RATE -= increment;
				}
			} else {
				statHandler->MUTATION_RATE -= increment;
				if (statHandler->MUTATION_RATE < 0) {
					cout << "\n***MUTATION_RATE has reached minimum! (no change)***" << endl;
					statHandler->MUTATION_RATE += increment;
				}
			}
			adjustParameters();
		}

		if (ch == 'k') { //adjust HIGH_MUT_AGENTS_PRC
			increment = 1;
			cout << "HIGH_MUT_AGENTS_PRC" << endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->HIGH_MUT_AGENTS_PRC += increment;
				if (statHandler->HIGH_MUT_AGENTS_PRC > 100 ) {
					cout << "\n***HIGH_MUT_AGENTS_PRC has reached maximum! (no change)***" << endl;
					statHandler->HIGH_MUT_AGENTS_PRC -= increment;
				}
			} else {
				statHandler->HIGH_MUT_AGENTS_PRC -= increment;
				if (statHandler->HIGH_MUT_AGENTS_PRC < 0) {
					cout << "\n***HIGH_MUT_AGENTS_PRC has reached minimum! (no change)***" << endl;
					statHandler->HIGH_MUT_AGENTS_PRC += increment;
				}
			}
			adjustParameters();
		}
		
		if (ch == 'g') { //adjust MIN_NUM_OF_AGENTS_IN_GRID
			increment = 100;
			cout << "MIN_NUM_OF_AGENTS_IN_GRID" << endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->MIN_NUM_OF_AGENTS_IN_GRID += increment;
				if (statHandler->MIN_NUM_OF_AGENTS_IN_GRID > 10000 ) {
					cout << "\n***MIN_NUM_OF_AGENTS_IN_GRID has reached maximum! (no change)***" << endl;
					statHandler->MIN_NUM_OF_AGENTS_IN_GRID -= increment;
				}
			} else {
				statHandler->MIN_NUM_OF_AGENTS_IN_GRID -= increment;
				if (statHandler->MIN_NUM_OF_AGENTS_IN_GRID < 0) {
					cout << "\n***MIN_NUM_OF_AGENTS_IN_GRID has reached minimum! (no change)***" << endl;
					statHandler->MIN_NUM_OF_AGENTS_IN_GRID += increment;
				}
			}
			adjustParameters();
		}

		if (ch == 'j') { //adjust IDEAL_NUM_OF_AGENTS_IN_GRID
			increment = 100;
			cout << "IDEAL_NUM_OF_AGENTS_IN_GRID" << endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->IDEAL_NUM_OF_AGENTS_IN_GRID += increment;
				if (statHandler->IDEAL_NUM_OF_AGENTS_IN_GRID > 10000 ) {
					cout << "\n***IDEAL_NUM_OF_AGENTS_IN_GRID has reached maximum! (no change)***" << endl;
					statHandler->IDEAL_NUM_OF_AGENTS_IN_GRID -= increment;
				}
			} else {
				statHandler->IDEAL_NUM_OF_AGENTS_IN_GRID -= increment;
				if (statHandler->IDEAL_NUM_OF_AGENTS_IN_GRID < 0) {
					cout << "\n***IDEAL_NUM_OF_AGENTS_IN_GRID has reached minimum! (no change)***" << endl;
					statHandler->IDEAL_NUM_OF_AGENTS_IN_GRID += increment;
				}
			}
			adjustParameters();
		}

		if (ch == 'h') { //adjust GRAVITY_CENTER
			increment = 5;
			cout << "GRAVITY_CENTER" << endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->GRAVITY_CENTER += increment;
				if (statHandler->GRAVITY_CENTER > 100 ) {
					cout << "\n***GRAVITY_CENTER has reached maximum! (no change)***" << endl;
					statHandler->GRAVITY_CENTER -= increment;
				}
			} else {
				statHandler->GRAVITY_CENTER -= increment;
				if (statHandler->GRAVITY_CENTER < 0) {
					cout << "\n***GRAVITY_CENTER has reached minimum! (no change)***" << endl;
					statHandler->GRAVITY_CENTER += increment;
				}
			}
			adjustParameters();
		}
		
		if (ch == 'l') { //adjust DC_EARLY_RESP_REW_I0_PRC
			increment = 5;
			cout << "DC_EARLY_RESP_REW_I0_PRC" << endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->DC_EARLY_RESP_REW_I0_PRC += increment;
				if (statHandler->DC_EARLY_RESP_REW_I0_PRC > 100 ) {
					cout << "\n***DC_EARLY_RESP_REW_I0_PRC has reached maximum! (no change)***" << endl;
					statHandler->DC_EARLY_RESP_REW_I0_PRC -= increment;
				} else {
					statHandler->DC_EARLY_RESP_REW_I1_PRC += increment;
					if (statHandler->DC_EARLY_RESP_REW_I1_PRC > 100)
						statHandler->DC_EARLY_RESP_REW_I1_PRC = 100;
					statHandler->DC_EARLY_RESP_REW_I2_PRC += increment;
					if (statHandler->DC_EARLY_RESP_REW_I2_PRC > 100)
						statHandler->DC_EARLY_RESP_REW_I2_PRC = 100;
				}
			} else {
				statHandler->DC_EARLY_RESP_REW_I0_PRC -= increment;
				if (statHandler->DC_EARLY_RESP_REW_I0_PRC < 0) {
					cout << "\n***DC_EARLY_RESP_REW_I0_PRC has reached minimum! (no change)***" << endl;
					statHandler->DC_EARLY_RESP_REW_I0_PRC += increment;
				} else {
					statHandler->DC_EARLY_RESP_REW_I1_PRC -= increment;
					if (statHandler->DC_EARLY_RESP_REW_I1_PRC < 0)
						statHandler->DC_EARLY_RESP_REW_I1_PRC = 0;
					statHandler->DC_EARLY_RESP_REW_I2_PRC -= increment;
					if (statHandler->DC_EARLY_RESP_REW_I2_PRC < 0)
						statHandler->DC_EARLY_RESP_REW_I2_PRC = 0;
				}

			}
			cout << "\n DC_EARLY_RESP_REW_I1_PRC: " << statHandler->DC_EARLY_RESP_REW_I1_PRC << endl;
			cout << "DC_EARLY_RESP_REW_I2_PRC: " << statHandler->DC_EARLY_RESP_REW_I2_PRC << endl;
			adjustParameters();
		}
		if (ch == 'o') { //adjust MAX_NUM_OF_OBS_INP
			increment = 1;
			cout << "MAX_NUM_OF_OBS_INP" << endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->MAX_NUM_OF_OBS_INP += increment;
				if (statHandler->MAX_NUM_OF_OBS_INP > 100 ) {
					cout << "\n***MAX_NUM_OF_OBS_INP has reached maximum! (no change)***" << endl;
					statHandler->MAX_NUM_OF_OBS_INP -= increment;
				}
			} else {
				statHandler->MAX_NUM_OF_OBS_INP -= increment;
				if (statHandler->MAX_NUM_OF_OBS_INP < 0) {
					cout << "\n***MAX_NUM_OF_OBS_INP has reached minimum! (no change)***" << endl;
					statHandler->MAX_NUM_OF_OBS_INP += increment;
				}
			}
			adjustParameters();
		}
		if (ch == 'p') { //adjust MAX_NUM_OF_OBS_EFF
			increment = 1;
			cout << "MAX_NUM_OF_OBS_EFF" << endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->MAX_NUM_OF_OBS_EFF += increment;
				if (statHandler->MAX_NUM_OF_OBS_EFF > 100 ) {
					cout << "\n***MAX_NUM_OF_OBS_EFF has reached maximum! (no change)***" << endl;
					statHandler->MAX_NUM_OF_OBS_EFF -= increment;
				}
			} else {
				statHandler->MAX_NUM_OF_OBS_EFF -= increment;
				if (statHandler->MAX_NUM_OF_OBS_EFF < 0) {
					cout << "\n***MAX_NUM_OF_OBS_EFF has reached minimum! (no change)***" << endl;
					statHandler->MAX_NUM_OF_OBS_EFF += increment;
				}
			}
			adjustParameters();
		}

	}
}

void SyncManager::pressSpaceWait() {
	int ch;
	cout << "press space..." << endl;
	while (ch != ' ') {
		ch = _getch();
	}
}
void SyncManager::quit() {
	statHandler->closeFiles();
	//stopSystem = true;
	exit(0);
}

void SyncManager::setDecCat(Observable* decCat) {
	decCategory = decCat;
}


int SyncManager::getRandNumBetwZeroAnd(int maximum) {
	int randomNum;
	if (maximum<65536)  //no overflow risk
		randomNum = (rand()*(maximum+1)) / RAND_MAX;
	else //large number: use other method
		randomNum = ( ((double)rand()) / ((double)RAND_MAX)  ) *maximum;
	if (randomNum<0) {
		cout << "ERROR: getRandomNum" << endl;
		while (true) {}
	}
	if (randomNum>maximum)
		randomNum = maximum; //happens when rand()=RAND_MAX (rarely)
	return randomNum;
}


void SyncManager::updateAllViews() {
	gridView->paintGrid();
	agentView->updateInfoAgents(cycleNum);
	trainingView->updateInfoTraining(sessionNum-1);
	varStatView->updateInfoVarStat();
	//updateGridView();
	//updateAgentView( cNum );
	//updateTrainingView( sNum );
}