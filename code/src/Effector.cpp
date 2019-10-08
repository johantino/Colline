// Effector.cpp: implementation of the Effector class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "Colline.h"
#include "Effector.h"
#include <iostream>
#include "Konst.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Effector::Effector(Environment* e,
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
		Message* effectorPart,
		DecisionCategory* myCat) : Agent(e, sh, cStamp, mGrid, mCell, TYPE_EFFECTOR, s, initFitn, currFitn, initAge, matStat, lastPrSess, appearanceMating, appearanceBusiness, agentPart)
{
	//statHandler->adjustAgents(1, TYPE_EFFECTOR);
	//-------------set DNA values:-----------------
	int msbPos=0; //DNA position counter
	if (effectorPart->getSize() != N_DNA_EFF) {
		std::cout << "ERROR: DNA bit length error effector" << std::endl;
		pressSpaceToQuit();
	}
	effectorDNA = effectorPart;
	appIdealBus1 = effectorDNA->readRange(msbPos,APPEAR_SIZE_BUS);	msbPos += APPEAR_SIZE_BUS; //FACTOR = ? 
	appPickiBus1 = effectorDNA->readRange(msbPos,APPEAR_SIZE_BUS);	msbPos += APPEAR_SIZE_BUS;
	appIdealBus2 = effectorDNA->readRange(msbPos,APPEAR_SIZE_BUS);	msbPos += APPEAR_SIZE_BUS;
	appPickiBus2 = effectorDNA->readRange(msbPos,APPEAR_SIZE_BUS);	msbPos += APPEAR_SIZE_BUS;
	typeOfProc = effectorDNA->readValue(msbPos,3);		msbPos += 3; //FACTOR = 8;

	//std::cout << "DEBUG: effector dna is " << effectorDNA->toStringBits( /*new Message(pow(2,N_DNA_EFF)-1,N_DNA_EFF)*/) << ", value is: " << effectorDNA->getValue() << std::endl;
	//std::cout << "appIdeal2 is: " << appIdealBus2->toStringBits() << " (12-17)" << std::endl;
	//std::cout << "appPicki2 is: " << appPickiBus2->toStringBits() << " (18-23)" << std::endl;
	//pressSpaceOrQuit();

	//---------test counter:-------------------
	if (msbPos != N_DNA_EFF) {
		std::cout << "ERROR: DNA bit count in Agent \n" << std::endl;
		pressSpaceToQuit();
	}
	//------set various attributs---------------------
	//vicinityBus = VICINITY_BUS_EFF;
	effCategory = myCat;
	//catLevel = findCategoryLevel( appearanceBusiness ); //the category level is a number that tells how well busApp match the ideal in DC (ASSUMED = [111...])
	//setBidPoF(BIDPOF_EFF);
	//------initialize various values------------------
	numOfObsAgents1 = 0;
	numOfObsAgents2 = 0;
	numOfObsAgentsBoth=0;
	//numOfObservedTotal  ; //is not equal to numOfObs1+numOfObs2 as some agents is 'obs by both inputs'
	//make boughtMessages point to a common empty message when not used
	EMPTYMESS = new Message(0,0);
	boughtMessageInp1 = EMPTYMESS;
	boughtMessageInp2 = EMPTYMESS;
	boughtMessageBoth = EMPTYMESS;
	bidCategory = CAT_NONE;
	lastBoughtInCycle = -1;
	lastBoughtFromAgent = -1;
	//Inform StatHandler:
	statHandler->updateStatAgentBorn(this); //nb! this is the last thing to do in constructor (observable->agent->type (agent created and update can be done))

}

Effector::~Effector()
{
	//std::cout << "debug: deletes allocated mem for effector" << std::endl;
	delete effectorDNA;
	delete appIdealBus1;
	delete appPickiBus1;
	delete appIdealBus2;
	delete appPickiBus2;
	delete EMPTYMESS;
	//std::cout << "...(ok)" << std::endl;
}

void Effector::TESTsetBoughtMess1(Message* m1) {
	boughtMessageInp1 = m1;
}

void Effector::TESTsetBoughtMess2(Message* m1) {
	boughtMessageInp2 = m1;
}

void Effector::TESTsetBoughtMessB(Message* m1) {
	boughtMessageBoth = m1;
}

int Effector::getProcType() {
	return typeOfProc;
}

void Effector::TESTsetProcType(int val) {
	typeOfProc = val;
}

