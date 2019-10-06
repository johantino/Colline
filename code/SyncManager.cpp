// SyncManager.cpp: implementation of the SyncManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Colline.h"
#include "SyncManager.h"
#include "Agent.h"
#include "DecisionCategory.h"
#include "Fountain.h"
#include <iostream>
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
		std::cout << "ERROR: removeIdTag, id not in list" << std::endl;
		pressSpaceToQuit();
	}
	idTags_currentAgents->RemoveAt( pos );
}*/

void SyncManager::removeAgentFromList(Observable* deadAgent) {
	POSITION pos = list_currentAgents->Find( deadAgent);
	if (pos==NULL) {
		std::cout << "ERROR: removeIdTag, agent not in list" << std::endl;
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
	//std::cout << "Agent: " << ag->toStringId() << " dies in next cycle";
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
	std::cout << "num of drifters: " << nextDrifters->GetCount() << std::endl;
	nextStopSession = sessionNum+1;
	std::cout << " num of agents total: " << statHandler->getNumOfAgents() << std::endl;
	pressSpaceOrQuit();
	while (statHandler->getNumOfAgents() > NUM_OF_FOUNTAINS) {
		prepareForNextCycle();
		if (isLoadingFountains1st || isLoadingFountains2nd || isLoadingFountains3rd) 
			loadFountains( ); //1
		initiateProcessors(); //2
		//std::cout << "syncman: initiates drifters" << std::endl;
		initiateDrifters(); //3
		deleteDeadAgents(); //4
		updateAllViews();
	}
	std::cout << "All living agents are dead! ..." << std::endl;
	//statHandler->closeFiles();
	pressSpaceToQuit();

}

void SyncManager::prepareForNextCycle() {
	statHandler->saveCycleData(cycleNum); //very first cycle activity is to save data from the previous cycle
	//deleteDeadAgents(); //delete agents that are dead in this cycle
	cycleNum++;
	//std::cout << "\n*********************************************" << std::endl;
	//std::cout << "cycle num = " << cycleNum  << std::endl;
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
		std::cout << "Shaking frozen agents..." << std::endl;
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
	std::cout << "INFO: " << numOfFrozen << " agents were 'frozen' in this session, " << numOfShaken << " of them is now drifters " << std::endl;
}

void SyncManager::prepareForNextSession() {
	shakeFrozenAgents(SHAKE_PERCENT);
	setHasBeenDrifterFlags(); //set all flags to false
	//test num of agents with taglist:
	if (list_currentAgents->GetCount() != statHandler->getNumOfAgents()) {
		std::cout << "ERROR: idTag_list and numOfAgents mismatch!" << std::endl;
		std::cout << "list_currentAgents size: " << list_currentAgents->GetCount() << std::endl;
		std::cout << "numOfAgents:     " << statHandler->getNumOfAgents() << std::endl;
		pressSpaceToQuit();
	} //end test numOfAgents
	statHandler->saveSessionData();
	DecisionCategory* decCat= (DecisionCategory*)decCategory; //Not nice style
	//decCat->giveRemainCatBuffToMatBonusBuff(env->getLoadedCategory());
	std::cout << "Trainingsession number " << sessionNum << " has just ended" << std::endl;
	//std::cout << env->getLastMessageString() << std::endl; //  "\nMessage from env: " << (LPCTSTR)tempMess->toStringBits() << " , answer = " << env->getLastAnswer() << std::endl;
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
				std::cout << "ERROR: menu choice" << std::endl;
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
	//std::cout << "DEBUG: loading fountains..."<< std::endl;
	if (isLoadingFountains1st + isLoadingFountains2nd + isLoadingFountains3rd > 1) {
		std::cout << "ERROR: 1st,2nd or 3rd fountain inflow can not overlap!" << std::endl;
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
		std::cout << "ERROR: loadFountains" << std::endl;
		pressSpaceToQuit();
	}
	if (loadingSlot < 0 || loadingSlot > (LOADING_TIME-1)) {
		std::cout << "ERROR: loadingSlot error" << std::endl;
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
	//std::cout << "loading fountain " << fNum << std::endl;
	Fountain* currFountain = (Fountain*)fountains->GetAt( fountains->FindIndex(fNum)); //(Fountain*)fountains.at(fNum);
	//std::cout << "syncman: Fountain " << currFountain->getId() << " 'loaded' (processor next cycle)" << std::endl;
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
		//std::cout << "syncman: Initiates " << numOfProcessors << " processors" << std::endl;
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
	//std::cout << "syncman: Initiates " << numOfDrifters << " drifters" << std::endl;
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
		//std::cout << "syncman: Deletes " << numOfDead << " dead agents" << std::endl;
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
	std::cout << "press space..." << std::endl;
	while (ch != ' ') {
		ch = _getch();
	}
	quit();
}

void SyncManager::pressSpaceOrQuit() {
	int ch = 0;
	std::cout << "press space or 'q' to quit..." << std::endl;
	while ((ch != ' ') && (ch != 'q')) {
		ch = _getch();
	}
	if (ch == 'q')
		quit();
}

int SyncManager::press12345SaveOrQuit() {
	int ch = 0;
	std::cout << "Choice..." << std::endl;
	std::cout << " '1': run 1 session" << std::endl;
	std::cout << " '2': run 10 sessions" << std::endl;
	std::cout << " '3': run 100 sessions" << std::endl;
	std::cout << " '4': run 1000 sessions" << std::endl;
	std::cout << " 'a': adjust parameters" << std::endl;
	std::cout << " 's': close files and save population" << std::endl;
	std::cout << "  ....or 'Q' to quit" << std::endl;
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
	std::cout << "\n-----------------------------------------\nAdjustable parameters:" << std::endl;
	std::cout << " '1': MAX_STATE_BUYERS_INP        = " << statHandler->MAX_STATE_BUYERS_INP << std::endl;
	std::cout << " '4': BIDP_BASE_INP               = " << statHandler->BIDP_BASE_INP << std::endl;
	std::cout << " '5': BIDP_STEP_INP               = " << statHandler->BIDP_STEP_INP << " (" << pow(2,BIDLEVELS_LENGTH) << " steps)" << std::endl;
	std::cout << " '6': BIDP_BASE_EFF               = " << statHandler->BIDP_BASE_EFF << std::endl;
	std::cout << " '7': BIDP_STEP_EFF               = " << statHandler->BIDP_STEP_EFF << " (" << pow(2,BIDLEVELS_LENGTH) << " steps)" << std::endl;
	std::cout << " '8': SESSION_LENGTH              = " << statHandler->SESSION_LENGTH << std::endl;
	std::cout << " '9': AGENT_LIFETIME_MAX          = " << statHandler->AGENT_LIFETIME_MAX << " , (avg: " << (int)floor(statHandler->avgAgentAge) << ", %hitMax: " << (int)floor(statHandler->percThatReachMaxAge) << ")" << std::endl;
	std::cout << " 'z': COLL_MIN_PROCTIME           = " << statHandler->COLL_MIN_PROCTIME << std::endl;
	std::cout << " 'x': GRID_ZOOM                   = " << statHandler->GRID_ZOOM << std::endl;
	std::cout << " 'c': C_MATSTART_PRC              = " << statHandler->C_MATSTART_PRC << std::endl;
	std::cout << " 's': DC_REW_FROM_BID_AGR_LEVEL   = " << statHandler->DC_REW_FROM_BID_AGR_LEVEL << std::endl;
	std::cout << " 'd': MAX_INACTIVE_SESS           = " << statHandler->MAX_INACTIVE_SESS << std::endl;
	std::cout << " 'f': MUTATION_RATE               = " << statHandler->MUTATION_RATE << std::endl;
	std::cout << " 'k': HIGH_MUT_AGENTS_PRC         = " << statHandler->HIGH_MUT_AGENTS_PRC << std::endl;	
	std::cout << " 'g': MIN_NUM_OF_AGENTS_IN_GRID   = " << statHandler->MIN_NUM_OF_AGENTS_IN_GRID << std::endl;
	std::cout << " 'j': IDEAL_NUM_OF_AGENTS_IN_GRID = " << statHandler->IDEAL_NUM_OF_AGENTS_IN_GRID << std::endl;
	std::cout << " 'h': GRAVITY_CENTER              = " << statHandler->GRAVITY_CENTER << std::endl;
	std::cout << " 'l': DC_EARLY_RESP_REW_I0_PRC    = " << statHandler->DC_EARLY_RESP_REW_I0_PRC << std::endl;
	std::cout << " 'o': MAX_NUM_OF_OBS_INP          = " << statHandler->MAX_NUM_OF_OBS_INP << std::endl;
	std::cout << " 'p': MAX_NUM_OF_OBS_EFF(each inp)= " << statHandler->MAX_NUM_OF_OBS_EFF << std::endl;
	
	std::cout << " 'e': End adjusting" << std::endl;

	while ((ch != '1') && (ch != '4') && (ch != '5') && (ch != '6') && (ch != '7') && (ch != '8') && (ch != '9') && (ch != 'z') && (ch != 'x') && (ch != 'c') && (ch != 's') && (ch != 'd') && (ch != 'f') && (ch != 'g') && (ch != 'h') && (ch != 'j') && (ch != 'k') && (ch != 'l') && (ch != 'p') && (ch != 'o') && (ch != 'e') ) {
		ch = _getch();
	}
	if (ch!='e') {
		std::cout << "\nPress '+' or '-' to increase/decrease value of ";
		if (ch == '1') {
			increment = 1;
			std::cout << "MAX_STATE_BUYERS_INP" << std::endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+')
				statHandler->MAX_STATE_BUYERS_INP += increment;
			else {
				statHandler->MAX_STATE_BUYERS_INP -= increment;
				if (statHandler->MAX_STATE_BUYERS_INP < 0) {
					std::cout << "\n***MAX_STATE_BUYERS_INP has reached mimimum! (no change)***" << std::endl;
					statHandler->MAX_STATE_BUYERS_INP += increment;
				}
			}
			adjustParameters();
		}
		if (ch == '4') { // adjust BIDP_BASE_INP
			increment = 5;
			std::cout << "BIDP_BASE_INP" << std::endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->BIDP_BASE_INP += increment;
				if (statHandler->BIDP_BASE_INP +  statHandler->BIDP_STEP_INP * (pow(2,BIDLEVELS_LENGTH)-1) > 100 ) {
					std::cout << "\n***BIDP_BASE_INP has reached maximum! (no change)***" << std::endl;
					statHandler->BIDP_BASE_INP -= increment;
				}
			} else {
				statHandler->BIDP_BASE_INP -= increment;
				if (statHandler->BIDP_BASE_INP < 5) {
					std::cout << "\n***BIDP_BASE_INP has reached mimimum! (no change)***" << std::endl;
					statHandler->BIDP_BASE_INP += increment;
				}
			}
			adjustParameters();
		}
		if (ch == '5') { //adjust BIDP_STEP_INP
			increment = 1;
			std::cout << "BIDP_STEP_INP" << std::endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->BIDP_STEP_INP += increment;
				if (statHandler->BIDP_STEP_INP * (pow(2,BIDLEVELS_LENGTH)-1) + statHandler->BIDP_BASE_INP > 100 ) {
					std::cout << "\n***BIDP_STEP_INP has reached maximum! (no change)***" << std::endl;
					statHandler->BIDP_STEP_INP -= increment;
				}
			} else {
				statHandler->BIDP_STEP_INP -= increment;
				if (statHandler->BIDP_STEP_INP < 0) {
					std::cout << "\n***BIDP_STEP_INP has reached minimum! (no change)***" << std::endl;
					statHandler->BIDP_STEP_INP += increment;
				}
			}
			adjustParameters();
		}
		if (ch == '6') { // adjust BIDP_BASE_EFF
			increment = 2;
			std::cout << "BIDP_BASE_EFF" << std::endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->BIDP_BASE_EFF += increment;
				if (statHandler->BIDP_BASE_EFF +  statHandler->BIDP_STEP_EFF * (pow(2,BIDLEVELS_LENGTH)-1) > 100 ) {
					std::cout << "\n***BIDP_BASE_EFF has reached maximum! (no change)***" << std::endl;
					statHandler->BIDP_BASE_EFF -= increment;
				}
			} else {
				statHandler->BIDP_BASE_EFF -= increment;
				if (statHandler->BIDP_BASE_EFF < 5) {
					std::cout << "\n***BIDP_BASE_EFF has reached mimimum! (no change)***" << std::endl;
					statHandler->BIDP_BASE_EFF += increment;
				}
			}
			adjustParameters();
		}
		if (ch == '7') { //adjust BIDP_STEP_EFF
			increment = 1;
			std::cout << "BIDP_STEP_EFF" << std::endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->BIDP_STEP_EFF += increment;
				if (statHandler->BIDP_STEP_EFF * (pow(2,BIDLEVELS_LENGTH)-1) + statHandler->BIDP_BASE_EFF > 100 ) {
					std::cout << "\n***BIDP_STEP_EFF has reached maximum! (no change)***" << std::endl;
					statHandler->BIDP_STEP_EFF -= increment;
				}
			} else {
				statHandler->BIDP_STEP_EFF -= increment;
				if (statHandler->BIDP_STEP_EFF < 0) {
					std::cout << "\n***BIDP_STEP_EFF has reached minimum! (no change)***" << std::endl;
					statHandler->BIDP_STEP_EFF += increment;
				}
			}
			adjustParameters();
		}
		if (ch == '8') { //adjust SESSION_LENGTH
			increment = 1;
			std::cout << "SESSION_LENGTH" << std::endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->SESSION_LENGTH += increment;
			} else {
				statHandler->SESSION_LENGTH -= increment;
				if (statHandler->SESSION_LENGTH < (LOADING_TIME+1)) {
					std::cout << "\n***SESSION_LENGTH has reached minimum! (no change)***" << std::endl;
					statHandler->SESSION_LENGTH += increment;
				}
			}
			adjustParameters();
		}
		if (ch == '9') { //adjust AGENT_LIFETIME
			increment = statHandler->SESSION_LENGTH;
			std::cout << "AGENT_LIFETIME_MAX" << std::endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->AGENT_LIFETIME_MAX += increment;
				if (statHandler->AGENT_LIFETIME_MAX > (statHandler->SESSION_LENGTH*20)) {
					std::cout << "\n***AGENT_LIFETIME_MAX has reached maximum! (no change)***" << std::endl;
					statHandler->AGENT_LIFETIME_MAX -= increment;
				}

			} else {
				statHandler->AGENT_LIFETIME_MAX -= increment;
				if (statHandler->AGENT_LIFETIME_MAX < 10) {
					std::cout << "\n***AGENT_LIFETIME_MAX has reached minimum! (no change)***" << std::endl;
					statHandler->AGENT_LIFETIME_MAX += increment;
				}
			}
			adjustParameters();
		}
		
		if (ch == 'z') { //adjust COLL_MIN_PROCTIME
			increment = 1;
			std::cout << "COLL_MIN_PROCTIME" << std::endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->COLL_MIN_PROCTIME += increment;
				if (statHandler->COLL_MIN_PROCTIME > statHandler->SESSION_LENGTH-COLLECTION_TIME ) {
					std::cout << "\n***COLL_MIN_PROCTIME has reached maximum! (no change)***" << std::endl;
					statHandler->COLL_MIN_PROCTIME -= increment;
				}
				statHandler->COLL_MAX_PROCTIME = statHandler->COLL_MIN_PROCTIME + (COLLECTION_TIME-1);
			} else {
				statHandler->COLL_MIN_PROCTIME -= increment;
				if (statHandler->COLL_MIN_PROCTIME < 0) {
					std::cout << "\n***COLL_MIN_PROCTIME has reached minimum! (no change)***" << std::endl;
					statHandler->COLL_MIN_PROCTIME += increment;
				}
				statHandler->COLL_MAX_PROCTIME = statHandler->COLL_MIN_PROCTIME + (COLLECTION_TIME-1);
			}
			adjustParameters();
		}
		if (ch == 'x') { //adjust GRID_ZOOM
			increment = 1;
			std::cout << "GRID_ZOOM" << std::endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->GRID_ZOOM += increment;
				if (statHandler->GRID_ZOOM > 6 ) {
					std::cout << "\n***GRID_ZOOM has reached maximum! (no change)***" << std::endl;
					statHandler->GRID_ZOOM -= increment;
				}
			} else {
				statHandler->GRID_ZOOM -= increment;
				if (statHandler->GRID_ZOOM < 1) {
					std::cout << "\n***GRID_ZOOM has reached minimum! (no change)***" << std::endl;
					statHandler->GRID_ZOOM += increment;
				}
			}
			adjustParameters();
		}
		if (ch == 'c') { //adjust C_MATSTART_PRC
			increment = 10;
			std::cout << "C_MATSTART_PRC" << std::endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->C_MATSTART_PRC += increment;
				if (statHandler->C_MATSTART_PRC > 10000 ) {
					std::cout << "\n***C_MATSTART_PRC has reached maximum! (no change)***" << std::endl;
					statHandler->C_MATSTART_PRC -= increment;
				}
			} else {
				statHandler->C_MATSTART_PRC -= increment;
				if (statHandler->C_MATSTART_PRC < 101) {
					std::cout << "\n***C_MATSTART_PRC has reached minimum! (no change)***" << std::endl;
					statHandler->C_MATSTART_PRC += increment;
				}
			}
			adjustParameters();
		}
		
		if (ch == 's') { //adjust DC_REW_FROM_BID_AGR_LEVEL
			increment = 1;
			std::cout << "DC_REW_FROM_BID_AGR_LEVEL" << std::endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->DC_REW_FROM_BID_AGR_LEVEL += increment;
				if (statHandler->DC_REW_FROM_BID_AGR_LEVEL > (pow(2,BIDLEVELS_LENGTH)-1) ) {
					std::cout << "\n***DC_REW_FROM_BID_AGR_LEVEL has reached maximum! (no change)***" << std::endl;
					statHandler->DC_REW_FROM_BID_AGR_LEVEL -= increment;
				}
			} else {
				statHandler->DC_REW_FROM_BID_AGR_LEVEL -= increment;
				if (statHandler->DC_REW_FROM_BID_AGR_LEVEL < 0) {
					std::cout << "\n***DC_REW_FROM_BID_AGR_LEVEL has reached minimum! (no change)***" << std::endl;
					statHandler->DC_REW_FROM_BID_AGR_LEVEL += increment;
				}
			}
			adjustParameters();
		}
		if (ch == 'd') { //adjust MAX_INACTIVE_SESS
			increment = 1;
			std::cout << "MAX_INACTIVE_SESS" << std::endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->MAX_INACTIVE_SESS += increment;
				if (statHandler->MAX_INACTIVE_SESS > 20 ) {
					std::cout << "\n***MAX_INACTIVE_SESS has reached maximum! (no change)***" << std::endl;
					statHandler->MAX_INACTIVE_SESS -= increment;
				}
			} else {
				statHandler->MAX_INACTIVE_SESS -= increment;
				if (statHandler->MAX_INACTIVE_SESS < 0) {
					std::cout << "\n***MAX_INACTIVE_SESS has reached minimum! (no change)***" << std::endl;
					statHandler->MAX_INACTIVE_SESS += increment;
				}
			}
			adjustParameters();
		}

		if (ch == 'f') { //adjust MUTATION_RATE
			increment = 1;
			std::cout << "MUTATION_RATE" << std::endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->MUTATION_RATE += increment;
				if (statHandler->MUTATION_RATE > 1000 ) {
					std::cout << "\n***MUTATION_RATE has reached maximum! (no change)***" << std::endl;
					statHandler->MUTATION_RATE -= increment;
				}
			} else {
				statHandler->MUTATION_RATE -= increment;
				if (statHandler->MUTATION_RATE < 0) {
					std::cout << "\n***MUTATION_RATE has reached minimum! (no change)***" << std::endl;
					statHandler->MUTATION_RATE += increment;
				}
			}
			adjustParameters();
		}

		if (ch == 'k') { //adjust HIGH_MUT_AGENTS_PRC
			increment = 1;
			std::cout << "HIGH_MUT_AGENTS_PRC" << std::endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->HIGH_MUT_AGENTS_PRC += increment;
				if (statHandler->HIGH_MUT_AGENTS_PRC > 100 ) {
					std::cout << "\n***HIGH_MUT_AGENTS_PRC has reached maximum! (no change)***" << std::endl;
					statHandler->HIGH_MUT_AGENTS_PRC -= increment;
				}
			} else {
				statHandler->HIGH_MUT_AGENTS_PRC -= increment;
				if (statHandler->HIGH_MUT_AGENTS_PRC < 0) {
					std::cout << "\n***HIGH_MUT_AGENTS_PRC has reached minimum! (no change)***" << std::endl;
					statHandler->HIGH_MUT_AGENTS_PRC += increment;
				}
			}
			adjustParameters();
		}
		
		if (ch == 'g') { //adjust MIN_NUM_OF_AGENTS_IN_GRID
			increment = 100;
			std::cout << "MIN_NUM_OF_AGENTS_IN_GRID" << std::endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->MIN_NUM_OF_AGENTS_IN_GRID += increment;
				if (statHandler->MIN_NUM_OF_AGENTS_IN_GRID > 10000 ) {
					std::cout << "\n***MIN_NUM_OF_AGENTS_IN_GRID has reached maximum! (no change)***" << std::endl;
					statHandler->MIN_NUM_OF_AGENTS_IN_GRID -= increment;
				}
			} else {
				statHandler->MIN_NUM_OF_AGENTS_IN_GRID -= increment;
				if (statHandler->MIN_NUM_OF_AGENTS_IN_GRID < 0) {
					std::cout << "\n***MIN_NUM_OF_AGENTS_IN_GRID has reached minimum! (no change)***" << std::endl;
					statHandler->MIN_NUM_OF_AGENTS_IN_GRID += increment;
				}
			}
			adjustParameters();
		}

		if (ch == 'j') { //adjust IDEAL_NUM_OF_AGENTS_IN_GRID
			increment = 100;
			std::cout << "IDEAL_NUM_OF_AGENTS_IN_GRID" << std::endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->IDEAL_NUM_OF_AGENTS_IN_GRID += increment;
				if (statHandler->IDEAL_NUM_OF_AGENTS_IN_GRID > 10000 ) {
					std::cout << "\n***IDEAL_NUM_OF_AGENTS_IN_GRID has reached maximum! (no change)***" << std::endl;
					statHandler->IDEAL_NUM_OF_AGENTS_IN_GRID -= increment;
				}
			} else {
				statHandler->IDEAL_NUM_OF_AGENTS_IN_GRID -= increment;
				if (statHandler->IDEAL_NUM_OF_AGENTS_IN_GRID < 0) {
					std::cout << "\n***IDEAL_NUM_OF_AGENTS_IN_GRID has reached minimum! (no change)***" << std::endl;
					statHandler->IDEAL_NUM_OF_AGENTS_IN_GRID += increment;
				}
			}
			adjustParameters();
		}

		if (ch == 'h') { //adjust GRAVITY_CENTER
			increment = 5;
			std::cout << "GRAVITY_CENTER" << std::endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->GRAVITY_CENTER += increment;
				if (statHandler->GRAVITY_CENTER > 100 ) {
					std::cout << "\n***GRAVITY_CENTER has reached maximum! (no change)***" << std::endl;
					statHandler->GRAVITY_CENTER -= increment;
				}
			} else {
				statHandler->GRAVITY_CENTER -= increment;
				if (statHandler->GRAVITY_CENTER < 0) {
					std::cout << "\n***GRAVITY_CENTER has reached minimum! (no change)***" << std::endl;
					statHandler->GRAVITY_CENTER += increment;
				}
			}
			adjustParameters();
		}
		
		if (ch == 'l') { //adjust DC_EARLY_RESP_REW_I0_PRC
			increment = 5;
			std::cout << "DC_EARLY_RESP_REW_I0_PRC" << std::endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->DC_EARLY_RESP_REW_I0_PRC += increment;
				if (statHandler->DC_EARLY_RESP_REW_I0_PRC > 100 ) {
					std::cout << "\n***DC_EARLY_RESP_REW_I0_PRC has reached maximum! (no change)***" << std::endl;
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
					std::cout << "\n***DC_EARLY_RESP_REW_I0_PRC has reached minimum! (no change)***" << std::endl;
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
			std::cout << "\n DC_EARLY_RESP_REW_I1_PRC: " << statHandler->DC_EARLY_RESP_REW_I1_PRC << std::endl;
			std::cout << "DC_EARLY_RESP_REW_I2_PRC: " << statHandler->DC_EARLY_RESP_REW_I2_PRC << std::endl;
			adjustParameters();
		}
		if (ch == 'o') { //adjust MAX_NUM_OF_OBS_INP
			increment = 1;
			std::cout << "MAX_NUM_OF_OBS_INP" << std::endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->MAX_NUM_OF_OBS_INP += increment;
				if (statHandler->MAX_NUM_OF_OBS_INP > 100 ) {
					std::cout << "\n***MAX_NUM_OF_OBS_INP has reached maximum! (no change)***" << std::endl;
					statHandler->MAX_NUM_OF_OBS_INP -= increment;
				}
			} else {
				statHandler->MAX_NUM_OF_OBS_INP -= increment;
				if (statHandler->MAX_NUM_OF_OBS_INP < 0) {
					std::cout << "\n***MAX_NUM_OF_OBS_INP has reached minimum! (no change)***" << std::endl;
					statHandler->MAX_NUM_OF_OBS_INP += increment;
				}
			}
			adjustParameters();
		}
		if (ch == 'p') { //adjust MAX_NUM_OF_OBS_EFF
			increment = 1;
			std::cout << "MAX_NUM_OF_OBS_EFF" << std::endl;
			while ((ch!='+') && (ch!='-')) {
				ch =_getch();
			}
			if (ch=='+') {
				statHandler->MAX_NUM_OF_OBS_EFF += increment;
				if (statHandler->MAX_NUM_OF_OBS_EFF > 100 ) {
					std::cout << "\n***MAX_NUM_OF_OBS_EFF has reached maximum! (no change)***" << std::endl;
					statHandler->MAX_NUM_OF_OBS_EFF -= increment;
				}
			} else {
				statHandler->MAX_NUM_OF_OBS_EFF -= increment;
				if (statHandler->MAX_NUM_OF_OBS_EFF < 0) {
					std::cout << "\n***MAX_NUM_OF_OBS_EFF has reached minimum! (no change)***" << std::endl;
					statHandler->MAX_NUM_OF_OBS_EFF += increment;
				}
			}
			adjustParameters();
		}

	}
}

void SyncManager::pressSpaceWait() {
	int ch;
	std::cout << "press space..." << std::endl;
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
		std::cout << "ERROR: getRandomNum" << std::endl;
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