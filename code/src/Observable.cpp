// Observable.cpp: implementation of the Observable class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "Colline.h"
#include "Observable.h"
#include <iostream>
#include <conio.h> //for press key
#include "Konst.h"
#include "dCollector.h" //delete these when solution to overload problem found in 'setStatusToDrifter'
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

Observable::Observable()
{

}

Observable::Observable(Environment* e, /*StatisticsHandler* sh,*/ IdStamp* collStamp, int elementType, Message* appearanceBusiness)
{
	env = e;
	//statHandler = sh;
	type = elementType;
	collineStamp = collStamp;
	id = collineStamp->stamp();

	//-----------------------------
	//syncMan->addIdToIdTagList(id);	
	//--------set DNA attributs:------------------------
	int msbPos = 0;
	appearBusSmall = appearanceBusiness;
	if (type != TYPE_CATEGORY) {
		if (appearanceBusiness->getSize() != (APPEAR_SIZE_BUS - 2)) {
			std::cout << "ERROR: DNA bit length error observable " << appearanceBusiness->getSize() << " = " << (APPEAR_SIZE_BUS - 2) << std::endl;
			pressSpaceToQuit();
		}
		appearBusFull = new Message((int)type,2); //the first two bits in appearBus is linked to agentType
		appearBusFull->addToEnd( appearBusSmall->clone() );
	} else
		appearBusFull = new Message(0,0);

	//------Initialize various values: -----------------

	agentsObserved =  new CTypedPtrList<CObList, Observable*>; //new CList<Observable*, Observable*>;
	
	observersProc =   new CTypedPtrList<CObList, Observable*>; //CList<Observable*, Observable*>;  
	observersUnproc = new CTypedPtrList<CObList, Observable*>; //CList<Observable*, Observable*>;
	observersState =  new CTypedPtrList<CObList, Observable*>; //CList<Observable*, Observable*>;
	
}

Observable::~Observable()
{
	//std::cout << "debug: deletes allocated mem for observable" << std::endl;
	delete appearBusSmall;
	delete appearBusFull;
	delete agentsObserved;
	delete observersProc;
	delete observersUnproc;
	delete observersState;
	//std::cout << "ok, observable delete" << std::endl;
}

void Observable::pressSpaceToQuit() {
	int ch;
	std::cout << "press space..." << std::endl;
	while (ch != ' ') {
		ch = _getch();
	}
	exit(0);
}

void Observable::pressSpaceOrQuit() {
	int ch;
	std::cout << "press space or 'q' to quit..." << std::endl;
	while ((ch != ' ') && (ch != 'q')) {
		ch = _getch();
	}
	if (ch == 'q')
		exit(0);
}

void Observable::deleteBusinessConnections() {
	//debug start
	/*if (getId() == 137) {
		std::cout << "debug: agent 137 deletes all buscon" << std::endl;
		pressSpaceOrQuit();
	}*/ //end

	int numOfPotSell = getNumOfPotentialSellers();
	int numOfObs=0;
	if (getType() == TYPE_COLLECTOR)
		numOfObs = ((Collector*)this)->clearNumOfObservedAgents();
	else if (getType() == TYPE_INPODER)
		numOfObs = ((Inpoder*)this)->clearNumOfObservedAgents();
	else if (getType() == TYPE_EFFECTOR) {
		numOfObs = ((Effector*)this)->clearNumOfObservedAgents();
		/*if (numOfObs>2) {
			std::cout << "DEBUG: effector clears business connections" << std::endl;
			std::cout << "num of agents observed: " << numOfObs << std::endl;
			std::cout << toString() << std::endl;
			pressSpaceOrQuit();
		}*/
	}
	else  if (getType() != TYPE_FOUNTAIN) {
		std::cout << "ERROR: 590" << std::endl;
		pressSpaceToQuit();
	}
	if (numOfPotSell != numOfObs) {
		std::cout << "ERROR: deleteBusConn 101, " << std::endl;
		std::cout << "numOfPotsell: " << numOfPotSell << std::endl;
		std::cout << "numOfObsAgents: " << numOfObs << std::endl;
		std::cout << this->toString() << std::endl;
		pressSpaceOrQuit();
	}
	clearPotentialSellers();
	clearPotentialBuyers();
}

int Observable::getId() {
	return id;
}

/*Environment* Observable::getEnvironment() {
	return env;
}*/

int Observable::getType() {
	return type;
}

bool Observable::hasOneStateMessage() {
	std::cout << "ERROR: observable hasOne state mess.." << std::endl;
	pressSpaceToQuit();
	return false;
}

CString Observable::toStringAppBus() {
	CString info = appearBusFull->toStringBits();
	return info;
}