Message* Effector::getTypeDNA() {
	return effectorDNA;
}

std::string Effector::toStringObserveBusApp() {
	std::string info;
	info = "\n| Observes (Input 1): ";
	info += appIdealBus1->toStringBits(appPickiBus1);
	info += "\n| Observes (Input 2): ";
	info += appIdealBus2->toStringBits(appPickiBus2);
	info += "\n| Agents observed: ";
	info += toStringAgentsObserved();
	return info;
}

std::string Effector::toStringProcType() {
	std::string info;
	if (typeOfProc == PROC_AND)
		info = "AND";
	else if (typeOfProc == PROC_OR)
		info = "OR";
	else if (typeOfProc == PROC_XOR)
		info = "XOR";
	else if (typeOfProc == PROC_NAND)
		info = "NAND";
	else if (typeOfProc == PROC_NOR)
		info = "NOR";
	else if (typeOfProc == PROC_XNOR)
		info ="XNOR";
	else if (typeOfProc == PROC_INA)
		info = "Input A";
	else if (typeOfProc == PROC_INB)
		info = "Input B";
	else 
		info = "ERROR: effector, tostringproctype";
	return info;
}

std::string Effector::toStringTypeSpec() {
	std::string info;
	char strLevel[2];
	_itoa(bidAgressionLevel, strLevel, 10);
	char strVic[2];
	_itoa(getVicinityBus(), strVic, 10);

	info = toStringObserveBusApp();
	info += "\n| Processing type: ";
	info += toStringProcType();
	info += "  Categori level: ";
	info += strLevel;
	info += "  Vicinity (bus): ";
	info += strVic;
	info += "\n| DNA: ";
	info += getAppearBusSmall()->toStringBits();
	info += " ";
	info += getAgentDNA()->toStringBits();
	info += " ";
	info += effectorDNA->toStringBits();
	return info;
}

//for 3 bit smallAppearance:
//level 0: 0***
//level 1: 10**
//level 2: 110*
//level 3: 1110
//level 4: 1111
/*int Effector::findCategoryLevel(Message* obsPart) {
	int numOfBits = obsPart->getSize();
	int level = 0;
	unsigned int bn = 0;
	while (bn < numOfBits) {
		if (obsPart->bitNum(bn))
			level++;
		else
			bn = numOfBits;
		bn++;
	}
	//std::cout << "DEBUG: EFFECTOR is born with decCat level: " << level << std::endl;
	return level;
}*/

/*int Effector::getCatLevel() {
	return catLevel;
}*/

int Effector::getNumOfTypeDNAbits() {
	return N_DNA_EFF;
}


/*int Effector::getVicinityBus() {
	return vicinityBus;
}*/

Observable* Effector::makeBaby(Environment* e, IdStamp* cStamp, Grid* collineGrid, GridCell* babyPosition, SyncManager* syncMan, int offspringFitness, Message* offspringAppMat, Message* offspringAppBusSmall, Message* offspringDNA_AG, Message* offspringDNA_EFF) {
	Observable* baby = new Effector(e,statHandler,cStamp,collineGrid, babyPosition, syncMan, offspringFitness, offspringFitness, 0, false, syncMan->getSessionNum(), offspringAppMat, offspringAppBusSmall, offspringDNA_AG, offspringDNA_EFF, effCategory);
	Effector* temp = (Effector*)baby;
	temp->setStatusForNextDrifter();
	return baby;
}

