// Observable.cpp: implementation of the Observable class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "Colline.h"
#include "Observable.h"
#include "iostream.h"
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
			cout << "ERROR: DNA bit length error observable " << appearanceBusiness->getSize() << " = " << (APPEAR_SIZE_BUS - 2) << endl;
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
	//cout << "debug: deletes allocated mem for observable" << endl;
	delete appearBusSmall;
	delete appearBusFull;
	delete agentsObserved;
	delete observersProc;
	delete observersUnproc;
	delete observersState;
	//cout << "ok, observable delete" << endl;
}

void Observable::pressSpaceToQuit() {
	int ch;
	cout << "press space..." << endl;
	while (ch != ' ') {
		ch = _getch();
	}
	exit(0);
}

void Observable::pressSpaceOrQuit() {
	int ch;
	cout << "press space or 'q' to quit..." << endl;
	while ((ch != ' ') && (ch != 'q')) {
		ch = _getch();
	}
	if (ch == 'q')
		exit(0);
}

void Observable::deleteBusinessConnections() {
	//debug start
	/*if (getId() == 137) {
		cout << "debug: agent 137 deletes all buscon" << endl;
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
			cout << "DEBUG: effector clears business connections" << endl;
			cout << "num of agents observed: " << numOfObs << endl;
			cout << toString() << endl;
			pressSpaceOrQuit();
		}*/
	}
	else  if (getType() != TYPE_FOUNTAIN) {
		cout << "ERROR: 590" << endl;
		pressSpaceToQuit();
	}
	if (numOfPotSell != numOfObs) {
		cout << "ERROR: deleteBusConn 101, " << endl;
		cout << "numOfPotsell: " << numOfPotSell << endl;
		cout << "numOfObsAgents: " << numOfObs << endl;
		cout << this->toString() << endl;
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
	cout << "ERROR: observable hasOne state mess.." << endl;
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
	cout << "ERROR: Observable, aucitonwon, method must be overloaded" << endl;
	pressSpaceToQuit();
	return 0;
}

int Observable::getBid() {
	cout << "ERROR: obs, getBid, must be overloaded" << endl;
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
	//cout << "DEBUG: ready to notify" << endl;
	int numOfObs = listOfObs->GetCount();
	//cout << "DEBUG: numOfObs " << numOfObs << endl;
	//Observable* t;
	for (int i=0; i<numOfObs; i++) {
		//t = tempCopy->GetAt( tempCopy->FindIndex(i));
		//cout << i << ". observer data is " << t->toString() << endl;
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
		cout << "ERROR: toStringStatus" ;
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
		cout << "agent 213 is added to agent 112 observe-list" << endl;
		pressSpaceOrQuit();
	}*/
	agentsObserved->AddTail(potentialSeller);
}

void Observable::removeFromAgentsObserved(Observable* leavingAgent) {
	//cout << "observer: " << (LPCTSTR)toString() << endl;
	//cout << "observed: " << (LPCTSTR)leavingAgent->toString() << endl;
	if (getStatus() == ST_DEAD) {
		if (agentsObserved->GetCount() != 0) {
			cout << "ERROR: observable removeFromAgents.. should be zero length" << endl;
			pressSpaceToQuit();
		}
		return; //the agents should already be deleted (in clear potentialSellers)
	}
	POSITION pos;
	pos = agentsObserved->Find(leavingAgent);
	if (pos == NULL) {
		cout << "ERROR: observable, removeFrom...agent not found" << endl;
		cout << "leavingAgent id=" << leavingAgent->getId() << ", type="<<getType()<< endl;
		cout << "this         id=" << getId() <<", type="<<getType()<<endl;
		pressSpaceToQuit();
	} else
		agentsObserved->RemoveAt(pos);
	/*if ((getId()==112) && leavingAgent->getId()==213){
		cout << "agent 213 has been removed from agent 112 observe-list" << endl;
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
	//cout << "agent, type " << observer->getType() << " is added as state obs to agent type " << getType() << endl;
	observersState->AddTail(observer);
	/*if (observer->getId()==839 && getId()==137) {
		cout << "debug: effector 839 is added as stateobserver to agent " << getId() << endl;
		pressSpaceOrQuit();
	}*/
}

//NOTE: copy list before announcing, and use copied list to announce - announced agents removes themselves from orig list
//change to method with pointer passing?
void Observable::clearPotentialBuyers() {
	/*if (getId()==213){
		cout << "agent 213 clears pot buyers" << endl;
		pressSpaceOrQuit();
	}*/
	informAndClearObservers( observersProc );	
	informAndClearObservers( observersUnproc );
	informAndClearObservers( observersState ) ;
	//FIX: TEMPORARY TEST ..delete later:
	/*if (observersProc->GetCount() != 0) {
		cout << "ERROR: obs clearpotsell" << endl;
		pressSpaceToQuit();
	}
	if (observersUnproc->GetCount() != 0) {
		cout << "ERROR: obs clearpotsell" << endl;
		pressSpaceToQuit();
	}
	if (observersState->GetCount() != 0) {
		cout << "ERROR: obs clearpotsell" << endl;
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
	cout << "ERROR: should be overloaded, setnumtozero" << endl;
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
				cout << "ERROR: observable, type " << type << ", id=" << getId() << ", potentialbuyer " << leavingAgent->getType() << ", id=" << leavingAgent->getId() << " ...agent not found!" << endl;
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
		//cout << "debug: nbAgent is not observed, pos is NULL" << endl;
		return false;
	}
	else {
		//cout <<"debug: nbAgent is observed" << endl;
		//pressSpaceOrQuit();
		return true;
	}
	return false;
}

void Observable::turnFocusTo(Observable* changedObj) {
	//Should be placed in Agent.h (don't know how to use Agent in Observable without loop conflict)
	cout << "ERROR.Observable:turnFocusTo must be overloaded in derived classes!" << endl;
}

void Observable::observedAgentIsDrifting(Observable* obsAgent) {
	//Should be placed in Agent.h (only)
	cout << "ERROR.Observable: observedAgentIs ... must be overloaded in derived classes!" << endl;
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
	/*cout << "ERROR: getStatus must be overloaded in derived classes!" << endl;
	pressSpaceToQuit();
	return 0;*/
}

void Observable::setStatus(int status) {
	agentStatus = status;
}

int Observable::getStatusAndActive() {
	cout << "ERROR: getStatus must be overloaded in derived classes!" << endl;
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
	cout << "ERROR: addTobidders must be overloaded in derived classes!!" << endl;
	pressSpaceToQuit();
}

void Observable::process() {
	cout << "ERROR: process, observable..." << endl;
	pressSpaceToQuit();
}

void Observable::drift() {
	cout << "ERROR: drift, observable" << endl;
	pressSpaceToQuit();
}

int Observable::clearNumOfObservedAgents() {
	cout << "error: 453 must be overloaded" << endl;
	pressSpaceToQuit();
	return 0;
}


