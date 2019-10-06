// Fountain.cpp: implementation of the Fountain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Colline.h"
#include "Fountain.h"
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

Fountain::Fountain(Environment* e,
				   StatisticsHandler* sh,
				   IdStamp* cStamp,
				Grid* cGrid,
				GridCell* pos,
				SyncManager* s,
				Message* appearanceBusiness,
				int lSlot
				) : Observable(e, cStamp, TYPE_FOUNTAIN, appearanceBusiness)
{
	collineGrid = cGrid;
	position = pos;
	position->placeOccupier(this);
	loadingSlot = lSlot; 
	statHandler = sh;
	syncMan = s;
	statHandler->accumulateBorn(); //the fountain is an 'imposter agent' ...move this function to adjAgents
	statHandler->adjustAgents(1, TYPE_FOUNTAIN);
	statHandler->adjustObservers( 1, getType() );
	syncMan->addFountain(this); //NOTE: the fountain is assigned the a pipeNum according to num in list
	//syncMan->addIdToIdTagList(getId());
	syncMan->addAgentToList( this );
	//Set status to observer
	
	setStatus( ST_OBSERVER ); //agentStatus = ST_OBSERVER;
	lastStatusUpdateInCycle = syncMan->getCycleNum();
	statHandler->changeGridView(this, position);
	//end set status
	
	//-------------initialize various var:-------------
	EMPTYMESS = new Message(0,0);
	messFromEnv = EMPTYMESS;
	//bidders = new CList<Observable*, Observable*>(10);
	hasBeenActiveInSession = true;
}

Fountain::~Fountain()
{
	//delete bidders;
	delete EMPTYMESS;
}

CString Fountain::toString() {
	CString info;
	char strId[7];
	_itoa(getId(), strId, 10);
	Message* appBus = getAppearBusFull();
	info = "------ Fountain info -----";
	info += "\n| Id: ";
	info += strId;
	info += "\n| Type: ";
	info += toStringType();
	info += "\n| Business app.: ";
	info += appBus->toStringBits();
	info += toStringObservedBy();
	info += "\n--------------------------";
	return info;
}

void Fountain::addToBidders(Observable* obsBidder) {
	//Agent* bidder = (Agent*)obsBidder;
	Observable* tempBidder;
	int newBid = obsBidder->getBid();
	int numOfBidders = bidders.GetCount();
	int i = 0;
	POSITION pos;
	bool inserted = false;
	while (i<numOfBidders && !inserted) {
		pos = bidders.FindIndex(i);
		tempBidder = bidders.GetAt( pos );
		if (newBid>tempBidder->getBid()) {
			bidders.InsertBefore(pos, obsBidder);
			inserted = true;
		}
		i++;
	}
	//cout << "after while in addTobidders...addTail now..." << endl;
	//pressSpaceOrQuit();
	if (!inserted) //add to end
		bidders.AddTail(obsBidder);
	//cout << "ok" << endl;
	if (bidders.GetCount() != numOfBidders+1) {
		cout << "ERROR: addTobidders...";
		pressSpaceToQuit();
	}
	
}

int Fountain::getLoadingSlot() {
	return loadingSlot;
}

GridCell* Fountain::getPosition() {
	return position;
}

int Fountain::getStatusAndActive() {
	if (lastStatusUpdateInCycle == syncMan->getCycleNum())
		return ST_FINISHED_THIS_CYCLE; //agent has been active this cycle and can not participate in (1) being observed (2) mating
	int stat = getStatus();
	return stat;
}

void Fountain::receiveMessage(Message* newMess) { //corresponds to auctionwon for agents
	//cout << "Fountain " << getId() << " receives " << newMess->toStringBits() << endl;
	if (messFromEnv->getSize() != 0) {
		cout << "ERROR: fountain, receivemess: already has message!" << endl;
		pressSpaceToQuit();
	}
	messFromEnv = newMess;
	setStatusForNextProcessor();
	//syncMan->numOfObserversSub();
	//syncMan->isProcessorNextCycle(this);
}

void Fountain::process() {
	//cout << "info: Fountain " << getId() << "'process'..." << endl;
	if (messFromEnv->getSize() == 0) {
		cout << "ERROR, fountain process" << endl;
		pressSpaceToQuit();
	}
	//Sell state (state is always true for fountains, but using effectors helps stabilizing the structure)
	/*Observable::notifyObserversState(); //collect bids (in bidders vector)
	Message* stateMess = new Message(true, 1);
	if (sellMessage(stateMess)) {
		cout << "ERROR: fountain proces";
		pressSpaceToQuit();
		syncMan->addToNumOfProducedBits( 1 );
	} else {
		delete stateMess;
	}*/

	//sell message from environment
	//cout << "fountain notifies obs" << endl;
	Observable::notifyObserversProc(); //collect bids from collectors/ inpoders
	//cout << "info: Fountain " << getId() << " all bids collected" << endl;
	statHandler->addToNumBitsFromEnv( messFromEnv->getSize() );
	if (sellMessage(messFromEnv)) {
		messFromEnv = EMPTYMESS; //let pointer point to empty message
	} else {
		statHandler->addToNumOfDiscUBits( messFromEnv->getSize() );
		delete messFromEnv; //waste message
		messFromEnv = EMPTYMESS;
	}
	//cout << "Debug: fountain set status to drifter" << endl;
	setStatusForNextDrifter(); 
	//syncMan->numOfObserversAdd();
}