bool Effector::makeBusinessConnections() {
	//start test...
	if ((numOfObsAgents1 != 0) || (numOfObsAgents2 != 0)) {
		std::cout << "error: effector has not been observing anyone (was drifter)" << std::endl;
		pressSpaceToQuit();
	}//end test
	if (getCurrentNeighbours().empty())
		return false; //no neighbours
	CList<Observable*,Observable*>* legalBus = removeNonLegalBusiness(); //Removes drifters from neigbourhood list
	//std::cout << "num of legal bus subj: " << legalBus.size() << std::endl;
	if (legalBus->GetCount() == 0) {
		delete legalBus;
		return false; //no business in neighbourhood
	}
	CList<Observable*,Observable*>* agentsInput1 = new CList<Observable*,Observable*>;
	CList<Observable*,Observable*>* agentsInput2 = new CList<Observable*,Observable*>;
	CList<Observable*,Observable*>* agentsInputBoth = new CList<Observable*,Observable*>;
	int numOfSubj = legalBus->GetCount();
	int i;
	int cat;
	Observable* tempObs;
	for (i=0; i<numOfSubj; i++) {
		tempObs = legalBus->GetAt( legalBus->FindIndex(i));
		cat = findObserveCategory( tempObs );
		if (cat==CAT_INP1)
			agentsInput1->AddTail( tempObs );
		if (cat==CAT_INP2)
			agentsInput2->AddTail( tempObs );
		if (cat==CAT_BOTH)
			agentsInputBoth->AddTail( tempObs );
	}
	int onlyMatch1 = agentsInput1->GetCount();
	int onlyMatch2 = agentsInput2->GetCount();
	int numOfObsAgentsBoth = agentsInputBoth->GetCount();
	//std::cout << "Effector " << getId() << " observing at least two needed: size 1= " << size1 << "  ,size2= "<<size2<<"   ,sizeB= " << sizeBoth << std::endl;
	//Test if enough agents match:
	if ((onlyMatch1==0) && (onlyMatch2==0)) {
		if (numOfObsAgentsBoth<2) { // || MAX_NUM_OF_OBS_BOTH_EFF<2) {
			delete legalBus;
			delete agentsInput1;
			delete agentsInput2;
			delete agentsInputBoth;
			return false; //Not enough in neighbourhood
		}
	} else if ((onlyMatch1==0) || (onlyMatch2==0)) {
		if (numOfObsAgentsBoth < 1) { // || MAX_NUM_OF_OBS_BOTH_EFF<1) {
			delete legalBus;
			delete agentsInput1;
			delete agentsInput2;
			delete agentsInputBoth;
			return false;
		}
	}
	//Enough in nb; observe all - determine input1 or 2 when receiving
	if (onlyMatch1 > statHandler->MAX_NUM_OF_OBS_EFF) {
		onlyMatch1 = statHandler->MAX_NUM_OF_OBS_EFF; 
		agentsInput1 = shuffleAgentList(agentsInput1); //shuffle list since only some of them are observed
	}
	for (i=0; i<onlyMatch1;i++) {
		observe(agentsInput1->GetAt(agentsInput1->FindIndex(i)));
	}
	
	if (onlyMatch2 > statHandler->MAX_NUM_OF_OBS_EFF) {
		onlyMatch2 = statHandler->MAX_NUM_OF_OBS_EFF;
		agentsInput2 = shuffleAgentList(agentsInput2); //shuffle list since only some of them are observed
	}
	for (i=0; i<onlyMatch2;i++) {
		observe(agentsInput2->GetAt(agentsInput2->FindIndex(i)));
	}
	
	if (numOfObsAgentsBoth > (statHandler->MAX_NUM_OF_OBS_EFF - max(onlyMatch1, onlyMatch2))) {
		numOfObsAgentsBoth = (statHandler->MAX_NUM_OF_OBS_EFF - max(onlyMatch1, onlyMatch2));
		if (numOfObsAgentsBoth > 0)
			agentsInputBoth = shuffleAgentList(agentsInputBoth); //shuffle list since only some of them are observed
	}
	for (i=0; i<numOfObsAgentsBoth;i++) {
		observe(agentsInputBoth->GetAt(agentsInputBoth->FindIndex(i)));
	}

	//numOfObsAgents1 = onlyMatch1 + numThatMatchBoth; fix
	//numOfObsAgents2 = onlyMatch2 + numThatMatchBoth;
	//test...delete later..fix:
	if (numOfObsAgents1>statHandler->MAX_NUM_OF_OBS_EFF || numOfObsAgents2>statHandler->MAX_NUM_OF_OBS_EFF) {
		std::cout <<"error: makebus. effector" << "\n";
		pressSpaceToQuit();
	} //end test
	delete legalBus;
	delete agentsInput1;
	delete agentsInput2;
	delete agentsInputBoth;
	//std::cout << "DEBUG: Effector observes, numOfObs1=" << numOfObs1 << " , numOfObs2=" << numOfObs2 << ", numThatmBoth=" << numThatMatchBoth << std::endl;
	//pressSpaceOrQuit();
	return true;
}