CString Observable::toStringType() {
	if (type == TYPE_FOUNTAIN)
		return "Fountain ";
	else if (type == TYPE_COLLECTOR)
		return "Collector ";
	else if (type == TYPE_INPODER)
		return "Inpoder ";
	else if (type == TYPE_EFFECTOR)
		return "Effector ";
	return "Error";
}

CString Observable::toStringId() {
	CString info;
	char strId[7];
	_itoa(getId(), strId, 10);
	info = strId;
	return info;
}

CString Observable::toStringObservedBy() {
	CString info;
	int i;
	int sizeProc = observersProc->GetCount();
	int sizeUnproc = observersUnproc->GetCount();
	int sizeState = observersState->GetCount();
	info = "\n| Processed   obs: ";
	if (sizeProc > 0) {
		for (i=0; i<sizeProc; i++) {
			info += observersProc->GetAt( observersProc->FindIndex(i))->toStringId();
			info += ", ";
		}
	} else
		info += "(none)";
	info += "\n| Unprocessed obs: ";
	if (sizeUnproc > 0) {
		for (i=0; i<sizeUnproc; i++) {
			info += observersUnproc->GetAt( observersUnproc->FindIndex(i))->toStringId();
			info += ", ";
		}
	} else
		info += "(none)";
	info += "\n| State       obs: ";
	if (sizeState > 0) {
		for (i=0; i<sizeState; i++) {
			info += observersState->GetAt( observersState->FindIndex(i))->toStringId();
			info += ", ";
		}
	} else
		info += "(none)";
	return info;
}

CString Observable::toStringAgentsObserved() {
	CString info;
	info = "\n| AgentsObserved: ";
	int numOfObserved = agentsObserved->GetCount();
	if (numOfObserved == 0)
		info += "(none)";
	else {
		for (int i=0; i<numOfObserved; i++) {
			info += agentsObserved->GetAt( agentsObserved->FindIndex(i))->toStringId();
			info += ",";
		}
	}
	info += "\n";
	return info;
}


int Observable::auctionWon(Observable* seller, Message* m) {
	std::cout << "ERROR: Observable, aucitonwon, method must be overloaded" << std::endl;
	pressSpaceToQuit();
	return 0;
}

int Observable::getBid() {
	std::cout << "ERROR: obs, getBid, must be overloaded" << std::endl;
	pressSpaceToQuit();
	return 0;
}

void Observable::notifyObserversProc() {
	notifyObservers( observersProc );
}

void Observable::notifyObserversState() {
	notifyObservers( observersState );
}

void Observable::notifyObserversUnproc() {
	notifyObservers( observersUnproc ); 
}

void Observable::notifyObservers(CTypedPtrList<CObList, Observable*>* /*CList<Observable*, Observable*>* */ listOfObs) {

	CTypedPtrList<CObList, Observable*>* tempCopy = new CTypedPtrList<CObList, Observable*>;
	tempCopy->AddTail( listOfObs ); //tempCopy stay unaffected if listOfObs changes due to 'turnFocusTo' (if observer dies)
	//std::cout << "DEBUG: ready to notify" << std::endl;
	int numOfObs = listOfObs->GetCount();
	//std::cout << "DEBUG: numOfObs " << numOfObs << std::endl;
	//Observable* t;
	for (int i=0; i<numOfObs; i++) {
		//t = tempCopy->GetAt( tempCopy->FindIndex(i));
		//std::cout << i << ". observer data is " << t->toString() << std::endl;
		//pressSpaceOrQuit();
		//t->turnFocusTo(this);
		tempCopy->GetAt( tempCopy->FindIndex(i))->turnFocusTo(this); //observers make bid
	}
	delete tempCopy;
}

CString Observable::toStringAgentSpec() {
	CString info = "error obs tostring";
	return info;
}

CString Observable::toStringTypeSpec() {
	CString info = "error obs tostring";
	return info;
}

CString Observable::toStringStatus() {
	CString info;
	if (getStatus() == ST_OBSERVER) 
		info = "OBSERVER";
	else if (getStatus() == ST_PROCESSOR)
		info = "PROCESSOR";
	else if (getStatus() == ST_DRIFTER)
		info = "DRIFTER";
	else
		std::cout << "ERROR: toStringStatus" ;
	if (getStatusAndActive() == ST_FINISHED_THIS_CYCLE)
		info += " (not active)";
	else 
		info += " (active)";
	return info;
}

