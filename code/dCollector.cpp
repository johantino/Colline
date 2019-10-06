// dCollector.cpp: implementation of the Collector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Colline.h"
#include "dCollector.h"
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

Collector::Collector(Environment* e,
					 StatisticsHandler* sh,
					 IdStamp* cStamp,
					 Grid* cGrid,
		  GridCell* agentPos,
		  SyncManager* s,
		  int initFitn,
		  int currFitn,
		  int initAge,
		  bool matStat,
		  int lastPrSess,
		  Message* appearanceMating,
		  Message* appearanceBusiness,
		  Message* agentPart,
		  Message* collectorPart) : Agent(e, sh, cStamp, cGrid, agentPos, TYPE_COLLECTOR, s, initFitn, currFitn, initAge, matStat, lastPrSess, appearanceMating, appearanceBusiness, agentPart)
{

	//statHandler->adjustAgents(1, TYPE_COLLECTOR);
	// ---------DNA values:--------------
	int msbPos = 0; //DNA position counter
	if (collectorPart->getSize() != N_DNA_COLL) {
		cout << "ERROR: DNA bit length error collector" << endl;
		pressSpaceToQuit();
	}
	collectorDNA = collectorPart;
	appIdealBus = collectorDNA->readRange(msbPos,APPEAR_SIZE_BUS);	msbPos += APPEAR_SIZE_BUS;
	appPickiBus = collectorDNA->readRange(msbPos,APPEAR_SIZE_BUS);	msbPos += APPEAR_SIZE_BUS;
	observesForProcessed = collectorDNA->bitNum(msbPos);		msbPos += 1; //FACTOR = 2
	int b_collThr = collectorDNA->readValue(msbPos, 1);			msbPos += 1; //FACTOR = 4 (3)
	int b_collMax = collectorDNA->readValue(msbPos, 1);			msbPos += 1; //FACTOR = 4 (3)
	//-----calculate DNA val:-----
	collectThreshold = 2*pow(2,b_collThr); //gives values of 2 or 4; (minimum size for auction)
	collectMax = 4 + (b_collMax*2); //pow(2,b_collMax+1);     //gives values of 4 or 6
	//-----test DNA counter:------
	if (msbPos != N_DNA_COLL) {
		cout << "ERROR: DNA bit count in collector \n" << endl;
		pressSpaceToQuit();
	}

	//----set variuous attributs:-------------
	//vicinityBus = VICINITY_BUS_COLL;
	//setBidPoF(BIDPOF_COLL);
	//----initialize various values: ------------
	numOfObsAgents = 0;
	//EMPTYMESS = new Message(0,0);
	collectedMessage = new Message(0,0); //EMPTYMESS;
	//Inform StatHandler:
	statHandler->updateStatAgentBorn(this); //nb! this is the last thing to do in constructor (observable->agent->type (agent created and update can be done))
}

Collector::~Collector()
{
	//cout << "debug: deletes allocated mem for collector" << endl;
	delete collectorDNA;
	delete appIdealBus;
	delete appPickiBus;
	//cout << "about to delete EMPTYMESS :";
	//cout << EMPTYMESS->toString() << endl;
	//delete EMPTYMESS;
	//cout << "...ok delete alloac coll" << endl;
}

/*Message* Collector::TESTgetAppIdealBus() {
	return appIdealBus;
}

Message* Collector::TESTgetAppPickiBus() {
	return appPickiBus;
}*/

/*int Collector::TESTgetCollThr() {
	return collectThreshold;
}

int Collector::TESTgetCollMax() {
	return collectMax;
}*/

void Collector::TESTsetBoughtMessage(Message* m) {
	boughtMessage = m;
}


CString Collector::toStringObserveBusApp() {
	CString info;
	info = "\n| Observes: ";
	info += appIdealBus->toStringBits(appPickiBus);
	if (observesForProcessed)
		info += " (P)";
	else 
		info += " (U)";
	return info;
}

CString Collector::toStringTypeSpec() {
	CString info;
	char strMax[6];
	_itoa(collectMax, strMax, 10);
	char strThr[6];
	_itoa(collectThreshold, strThr, 10);
	char strVic[2];
	_itoa(getVicinityBus(), strVic, 10);

	info = toStringObserveBusApp();
	info += "\n| Max message size: ";
	info += strMax;
	info += "  collect threshold: ";
	info += strThr;
	info += "  Vicinity(bus): ";
	info += strVic;
	info += "\n| DNA: ";
	info += getAppearBusSmall()->toStringBits();
	info += " ";
	info += getAgentDNA()->toStringBits();
	info += " ";
	info += collectorDNA->toStringBits();
	//dna
	return info;
}