bool Effector::checkForNewObserveConnection(Agent* newObserverInNB){
	if (agentIsObserved(newObserverInNB)) {
		std::cout << "error: 494 checkfornewobsconn... newAgent has just been drifter!" << std::endl;
		std::cout << "newObserverInNB: " << std::endl;
		std::cout << newObserverInNB->toString() << std::endl;
		std::cout << "agent who observe newInNB: " << std::endl;
		std::cout << toString();
		pressSpaceToQuit();
	}
	bool newInNBObserved = false;
	int cat = findObserveCategory( newObserverInNB);
	if (agentPosition->getDistance(newObserverInNB->getPosition()) <= vicinityBus) {//check that agent is within vicinity
		if (cat==CAT_INP1) {
			if (numOfObsAgents1 < statHandler->MAX_NUM_OF_OBS_EFF) {
				observe(newObserverInNB);
				newInNBObserved = true;
			}
		}
		else if (cat==CAT_INP2) {
			if (numOfObsAgents2 < statHandler->MAX_NUM_OF_OBS_EFF) {
				observe(newObserverInNB);
				newInNBObserved = true;
			}
		}
		else if (cat==CAT_BOTH) {
			if (numOfObsAgents1 < statHandler->MAX_NUM_OF_OBS_EFF && numOfObsAgents2 < statHandler->MAX_NUM_OF_OBS_EFF) {
				observe(newObserverInNB);
				newInNBObserved = true;
			}
		}
	}
	//debug
	/*if (newInNBObserved && newObserverInNB->getId()==137) {
		std::cout << "debug: effector " << getId() << " start observing 137 (new in nb), cat=" << cat << std::endl;
		pressSpaceOrQuit();
	}*/ //end debug
	return newInNBObserved;
}

CList<Observable*,Observable*>* Effector::removeNonLegalBusiness() {
	auto currNB = getCurrentNeighbours();
	CList<Observable*,Observable*>* legalBusiness = new CList<Observable*,Observable*>;

	for(auto subj : currNB) {
		if (!subj->isFountain()) { //Remove fountains
			if (subj->getStatusAndActive() == ST_OBSERVER) 
				legalBusiness->AddTail((Agent*)subj);
		}
	}
	return legalBusiness;
}


void Effector::observe(Observable* obj) { //fix, transfer in 'cat' as parameter
	//This  = EFFECTOR
	//- obj = effector
	//        addToObserversProc()
	//- obj = inpoder or collector
	//        addToObserversState()
	//Not interested in: fountains

	if (obj->isEffector())
		obj->addToProcObservers(this);
	else
		obj->addToStateObservers(this);
	addToAgentsObserved(obj);

	int cat = findObserveCategory(obj);
	if (cat == CAT_INP1)
		numOfObsAgents1++;
	else if (cat == CAT_INP2)
		numOfObsAgents2++;
	else if (cat == CAT_BOTH) {
		numOfObsAgents1++;
		numOfObsAgents2++;
		numOfObsAgentsBoth++;
	} else {
		std::cout << "error; observe effector" << std::endl;
		pressSpaceToQuit();
	}
	//debug:
	/*if (obj->getId()==137 && getId()==839) {
		std::cout << "debug: effector " << getId() << " start observing 137 (observe method)" << std::endl;
		pressSpaceOrQuit();
	}*/

}

int Effector::findObserveCategory(Observable* agent) {
	int category = CAT_NONE;
	Message* agentApp = agent->getAppearBusFull();
	if (appIdealBus1->isEqualTo(agentApp, appPickiBus1)) {     //appBusIdeal1 match subject
		if (!appIdealBus2->isEqualTo(agentApp, appPickiBus2)) //appBusIdeal2 do NOT subject
			category = CAT_INP1;
	}
	if (appIdealBus2->isEqualTo(agentApp, appPickiBus2)) {     //appBusIdeal2 match subject
		if (!appIdealBus1->isEqualTo(agentApp, appPickiBus1)) //appBusIdeal1 do NOT subject
			category = CAT_INP2;
	}
	if (appIdealBus1->isEqualTo(agentApp, appPickiBus1)) {     //appBusIdeal1 match subject
		if (appIdealBus2->isEqualTo(agentApp, appPickiBus2)) //appBusIdeal2 match subject
			category = CAT_BOTH;
	}
	/*if (category == CAT_BOTH) {
		std::cout << "Effector: findobservecategory, the cat = bOTH, because:" << std::endl;
		std::cout << "Effector observes (inp1): " << appIdealBus1->toStringBits(appPickiBus1) << std::endl;
		std::cout << "Effector observes (inp2): " << appIdealBus2->toStringBits(appPickiBus2) << std::endl;
		std::cout << "Agent appearance        : " << agentApp->toStringBits() << std::endl;
		std::cout << "agentApp == inp1?       : " << appIdealBus1->isEqualTo(agentApp, appPickiBus1) << std::endl;
		std::cout << "agentApp == inp2?       : " << appIdealBus2->isEqualTo(agentApp, appPickiBus2) << std::endl;
		pressSpaceOrQuit();
	}*/
	return category;
}