CString Observable::toString() {
	//CString info = "error obs tostring";
	CString info;
	char strId[7];
	_itoa(id, strId, 10);
	
	//Message* appBus = getAppearBus();
	info = "-------- Agent info -------";
	info += "\n| Id: ";
	info += strId;
	info += "  Type: ";
	info += toStringType();
	info += "  Status: ";
	info += toStringStatus();
	info += "  Business app.: ";
	info += appearBusFull->toStringBits();
	info += toStringAgentSpec();
	info += toStringObservedBy();
	info += toStringAgentsObserved();
	info += toStringTypeSpec();
	/*info += "\n| Mating app   : ";
	info += appearMat->toStringBits();
	info += "\n| Fitness: ";
	info += strFit;
	info += toStringObserveBusApp();
	info += toStringObservedBy();
	info += "\n| Current status: ";
	info += toStringStatus();	*/
	info += "\n-------------------------";

	return info;
}


//agents also keep track on which agents is observed: Agent change to drifter when no agents is observed
void Observable::addToAgentsObserved(Observable* potentialSeller) {
	/*if ((getId()==112) && potentialSeller->getId()==213){
		std::cout << "agent 213 is added to agent 112 observe-list" << std::endl;
		pressSpaceOrQuit();
	}*/
	agentsObserved->AddTail(potentialSeller);
}

void Observable::removeFromAgentsObserved(Observable* leavingAgent) {
	//std::cout << "observer: " << (LPCTSTR)toString() << std::endl;
	//std::cout << "observed: " << (LPCTSTR)leavingAgent->toString() << std::endl;
	if (getStatus() == ST_DEAD) {
		if (agentsObserved->GetCount() != 0) {
			std::cout << "ERROR: observable removeFromAgents.. should be zero length" << std::endl;
			pressSpaceToQuit();
		}
		return; //the agents should already be deleted (in clear potentialSellers)
	}
	POSITION pos;
	pos = agentsObserved->Find(leavingAgent);
	if (pos == NULL) {
		std::cout << "ERROR: observable, removeFrom...agent not found" << std::endl;
		std::cout << "leavingAgent id=" << leavingAgent->getId() << ", type="<<getType()<< std::endl;
		std::cout << "this         id=" << getId() <<", type="<<getType()<<std::endl;
		pressSpaceToQuit();
	} else
		agentsObserved->RemoveAt(pos);
	/*if ((getId()==112) && leavingAgent->getId()==213){
		std::cout << "agent 213 has been removed from agent 112 observe-list" << std::endl;
		pressSpaceOrQuit();
	}*/
	
}

int Observable::getNumOfPotentialSellers() {
	int size = agentsObserved->GetCount();
	return size;
}

int Observable::getNumOfPotentialBuyers() {
	int numOfPotBuyers = 0;
	numOfPotBuyers += observersProc->GetCount();
	numOfPotBuyers += observersUnproc->GetCount();
	numOfPotBuyers += observersState->GetCount();
	return numOfPotBuyers;
}

//agents keep track on other agents observing themselve: Used when auction is announced
void Observable::addToProcObservers(Observable* observer) {
	observersProc->AddTail(observer);
}
//agents keep track on other agents observing themselve: Used when auction is announced
void Observable::addToUnprocObservers(Observable* observer) {
	observersUnproc->AddTail(observer);
}
//agents keep track on other agents observing themselve: Used when auction is announced
void Observable::addToStateObservers(Observable* observer) {
	//std::cout << "agent, type " << observer->getType() << " is added as state obs to agent type " << getType() << std::endl;
	observersState->AddTail(observer);
	/*if (observer->getId()==839 && getId()==137) {
		std::cout << "debug: effector 839 is added as stateobserver to agent " << getId() << std::endl;
		pressSpaceOrQuit();
	}*/
}

//NOTE: copy list before announcing, and use copied list to announce - announced agents removes themselves from orig list
//change to method with pointer passing?
void Observable::clearPotentialBuyers() {
	/*if (getId()==213){
		std::cout << "agent 213 clears pot buyers" << std::endl;
		pressSpaceOrQuit();
	}*/
	informAndClearObservers( observersProc );	
	informAndClearObservers( observersUnproc );
	informAndClearObservers( observersState ) ;
	//FIX: TEMPORARY TEST ..delete later:
	/*if (observersProc->GetCount() != 0) {
		std::cout << "ERROR: obs clearpotsell" << std::endl;
		pressSpaceToQuit();
	}
	if (observersUnproc->GetCount() != 0) {
		std::cout << "ERROR: obs clearpotsell" << std::endl;
		pressSpaceToQuit();
	}
	if (observersState->GetCount() != 0) {
		std::cout << "ERROR: obs clearpotsell" << std::endl;
		pressSpaceToQuit();
	}*/
	//END TEMP TEST
}

//Info: 'hey! one of your suppliers are leaving'
void Observable::informAndClearObservers(CTypedPtrList<CObList, Observable*>* list) {
	CTypedPtrList<CObList, Observable*>* tempCopy = new CTypedPtrList<CObList, Observable*>;
	tempCopy->AddTail( list ); //tempCopy stays unaffected if list change due to 'observedAgentIsDrifting'
	int listSize = tempCopy->GetCount();
	for (int i=0; i<listSize; i++) {
		tempCopy->GetAt( tempCopy->FindIndex(i) )->observedAgentIsDrifting(this);
	}
	list->RemoveAll();	
	delete tempCopy;
}

