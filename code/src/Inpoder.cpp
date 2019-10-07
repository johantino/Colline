// Inpoder.cpp: implementation of the Inpoder class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "Colline.h"
#include "Inpoder.h"
#include "iostream.h"
#include "Konst.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Inpoder::Inpoder(Environment* e,
				 StatisticsHandler* sh,
				 IdStamp* cStamp,
				 Grid* mGrid,
		GridCell* mCell,
		SyncManager* s,
		int initFitn,
		int currFitn,
		int initAge,
		bool matStat,
		int lastPrSess,
		Message* appearanceMating,
		Message* appearanceBusiness,
		Message* agentPart,
		Message* inpoderPart) : Agent(e, sh, cStamp, mGrid, mCell, TYPE_INPODER, s, initFitn, currFitn, initAge, matStat, lastPrSess, appearanceMating, appearanceBusiness, agentPart)
{
	//statHandler->adjustAgents(1, TYPE_INPODER);
	//--------- DNA determined attributs ---------
	int msbPos = 0; //DNA position counter
	if (inpoderPart->getSize() != N_DNA_INP) {
		cout << "ERROR: DNA bit length error inpoder" << endl;
		pressSpaceToQuit();
	}
	inpoderDNA = inpoderPart;
	appIdealBus = inpoderDNA->readRange(msbPos,APPEAR_SIZE_BUS);		msbPos += APPEAR_SIZE_BUS; 
	appPickiBus = inpoderDNA->readRange(msbPos,APPEAR_SIZE_BUS);		msbPos += APPEAR_SIZE_BUS;
	smallFood = inpoderDNA->bitNum(msbPos);				msbPos += 1; //FACTOR = 2
	if (smallFood) {
		msbPos++; //skip first bit in food range
		food = inpoderDNA->readRange(msbPos,1);			msbPos += 1; 
	} else {
		food = inpoderDNA->readRange(msbPos,2);			msbPos += 2; //FACTOR = 4 (2)
	}
	//----------test DNA counter:-----------------
	if (msbPos != N_DNA_INP) {
		cout << "ERROR: DNA bit count in inpoder \n" << endl;
		pressSpaceToQuit();
	}
	//----set various attributs--------
	//vicinityBus = VICINITY_BUS_INP;
	//setBidPoF(BIDPOF_INP);
	//----initialize various values----
	numOfObsAgents = 0;
	//lastBidInCycle = 0;
	//Inform StatHandler:
	statHandler->updateStatAgentBorn(this); //nb! this is the last thing to do in constructor (observable->agent->type (agent created and update can be done))
}

Inpoder::~Inpoder()
{
	//cout << "debug: deletes allocated mem for inpoder" << endl;
	delete inpoderDNA;
	delete food;
	delete appIdealBus;
	delete appPickiBus;
}

Message* Inpoder::getFood() {
	return food;
}

void Inpoder::TESTsetBoughtMessage(Message* m) {
	boughtMessage = m;
}

Message* Inpoder::getAppBusIdeal() {
	return appIdealBus;
}

Message* Inpoder::getAppBusPicki() {
	return appPickiBus;
}

bool Inpoder::getSmallFood() {
	return smallFood;
}

Message* Inpoder::getTypeDNA() {
	return inpoderDNA;
}

CString Inpoder::toStringObserveBusApp() {
	CString info;
	info = "\n| Obs. agents with app: ";
	info += appIdealBus->toStringBits(appPickiBus);
	info += "\n";
	return info;
}

CString Inpoder::toStringTypeSpec() {
	CString info;
	char strVic[2];
	_itoa(getVicinityBus(), strVic, 10);
	info = toStringObserveBusApp();
	info += "Eats: ";
	info += food->toStringBits();
	info += "  Vicinity (bus): ";
	info += strVic;
	info += "\n| DNA: ";
	info += getAppearBusSmall()->toStringBits();
	info += " ";
	info += getAgentDNA()->toStringBits();
	info += " ";
	info += inpoderDNA->toStringBits();
	return info;
}

/*int Inpoder::getVicinityBus() {
	return vicinityBus;
}*/


int Inpoder::getNumOfTypeDNAbits() {
	return N_DNA_INP;
}


Observable* Inpoder::makeBaby(Environment* e,IdStamp* cStamp, Grid* collineGrid, GridCell* babyPosition, SyncManager* syncMan, int offspringFitness, Message* offspringAppMat, Message* offspringAppBusSmall, Message* offspringDNA_AG, Message* offspringDNA_INP) {
	Observable* baby = new Inpoder(e,statHandler,cStamp,collineGrid,babyPosition,syncMan, offspringFitness, offspringFitness, 0, false, syncMan->getSessionNum(), offspringAppMat, offspringAppBusSmall, offspringDNA_AG, offspringDNA_INP);
	Inpoder* temp = (Inpoder*)baby;
	temp->setStatusForNextDrifter();
	return baby;
}