void Effector::turnFocusTo(Observable* seller) {
	if ((getStatusAndActive() == ST_FINISHED_THIS_CYCLE) || getStatus() == ST_PROCESSOR)
		return;
	if (!updateAge())
		return; //agent dies
	if (lastBoughtFromAgent==seller->getId() && lastBoughtInCycle==syncMan->getCycleNum()) { //if effector already has bought one mess from this seller in this cycle, message are the same as before: do not buy 
		std::cout << "ERROR: effector turnFocusTo, buy from same twice: Must be on obs list more than once (error)" << std::endl;
		pressSpaceToQuit();
	}
	//std::cout << "Effector " << getId() << " turns focus to " << seller->toStringType() << seller->getId() << std::endl;
	int sellerCat = findObserveCategory(seller);
	//std::cout << "seller cat is: " << (int)sellerCat << std::endl;
	//char numOfBidsMadeTC = numberOfBidsThisCycle();
	int numOfMissingInputs = 2 - getNumOfBoughtMess(); //legalBidsLeftTC = legalBidsLeftThisCycle( numOfBidsMadeTC ); //0, 1 or 2 legal bids left
	//std::cout << "legalBidsLeftTC: " << (int)legalBidsLeftTC << std::endl;
	if (numOfMissingInputs == 0) {
		std::cout << "\n \n ERROR: effector turnFocusTO ...this shouldnt happen\n" << std::endl;
		return; //dont bid: Has already all messages needed
	}
	if (numOfMissingInputs == 1) {
		int boughtMessCat = findBoughtMessCat(); //the previous bid can be (1) a message bought in earlier cycle or (2) a bid made in this cycle
		if (sellerCatOK(sellerCat, boughtMessCat)) {
			bidCategory = sellerCat;
			makeBidNoUpdate(seller);
			return;
		}
		else
			return; //dont bid: The input that the seller offer a message to is 'loaded'
	} else if (numOfMissingInputs != 2) {
		std::cout << "ERROR: Effector turnFocusTo error..." << std::endl;
		pressSpaceToQuit();
	}
	//MissingInputs = 2
	//std::cout << "***EFFECTOR: bid cat from " << seller->toStringType() << " is " << sellerCat << std::endl;
	bidCategory = sellerCat;
	makeBidUpdate(seller); //Needs both inputs: Make bid no matter what category
}

bool Effector::hasOneStateMessage() {
	if (getNumOfBoughtMess() == 1)
		return true;
	else if (getNumOfBoughtMess() == 0)
		return false;
	else {
		std::cout << "ERRROR: effector has one mess..." << std::endl;
		pressSpaceToQuit();
	}
	return false;
}

//NEEDED?
bool Effector::sellerCatOK(char sellerCat, char boughtMessCat) {
	if ((sellerCat == CAT_INP1) && (boughtMessCat != CAT_INP1))
		return true;
	else if ((sellerCat == CAT_INP2) && (boughtMessCat != CAT_INP2))
		return true;
	else if (sellerCat == CAT_BOTH)
		return true;
	
	return false;
}

//This method is called when effector alreade has _one_ message
int Effector::findBoughtMessCat() {
	int boughtMessCat;
	if (boughtMessageInp1->getSize() != 0)
		boughtMessCat = CAT_INP1;
	else if (boughtMessageInp2->getSize() != 0)
		boughtMessCat = CAT_INP2;
	else if (boughtMessageBoth->getSize() != 0) 
		boughtMessCat = CAT_BOTH;
	else {
		std::cout << "ERROR: effector findprevcat" << std::endl;
		pressSpaceToQuit();
	}
	return boughtMessCat;
}


int Effector::getNumOfBoughtMess() {
	int numOfBoughtMess = 0;
	if (boughtMessageInp1->getSize() != 0)
		numOfBoughtMess++;
	if (boughtMessageInp2->getSize() != 0)
		numOfBoughtMess++;
	if (boughtMessageBoth->getSize() != 0)
		numOfBoughtMess++;
	if (numOfBoughtMess > 2) {
		std::cout << "ERROR: effector legalBidsLeft" << std::endl;
		std::cout << "inp1 size: " << boughtMessageInp1->getSize() << std::endl;
		std::cout << "inp2 size: " << boughtMessageInp2->getSize() << std::endl;
		std::cout << "Both size: " << boughtMessageBoth->getSize() << std::endl;
		pressSpaceToQuit();
	}
	return numOfBoughtMess;
}