void Observable::clearPotentialSellers() {
	CTypedPtrList<CObList, Observable*>* tempCopy = new CTypedPtrList<CObList, Observable*>; //CList<Observable*, Observable*>* tempCopy = new CList<Observable*, Observable*>;
	Observable* potSeller;
	int listSize = agentsObserved->GetCount();
	tempCopy->AddTail( agentsObserved ); //tempCopy stays unaffected if agentsObserved change due to 'potentialBuyerIsDrifting'
	for (int i=0; i<listSize; i++) {
		potSeller = tempCopy->GetAt( tempCopy->FindIndex(i) );	
		potSeller->potentialBuyerIsDrifting(this);
	}
	agentsObserved->RemoveAll();
	//if (getType() == TYPE_EFFECTOR)
	//	setNumOfObsAgentsToZero();
	delete tempCopy;
}

/*void Observable::setNumOfObsAgentsToZero() {
	std::cout << "ERROR: should be overloaded, setnumtozero" << std::endl;
	pressSpaceToQuit();
}*/

void Observable::potentialBuyerIsDrifting(Observable* leavingAgent) {
	POSITION pos;
	pos = observersProc->Find(leavingAgent);
	if (pos == NULL) {
		pos = observersUnproc->Find(leavingAgent);
		if (pos == NULL) {
			pos = observersState->Find(leavingAgent);
			if (pos == NULL) {
				std::cout << "ERROR: observable, type " << type << ", id=" << getId() << ", potentialbuyer " << leavingAgent->getType() << ", id=" << leavingAgent->getId() << " ...agent not found!" << std::endl;
				pressSpaceToQuit();
			} else
				observersState->RemoveAt(pos);
		} else
			observersUnproc->RemoveAt(pos);
	} else
		observersProc->RemoveAt(pos);
}

bool Observable::agentIsObserved(Observable* nbAgent) {
	POSITION pos;
	pos = agentsObserved->Find(nbAgent);
	if (pos == NULL) {
		//std::cout << "debug: nbAgent is not observed, pos is NULL" << std::endl;
		return false;
	}
	else {
		//std::cout <<"debug: nbAgent is observed" << std::endl;
		//pressSpaceOrQuit();
		return true;
	}
	return false;
}

void Observable::turnFocusTo(Observable* changedObj) {
	//Should be placed in Agent.h (don't know how to use Agent in Observable without loop conflict)
	std::cout << "ERROR.Observable:turnFocusTo must be overloaded in derived classes!" << std::endl;
}

void Observable::observedAgentIsDrifting(Observable* obsAgent) {
	//Should be placed in Agent.h (only)
	std::cout << "ERROR.Observable: observedAgentIs ... must be overloaded in derived classes!" << std::endl;
	pressSpaceToQuit();
}

bool Observable::isFountain() {
	if (type == TYPE_FOUNTAIN)
		return true;
	return false;
}

bool Observable::isEffector() {
	if (type == TYPE_EFFECTOR)
		return true;
	return false;

}

bool Observable::isInpoder() {
	if (type == TYPE_INPODER)
		return true;
	return false;
}

bool Observable::isCollector() {
	if (type == TYPE_COLLECTOR)
		return true;
	return false;
}

int Observable::getStatus() {
	return agentStatus;
	/*std::cout << "ERROR: getStatus must be overloaded in derived classes!" << std::endl;
	pressSpaceToQuit();
	return 0;*/
}

void Observable::setStatus(int status) {
	agentStatus = status;
}

int Observable::getStatusAndActive() {
	std::cout << "ERROR: getStatus must be overloaded in derived classes!" << std::endl;
	pressSpaceToQuit();
	return 0;
}

Message* Observable::getAppearBusFull() {
	return appearBusFull;
}

Message* Observable::getAppearBusSmall() {
	return appearBusSmall;
}

void Observable::addToBidders(Observable* bidder) { //only one method in observable needed?
	std::cout << "ERROR: addTobidders must be overloaded in derived classes!!" << std::endl;
	pressSpaceToQuit();
}

void Observable::process() {
	std::cout << "ERROR: process, observable..." << std::endl;
	pressSpaceToQuit();
}

void Observable::drift() {
	std::cout << "ERROR: drift, observable" << std::endl;
	pressSpaceToQuit();
}

int Observable::clearNumOfObservedAgents() {
	std::cout << "error: 453 must be overloaded" << std::endl;
	pressSpaceToQuit();
	return 0;
}