int Collector::auctionWon(Observable* seller, Message* boughtMess) {
	//cout << "Collector " << getId() << " buys [" << boughtMess->toStringBits() << "] from " << seller->toStringType() << seller->getId() << endl;
	boughtMessage = boughtMess;
	setStatusForNextProcessor();
	int fitnToPay = getBid();
	setFitness(fitness - fitnToPay); //fitness -= fitnToPay;
	//syncMan->adjustFitnessColline(-fitnToPay, TYPE_COLLECTOR);
	//syncMan->adjustFitnessColline(fitnToPay, seller->getType() );
	return fitnToPay;
}

bool Collector::makeBusinessConnections( ) {
	//CList<Observable*,Observable*>*
	if (numOfObsAgents != 0) {
		cout << "ERROR: in collector makebus" << endl;
		cout <<"numOfObs: " << numOfObsAgents << endl;
		pressSpaceToQuit();
	}
	if (getCurrentNeighbours()->GetCount() == 0)
		return false; //no neighbours
	CList<Observable*,Observable*>* legalBus = removeNonLegalBusiness(); //Removes non-observers and collectors from list
	//cout << "num of legal bus subj: " << legalBus.size() << endl;
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
	int numOfBusConn = matchApp->GetCount();
	if (numOfBusConn > MAX_NUM_OF_OBS_COL) {
		numOfBusConn = MAX_NUM_OF_OBS_COL;
		matchApp = shuffleAgentList(matchApp); //shuffle list since not all are observed
	}

	for (int i=0; i < numOfBusConn; i++) {
		observe(matchApp->GetAt( matchApp->FindIndex(i)));
	}
	delete matchApp;
	return true;
}

bool Collector::checkForNewObserveConnection(Agent* newObserverInNB) {
	if (agentIsObserved(newObserverInNB)) {
		cout << "error: checkfornewobsconn... newAgent has just been drifter!" << endl;
		pressSpaceToQuit();
	}
	bool newInNBObserved = false;
	if (agentPosition->getDistance(newObserverInNB->getPosition()) <= vicinityBus) {//check that agent is within vicinity
		if (numOfObsAgents < MAX_NUM_OF_OBS_COL) {
			if (isOkBusApp(newObserverInNB)) {	
				observe(newObserverInNB);
				newInNBObserved = true;
			}
		}
	}
	return newInNBObserved;
}

CList<Observable*,Observable*>* Collector::removeNonLegalBusiness() {
	CList<Observable*,Observable*>* currNB = getCurrentNeighbours();
	CList<Observable*,Observable*>* legalBusiness = new CList<Observable*,Observable*>;
	Observable* subj;
	int nbSize = currNB->GetCount();
	for (int i=0; i<nbSize; i++) {
		subj = currNB->GetAt( currNB->FindIndex(i));
		if (!subj->isCollector()) { //Remove collectors
			if (subj->getStatusAndActive() == ST_OBSERVER) 
				legalBusiness->AddTail(subj);
		}
	}
	return legalBusiness;
}

void Collector::turnFocusTo(Observable* seller) {
	if ((getStatusAndActive() == ST_FINISHED_THIS_CYCLE) || getStatus() == ST_PROCESSOR)
		return; //dont bid if finished this cycle or is processor
	if (!updateAge())
		return; //agent dies
	
	makeBidUpdate(seller);
}

bool Collector::isOkBusApp(Observable* possNewBusConn) {
	if (appIdealBus->isEqualTo(possNewBusConn->getAppearBusFull(), appPickiBus))
		return true;
	else
		return false;
	return false;
}


CList<Observable*,Observable*>* Collector::findOkBusAppear(CList<Observable*,Observable*>* subj) {
	CList<Observable*,Observable*>* okAppear = new CList<Observable*,Observable*>;
	Observable* tempObservable;
	int numOfSubj = subj->GetCount();
	for (int i=0; i<numOfSubj; i++) {
		tempObservable = subj->GetAt(subj->FindIndex(i));
		if (isOkBusApp(tempObservable))
			okAppear->AddTail(tempObservable);
	}
	delete subj;
	return okAppear;
}