void Effector::observedAgentIsDrifting(Observable* agent) {
	//std::cout << "DEBUG: EFFOBSDRIF effector, method: observedAgentIsDrifting..." << std::endl;
	if ((getStatus() == ST_DRIFTER) || (getStatus() == ST_DEAD)) {
		//std::cout << "warning: 124 bus con should be removed, id=" << getId() << "leaving id=" << agent->getId() << std::endl;
		return; //no problem, already signed up to drift or die
	}
	removeFromAgentsObserved(agent);
	//std::cout << "DEBUG: EFFOBSDRIF " << getId() << "after remove from agents observing..." << std::endl;
	int cat = findObserveCategory(agent);
	//std::cout << "Effector: " << agent->toStringType() << " is drifting...<< std::endl;
	int neededMess = 2 - getNumOfBoughtMess();
	//std::cout << "DEBUG: EFFOBSDRIF number of needed mess: " << neededMess << std::endl;
	if (cat == CAT_INP1)
		numOfObsAgents1--;
	else if (cat == CAT_INP2)
		numOfObsAgents2--;
	else if (cat == CAT_BOTH) {
		numOfObsAgents1--;
		numOfObsAgents2--;
		numOfObsAgentsBoth--;
	} else
		std::cout << "ERROR in observed agent" << std::endl;
	//std::cout << "DEBUG: EFFOBSDRIF " << getId() << " (numOfObs1, numOfObs2, numThatMatchBoth) : " << numOfObs1 << " , " << numOfObs2 << " , " << numThatMatchBoth << std::endl;
	/*std::cout << "effector.numOfObs1 = " << numOfObs1 << std::endl;
	std::cout << "effector.numOfObs2 = " << numOfObs2 << std::endl;
	std::cout << "effector.numThatMatchBoth = " << numThatMatchBoth << std::endl;*/
	//Only set to drifter if misses input AND the status is not processor: if status is processor effector either waits to process
	//this cycle (and then drifter) or it is ready for proc. next cycle, i.e. DONT move agent
	if (getStatus() == ST_PROCESSOR)
		return;
	if (neededMess < 1) {
		std::cout << "ERROR, effector. observedAgentIsDrifting" << std::endl;
		pressSpaceToQuit();
	}
	//two situations: need 1 message or needs both mess
	if (neededMess == 1) {
		//std::cout << "neededMess = 1" << std::endl;
		int bmCat = findBoughtMessCat();
		if (bmCat == CAT_INP1) {
			if (numOfObsAgents2 > 0)
				return;
		} else if (bmCat == CAT_INP2) {
			if (numOfObsAgents1 > 0)
				return;
		} else if (bmCat == CAT_BOTH) {
			if (numOfObsAgents1>0 || numOfObsAgents2>0)
				return;
		} else
			std::cout << "ERROR, effector observedAgentisDrifting" << std::endl;
	} else if (neededMess == 2) {
		if (numOfObsAgentsBoth > 1)
			return;
		//numThatMatchBoth = 0 or 1
		//When numThatMatchboth are subtracted from numOfObs1 and numOfObs2, only one of them needs to be >0
		int test1 = numOfObsAgents1 - numOfObsAgentsBoth;
		int test2 = numOfObsAgents2 - numOfObsAgentsBoth;
		if (test1<0 || test2<0) {
			std::cout << "ERROR; effector, test1 test2" << std::endl;
			std::cout << "n1=" << numOfObsAgents1 << ", n2=" << numOfObsAgents2 << " , nboth=" << numOfObsAgentsBoth << std::endl;
			std::cout << "cat=" <<cat << std::endl;
			std::cout << "id=" << getId() << std::endl;
			std::cout << "numObserved: " << getNumOfPotentialSellers() << std::endl;
			pressSpaceToQuit();
		}
		if (test1>0 || test2>0)
			return;
	}
	//If method reaches this point _this_ needs input -> drift
	//std::cout << "Effector about to change status to drifter" << std::endl;
	//std::cout << "DEBUG: EFFOBSDRIF effector " << getId() << " changes to drifter" << std::endl;
	wasteMessages();
	//numOfObsAgents1 = 0;
	//numOfObsAgents2 = 0;
	//numOfObsAgentsBoth = 0;
	setStatusForNextDrifter();

}