int Inpoder::auctionWon(Observable* seller, Message* boughtMess) {
	//cout << "Inpoder " << getId() << " buys [" << boughtMess->toStringBits() << "] from " << seller->toStringType() << seller->getId() << endl;
	boughtMessage = boughtMess;
	setStatusForNextProcessor();
	int fitnToPay = getBid();
	setFitness( fitness - fitnToPay); //fitness -= fitnToPay;
	//syncMan->adjustFitnessColline(-fitnToPay, TYPE_INPODER);
	//syncMan->adjustFitnessColline(fitnToPay, seller->getType() );
	return fitnToPay;
}


CList<Observable*,Observable*>* Inpoder::removeNonLegalBusiness() {
	CList<Observable*,Observable*>* legalBusiness = new CList<Observable*,Observable*>;
	CList<Observable*,Observable*>* currNB = getCurrentNeighbours();
	Observable* subj;
	int nbSize = currNB->GetCount();
	for (int i=0; i<nbSize; i++) {
		subj = currNB->GetAt(currNB->FindIndex(i));
		if (subj->isEffector() && INP_CAN_BUY_FROM_EFF && (subj->getStatusAndActive() == ST_OBSERVER))  //proceed if non-effector
			legalBusiness->AddTail(subj);
		else if (!subj->isEffector() && (subj->getStatusAndActive() == ST_OBSERVER)) //proceed if non-effector
			legalBusiness->AddTail(subj);
	}
	return legalBusiness;
}


void Inpoder::turnFocusTo(Observable* seller) {
	//cout << "inpoder turns focus to type " << seller->getType() << endl;
	if ((getStatusAndActive() == ST_FINISHED_THIS_CYCLE) || getStatus() == ST_PROCESSOR)
		return; //dont bid if finished this cycle or if processor
	if (!updateAge())
		return; //agent dies
	//cout << "inpoder make bid" << endl;
	makeBidUpdate(seller);
}


bool Inpoder::makeBusinessConnections() {
	if (numOfObsAgents != 0) {
		cout << "ERROR: in inpoder makebus" << endl;
		cout <<"numOfObs: " << numOfObsAgents << endl;
		cout << "id=" <<getId() << endl;
		cout << toString() << endl;
		pressSpaceToQuit();
	}
	if (getCurrentNeighbours()->GetCount() == 0)
		return false; //no neighbours
	CList<Observable*,Observable*>* legalBus = removeNonLegalBusiness(); //Removes non-observers from list...FIX use same pointer
	//cout << "(Inpoder) num of legal bus in nb: " << legalBus.size() << endl;
	if (legalBus->GetCount() == 0) {
		delete legalBus;
		return false; //no business in neighbourhood
	}
	CList<Observable*,Observable*>* matchApp = findOkBusAppear(legalBus);
	if (matchApp->GetCount() == 0) {
		delete matchApp;
		return false; //no business in neighbourhood
	}
	//Business in neighbourhood
	/*if (getId() == 317) {
		cout << "Inpoder 317 makes bus conn., status: " << endl;
		cout << toString() << endl;
	}*/
	int numOfBusConn = matchApp->GetCount();
	if (numOfBusConn > statHandler->MAX_NUM_OF_OBS_INP) {
		numOfBusConn = statHandler->MAX_NUM_OF_OBS_INP;
		matchApp = shuffleAgentList(matchApp); //shuffle list since not all are observed
	}

	for (int i=0; i<numOfBusConn; i++) {
		observe(matchApp->GetAt( matchApp->FindIndex(i)));
	}
	/*if (getId() == 317) {
		cout << "Inpoder 317 finished making " << numOfBusConn << " bus conn., status: " << endl;
		cout << toString() << endl;
	}*/
	delete matchApp;
	return true;
}

bool Inpoder::checkForNewObserveConnection(Agent* newObserverInNB) {
	if (agentIsObserved(newObserverInNB)) {
		cout << "error: 811 checkfornewobsconn... newAgent has just been drifter!" << endl;
		pressSpaceToQuit();
	}
	bool newInNBObserved = false;
	if (agentPosition->getDistance(newObserverInNB->getPosition()) <= vicinityBus) {//check that agent is within vicinity
		if (numOfObsAgents < statHandler->MAX_NUM_OF_OBS_INP) {
			if (isOkBusApp(newObserverInNB)) {	
				observe(newObserverInNB);
				newInNBObserved = true;
			}
		}
	}
	return newInNBObserved;
}

bool Inpoder::isOkBusApp(Observable* possNewBusConn) {
	if (appIdealBus->isEqualTo(possNewBusConn->getAppearBusFull(), appPickiBus))
		return true;
	else
		return false;
	return false;
}


CList<Observable*,Observable*>* Inpoder::findOkBusAppear(CList<Observable*,Observable*>* subj) {
	Observable* tempObservable;
	CList<Observable*,Observable*>* okAppear = new CList<Observable*,Observable*>;
	int numOfSubj = subj->GetCount();
	for (int i=0; i<numOfSubj; i++) {
		tempObservable = subj->GetAt(subj->FindIndex(i));
		//if (getId()==317)
		//	cout << "DEBUG: (findOkBusApp) now checking app of agent " << tempObservable->getId() << endl;
		if (isOkBusApp(tempObservable))
			okAppear->AddTail(tempObservable);
	}
	delete subj;
	return okAppear;
}