void Fountain::drift() {
	setStatusForNextObserver();
}

bool Fountain::sellMessage(Message* mess) {
	if (bidders.GetCount() == 0) {
		//cout << "info: (fountain) no bidders...." << endl;
		return false;
	}
	else {
		//cout << "debug: fountain sells message" << endl;
		Agent* buyer = findHighestBidder();
		//cout << "debug: highest bidder found" << endl;
		int payment = buyer->auctionWon(this, mess);
		env->receiveFitness( payment );
		statHandler->adjustFitnessEnvironment( payment );
		bidders.RemoveAll();
	}
	return true;
}

//Only the highest bidding effector gets to buy message
/*bool Fountain::sellState(bool state) {
	int payment;
	//cout << "INFO: (sellstate) numOfBuyers: " << numOfBuyers << endl;
	for (int i=0; i<numOfBuyers; i++) {
		stateMess = new Message(state,1); 
		payment = bidders.at(i)->auctionWon(this, stateMess);
		env->receiveFitness( payment );
	}
	bidders.clear();
	//cout << "INFO: (sellstate) finished" << endl;
	return numOfBuyers;
}*/

//Move to observable?
Agent* Fountain::findHighestBidder() {
	int numOfBids = bidders.GetCount(); // bidders.size();
	if (numOfBids == 0) {
		cout << "ERROR: findhighestbidder assumes bidders!" << endl;
		pressSpaceToQuit();
	}
	Agent* maxBidder = (Agent*)bidders.GetAt( bidders.FindIndex(0) );
	int maxBid = maxBidder->getBid();
	//test that list is sorted
	/*Agent* tempBidder;
	for (int i=1; i<numOfBids; i++) {
		tempBidder = (Agent*)bidders.GetAt( bidders.FindIndex(i) );
		if (tempBidder->getBid() > maxBid) {
			cout << "ERROR: findHighestBider fountain" << endl;
			pressSpaceToQuit();
		}
	}*/
	//end test
	return maxBidder;
	/*int numOfBids = bidders.size();
	if (numOfBids == 0) {
		cout << "ERROR: findhighestbidder assumes bidders!" << endl;
		pressSpaceToQuit();
	}
	Agent* maxBidder = bidders.at(0);
	int maxBid = maxBidder->getBid();
	for (int i=1; i<numOfBids; i++) {
		if (bidders.at(i)->getBid() > maxBid) {
			maxBidder = bidders.at(i);
			maxBid = maxBidder->getBid();
		}
	}
	return maxBidder;*/
}

//Note: consider sharing status methods with agents (move to observable)
void Fountain::setStatusForNextProcessor() {
	if (getStatus() != ST_OBSERVER) {
		cout << "ERROR: fountain, set status processor: prev status must be observer" << endl;
		pressSpaceToQuit();
	}
	statHandler->adjustObservers(-1, getType() );
	statHandler->adjustProcessors(1, getType() );
	//syncMan->numOfObserversSub();
	syncMan->isProcessorNextCycle(this);
	setStatus( ST_PROCESSOR ); //agentStatus = ST_PROCESSOR;
	lastStatusUpdateInCycle = syncMan->getCycleNum();
	statHandler->changeGridView(this, position);
	hasBeenActiveInSession = true;
}

void Fountain::setStatusForNextObserver() {
	if (getStatus() == ST_DRIFTER)
		statHandler->adjustDrifters(-1, getType());
	else {
		cout << "ERROR: set to observer" << endl;
		pressSpaceToQuit();
	}
	statHandler->adjustObservers( 1, getType() );
	//syncMan->numOfObserversAdd();
	setStatus( ST_OBSERVER ); //agentStatus = ST_OBSERVER;
	lastStatusUpdateInCycle = syncMan->getCycleNum();
	statHandler->changeGridView(this, position);
}

//the fountains use the status drifter and the method 'drift' to copy agent behaviour (easier programming)
void Fountain::setStatusForNextDrifter() {
	//syncMan->removeIdFromHasNotBeenDrifterList(getId()); //should be temporary test
	if (getStatus() != ST_PROCESSOR) {
		cout << "ERROR: Fountain status shift to drifter" << endl;
		pressSpaceToQuit();//syncMan->numOfObserversSub();
	} else
		statHandler->adjustProcessors(-1, getType() );

	statHandler->adjustDrifters(1, getType() );
	syncMan->isDrifterNextCycle(this);
	setStatus( ST_DRIFTER );
	lastStatusUpdateInCycle = syncMan->getCycleNum();
	deleteBusinessConnections();
	statHandler->changeGridView(this, position);
	hasBeenActiveInSession = true;
}