void Effector::wasteMessages() {
	if (boughtMessageInp1->getSize() != 0) {
		statHandler->addToNumOfDiscUBits(1);
		delete boughtMessageInp1;
		boughtMessageInp1 = EMPTYMESS;
	}
	if (boughtMessageInp2->getSize() != 0) {
		statHandler->addToNumOfDiscUBits(1);
		delete boughtMessageInp2;
		boughtMessageInp2 = EMPTYMESS;
	}
	if (boughtMessageBoth->getSize() != 0) {
		statHandler->addToNumOfDiscUBits(1);
		delete boughtMessageBoth;
		boughtMessageBoth = EMPTYMESS;
	}
}

void Effector::process() {
	if (getStatus()!=ST_PROCESSOR) {
		std::cout << "ERROR: effector, process 334" << std::endl;
		pressSpaceOrQuit();
	}
	lastProcessedInSess = syncMan->getSessionNum();
	//first find the two input out of the three 'slots'
	Message* inpA;
	Message* inpB;
	Message* procResult;
	if (boughtMessageInp1->getSize() != 1) { //input1 empty
		inpA = boughtMessageBoth;
		inpB = boughtMessageInp2;
	} else if (boughtMessageInp2->getSize() != 1) { //input2 empty
		inpA = boughtMessageInp1;
		inpB = boughtMessageBoth;
	} else if (boughtMessageBoth->getSize() != 1) { //inputBoth empty
		inpA = boughtMessageInp1;
		inpB = boughtMessageInp2;
	} else {
		std::cout << "ERROR: effector process ...input slot error!" << std::endl;
	}
	//std::cout << "info: Effector " << getId() << " process...(result=";
	bool pResult = getProcResult(inpA, inpB, typeOfProc);
	//std::cout << pResult << ")" << std::endl;
	procResult = new Message(pResult,1);
	//statHandler->addToNumOfProcEffectors_total(); use other method (not total)
	if (syncMan->getCollectionIndex() != -1) //within reward period (collection time)
		statHandler->addToNumOfProcEffectors( bidAgressionLevel, syncMan->getCollectionIndex() ); //can not be here: 
	statHandler->addToNumOfProcessedBits(2);
	statHandler->addToNumOfProducedBits(1);
	//std::cout << "processing result: " << (LPCTSTR)procResult->toStringBits() << std::endl;
	effCategory->turnFocusTo(this); //collect bid from decCat (should be incl. in notifyObservers, fix)
	notifyObserversProc(); //collect bids from observers
	//std::cout << "INFO: Effector " << getId() << " bids gathered..." << std::endl;
	if (!sellMessage(procResult)) {
		//std::cout << "DEBUG: effector processing result not sold..." << std::endl;
		statHandler->addToNumOfDiscUBits(1);
		delete procResult; //to bad! message not sold
	}
	//std::cout << "(remem testIfreadytomate)" << std::endl;
	if (getFitness() < minBusinessFitness)
		setStatusForNextDead(); //agent has been involved in too many unprofitable businesses ...delete it
	else {
		testIfReadyToMate();
		//numOfObsAgents1=0;
		//numOfObsAgents2=0;
		//numOfObsAgentsBoth=0;
		setStatusForNextDrifter();
	}
	//std::cout << "INFO: Effector " << getId() << " end of processing..." << std::endl;
	//std::cout << toString() << std::endl;
}

bool Effector::getProcResult(Message* inpA, Message* inpB, int procType) {
	bool result;
	if (procType == PROC_AND)
		result = inpA->AND(inpB);
	else if (procType == PROC_OR)
		result = inpA->OR(inpB);
	else if (procType == PROC_XOR)
		result = inpA->XOR(inpB);
	else if (procType == PROC_NAND) {
		result = inpA->AND(inpB);
		result = !result;
	}
	else if (procType == PROC_NOR) {
		result = inpA->OR(inpB);
		result = !result;
	}
	else if (procType == PROC_XNOR) {
		result = inpA->XOR(inpB);
		result = !result;
	} else if (procType == PROC_INA) 
		result = inpA->getBoolValue();
	else if (procType == PROC_INB)
		result = inpB->getBoolValue();
	else {
		std::cout << "ERROR: effector getprocresult error" << std::endl;
		pressSpaceToQuit();
	}
	wasteMessages(); //deletes used messages
	if (getNumOfBoughtMess() != 0) {
		std::cout << "ERROR: getProcResult, effector. clearing failed!" << std::endl;
		pressSpaceToQuit();
	}
	return result;
}