void Collector::observe(Observable* obj) {
	//This  = COLLECTOR
	//- obj = effector
	//        addToObserversProc();
	//- obj = fountain
	//        addToObserversProc();
	//- obj = inpoder && observesForProcessed=true
	//        addToObserversProc();
	//- obj = inpoder && observesForProcessed=false
	//        addToObserversUnproc();
	//Not interested in: Collectors
	if (obj->isInpoder() && !observesForProcessed)
		obj->addToUnprocObservers(this); //agents keep track on other agents observing themselve (for auctioning)
	else
		obj->addToProcObservers(this);
	addToAgentsObserved(obj);
	numOfObsAgents++;
}
		
void Collector::observedAgentIsDrifting(Observable* agent){
	//this have just been removed from agents observerlist
	if ((getStatus() == ST_DRIFTER) || (getStatus() == ST_DEAD)) {
		cout << "info: collector were drifting or dead when notified of drifting seller (ok?)" << endl;
		return;
	}
	numOfObsAgents--;
	removeFromAgentsObserved(agent);
	if (numOfObsAgents != getNumOfPotentialSellers()) {
		cout << "ERROR: collector, observedAgent... numOfObsAgents error" << endl;
		pressSpaceToQuit();
	}
	//only set to drifter if none is obs AND status is not processor: if agent is processor it either waits 
	//to process this cycle (and will be drifter afterwards) OR it has bought a mess this cycle and process next cycle
	if ((numOfObsAgents == 0) && (getStatus() != ST_PROCESSOR)) 
		setStatusForNextDrifter();
}

//TEST: test ok(1)
void Collector::process() {
	lastProcessedInSess = syncMan->getSessionNum();
	//cout << "info: Collector " << getId() << " process..." << endl;
	bool pState;
	collectedMessage->addToEnd(boughtMessage);
	//if (collectedMessage->getSize() > MAX_MSIZE) {
	if (collectedMessage->getSize() >= collectThreshold) {
		pState = true;
		Observable::notifyObserversProc(); //collect bids
		if (sellMessage(collectedMessage))
			collectedMessage = new Message(0,0); //Start over
		else { //test if message should be  discarded
			if (collectedMessage->getSize() > collectMax ) {
				//cout << "info: Collector " << getId() << " waste collected message..." << endl;
				statHandler->addToNumOfDiscUBits( collectedMessage->getSize() );
				//delete collectedMessage;
				//collectedMessage = EMPTYMESS;
				collectedMessage->clear(); //change to EMPTYMESS like in effector?
			}
		}
	}
	else {
		pState = false;
		//note: could try to sell unprocessed
	}
	//cout << "proces state: " << pState << endl;
	//cout << "coll mess: " << (LPCTSTR)collectedMessage->toStringBits() << endl;
	Observable::notifyObserversState(); //collect bids
	//cout << "info: Collector " << getId() << " bids gathered..." << endl;
	int numOfBuyers = sellState(pState);
	statHandler->addToNumOfProducedBits(numOfBuyers); //each buyer buys one bit
	//cout << "REMEM testIfReadyTOMate!" << endl;

	if (getFitness() < minBusinessFitness)
		setStatusForNextDead(); //agent has been involved in too many unprofitable businesses ...delete it
	else {
		testIfReadyToMate();
		//numOfObsAgents=0;
		setStatusForNextDrifter();
	}
	//cout << "info: Collector " << getId() << " processing finished." << endl;
}

Message* Collector::getTypeDNA() {
	return collectorDNA;
}

/*int Collector::getVicinityBus() {
	return vicinityBus;
}*/

Observable* Collector::makeBaby(Environment* e, IdStamp* cStamp, Grid* collineGrid, GridCell* babyPosition, SyncManager* syncMan, int offspringFitness, Message* offspringAppMat, Message* offspringAppBusSmall, Message* offspringDNA_AG, Message* offspringDNA_COLL) {
	Observable* baby = new Collector(e,statHandler,cStamp,collineGrid, babyPosition, syncMan, offspringFitness, offspringFitness, 0, false, syncMan->getSessionNum(), offspringAppMat, offspringAppBusSmall, offspringDNA_AG, offspringDNA_COLL);
	Collector* temp = (Collector*)baby;
	temp->setStatusForNextDrifter();
	return baby;
}

int Collector::getNumOfTypeDNAbits() {
	return N_DNA_COLL;
}

Message* Collector::getAppBusIdeal() {
	return appIdealBus;
}

Message* Collector::getAppBusPicki() {
	return appPickiBus;
}

int Collector::clearNumOfObservedAgents() {
	int rv = numOfObsAgents;
	numOfObsAgents=0;
	return rv;
}