void Inpoder::observe(Observable* obj) {
	//This  = INPODER
	//- obj = inpoder
	//        addToObserversUnproc()
	//- obj = collector
	//        addToObserversProc()
	//- obj = fountain
	//        addToObserversProc()
	//Not interested in: effectors
	if (obj->getType() == TYPE_INPODER)
		obj->addToUnprocObservers(this);
	else
		obj->addToProcObservers(this);
	addToAgentsObserved(obj);
	numOfObsAgents++;
}

void Inpoder::observedAgentIsDrifting(Observable* subj) {
	if ((getStatus() == ST_DRIFTER) || (getStatus() == ST_DEAD)) {
		cout << "info: inpoder " << getId() << " were drifting or dead, " << getStatus() << ", when notified of drifting seller " << subj->getId() << " (ok?)" << endl;
		pressSpaceToQuit();
		return;
	}
	numOfObsAgents--;
	removeFromAgentsObserved(subj);
	if (numOfObsAgents != getNumOfPotentialSellers()) {
		cout << "ERROR: collector, observedAgent... numOfObsAgents error" << endl;
		pressSpaceToQuit();
	}
	if ((numOfObsAgents == 0) && getStatus() != ST_PROCESSOR) // (if status is processor: dontmove)
		setStatusForNextDrifter();
}

//see figure 8 in doc
//TEST: test ok (1)
void Inpoder::process() {
	if (getStatus()!=ST_PROCESSOR) {
		cout << "ERROR: inpoder, process 634" << endl;
		pressSpaceOrQuit();
	}
	lastProcessedInSess = syncMan->getSessionNum();
	//cout << "Inpoder " << getId() << " process..." << endl;
	Message* procMess;
	Message* unprocMess;
	bool pState;
	int startPos = boughtMessage->messageContained(food);
	if (startPos == -1) { //food not found	
		pState = false;
		procMess = boughtMessage; //note: do not delete boughtMessage (same object as the 'procMess' points to)
		unprocMess = new Message(0,0);
		//cout << "food not found..." << endl;
		//cout << "proc mess: " << (LPCTSTR)procMess->toStringBits() << endl;
		//cout << "unproc mess: " << (LPCTSTR)unprocMess->toStringBits() << endl;
	}
	else {//food found
		pState = true;
		procMess = boughtMessage->readLeftMost(startPos); //startPos = numOfbits searched/processed
		unprocMess = boughtMessage->readRightMost(boughtMessage->getSize() - startPos - food->getSize());
		//syncMan->addToNumOfProcessedBits( food->getSize() );
		statHandler->addToNumOfEatenBits( food->getSize() );
		//cout << "food found..." << endl;
		//cout << "proc mess: " << (LPCTSTR)procMess->toStringBits() << endl;
		//cout << "unproc mess: " << (LPCTSTR)unprocMess->toStringBits() << endl;
		delete boughtMessage; //free memory
	}
	//Announce auction and sell:
	//- sell pState
	//cout << "debug: aboutto notify for state" << endl;
	Observable::notifyObserversState(); //collect bids for pState (in bidders vector)
	//cout << "INFO: Inpoder " << getId() << " bids gathered..." << endl;
	int numOfBuyers = sellState(pState); //sell and clear bidders
	statHandler->updateAvgStateBuyers_inp(numOfBuyers);
	statHandler->addToNumOfProducedBits(numOfBuyers); //each buyer buys one bit
	//- sell proc message
	int sizeP = procMess->getSize();
	int sizeU = unprocMess->getSize();

	if ( sizeP > 0) {
		Observable::notifyObserversProc();
		if (!Agent::sellMessage(procMess)) {
			//cout << "processed message not sold...";
			statHandler->addToNumOfDiscPBits( procMess->getSize() );
			delete procMess;
			//cout << "(delete ok)" << endl;
		}
	}
	else
		delete procMess;
	//- sell unproc message
	if ( sizeU > 0) {
		Observable::notifyObserversUnproc();
		if (!sellMessage(unprocMess)) {
			//cout << "unprocessed message not sold...";
			statHandler->addToNumOfDiscUBits( unprocMess->getSize() );
			delete unprocMess;
			///cout << "(delete ok)" << endl;
		}
	}
	else
		delete unprocMess;

	if (getFitness() < minBusinessFitness)
		setStatusForNextDead(); //agent has been involved in too many unprofitable businesses ...delete it
	else {
		testIfReadyToMate();
		//numOfObsAgents=0;
		setStatusForNextDrifter();
	}
	//cout << "INFO: Inpoder " << getId() << " processing finished...data:" << endl;
	//cout << (LPCTSTR)toString() << endl;
}

int Inpoder::clearNumOfObservedAgents() {
	int rv = numOfObsAgents;
	numOfObsAgents = 0;
	return rv;
}