/*void Effector::setNumOfObsAgentsToZero() {
	numOfObsAgents1 = 0;
	numOfObsAgents2 = 0;
	numOfObsAgentsBoth = 0;
}*/

int Effector::auctionWon(Observable* seller, Message* boughtMess) {
	//When a message is bought it must be put into the right 'slot': boughMessInp1, boughtMessInp2 or boughtMessBoth
	//not because that the order matters for the 2-input logic processing, but because the agent must keep track on what 
	//input it needs (or doesnt need) and therefor bids for (or doesnt bid for)
	
	int cat = findObserveCategory(seller); //change to cat = bidCategory after a few tests
	//std::cout << "****Effector wins auciton from " << seller->toStringType() << ", cat=" << cat << std::endl;
	if (cat != bidCategory) {
		std::cout << "ERROR: effector, auction won. cat error" << std::endl;
		pressSpaceToQuit();
	}
	//std::cout << "Effector " << getId() << " buys [" << boughtMess->toStringBits() << "] from " << seller->toStringType() << seller->getId();
	if (bidCategory == CAT_INP1) {
		if (boughtMessageInp1->getSize() != 0) {
			std::cout << "ERROR: effector auctionWon input category already 'booked'!!" << std::endl;
			pressSpaceToQuit();
		} else {
			//std::cout << " (INP1)" << std::endl;
			boughtMessageInp1 = boughtMess;
		}
	} else if (bidCategory == CAT_INP2) {
		if (boughtMessageInp2->getSize() != 0) {
			std::cout << "ERROR: effector auctionWon input category already 'booked'!!" << std::endl;
			pressSpaceToQuit();
		} else {
			//std::cout << " (INP2)" << std::endl;
			boughtMessageInp2 = boughtMess;
		}
	} else if (bidCategory == CAT_BOTH) {
		if (boughtMessageBoth->getSize() != 0) {
			if (boughtMessageInp1->getSize() != 0) { //boughtMessInp1 is by def. used when bMessBoth is taken (could just as well be inp2)
				std::cout << "ERROR: effector " << getId() << " auctionWon input category already 'booked'!! (inp1)" << std::endl;
				std::cout << "message: " << boughtMessageInp1->toString() << std::endl;
				pressSpaceToQuit();
			} else {
				//std::cout << " (INP1)" << std::endl;
				boughtMessageInp1 = boughtMess;
			}
		} else {
			//std::cout << " (BOTH)" << std::endl;
			boughtMessageBoth = boughtMess;
		}
	} else {
		std::cout << "ERROR: auction won wrong categori!" << std::endl;
		pressSpaceToQuit();
	}
	//removeFromAgentsObserved(seller); //when one mess are bought stop observing mess from this agent, since to different agent states must be compared

	if (getNumOfBoughtMess() == 2) {
		//std::cout << "DEBUG: effector " << getId() << " are processor next cycle" << std::endl;
		setStatusForNextProcessor();
	}
	// (else keep observer status)
	int priceForMess = getBid();
	setFitness( fitness - priceForMess ); //fitness -= priceForMess;
	//syncMan->adjustFitnessColline(-priceForMess, TYPE_EFFECTOR);
	//syncMan->adjustFitnessColline(priceForMess, seller->getType() );
	lastBoughtInCycle = syncMan->getCycleNum();
	lastBoughtFromAgent = seller->getId();
	return priceForMess;
}

Message* Effector::getAppIdealBus1() {
	return appIdealBus1;
}

Message* Effector::getAppPickiBus1() {
	return appPickiBus1;
}

Message* Effector::getAppIdealBus2() {
	return appIdealBus2;
}

Message* Effector::getAppPickiBus2() {
	return appPickiBus2;
}

int Effector::getNumOfObsAgents1() {
	return numOfObsAgents1;
}

int Effector::getNumOfObsAgents2() {
	return numOfObsAgents2;
}

int Effector::clearNumOfObservedAgents() {
	int rv = numOfObsAgents1 + numOfObsAgents2 - numOfObsAgentsBoth;
	numOfObsAgents1=0;
	numOfObsAgents2=0;
	numOfObsAgentsBoth=0;
	return rv;
}
