// Agent.cpp: implementation of the Agent class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "Colline.h"
#include "Agent.h"
#include <iostream>
#include "Konst.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//using namespace jf_w3_agent;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Agent::Agent() //default
{

}

Agent::Agent(Environment* e,
			 StatisticsHandler* sh,
			 IdStamp* cStamp,
			 Grid* cGrid,
			 GridCell* startingCell,
			 int agentType,
			 SyncManager* s,
			 int initFitn,
			 int currFitn,
			 int currAge,
			 bool matStatus,
			 int lastPrSess,
			 Message* appearanceMating,
			 Message* appearanceBusiness,
			 Message* agentPart
			 ) : Observable(e, cStamp, agentType, appearanceBusiness)
{
	statHandler = sh;
	collineGrid = cGrid;
	agentPosition = startingCell;
	agentPosition->placeOccupier(this);
	syncMan = s;
	//statHandler->accumulateBorn();
	syncMan->addAgentToList( this );
	//syncMan->addIdToIdTagList( getId() ); //is getId() ok to use already (observable constr. finished?)
	age = currAge; //is != 0 when restoring, otherwise 0
	bornInCycle = syncMan->getCycleNum() - age;
	initialFitness = initFitn;
	if (age == 0 && initialFitness != currFitn) {
		std::cout << "ERROR: newborn agents must have fitness=initialFitness" << std::endl;
		pressSpaceToQuit();
	}
	fitness = 0;
	setFitness( currFitn);
	readyToMate = matStatus;
	int msbPos = 0;
	if (agentPart->getSize() != N_DNA_AG) {
		std::cout << "ERROR: DNA bit length error agent" << std::endl;
		pressSpaceToQuit();
	}
	//------------- set DNA values:----------------
	agentDNA = agentPart;
	appearMat = appearanceMating;
	appIdealMat = agentDNA->readRange(msbPos, APPEAR_SIZE_MAT);		msbPos += APPEAR_SIZE_MAT;
	//statHandler->updateCurrentSpeciesCount(agentType, appearMat, appIdealMat, true);
	//agentSpeciesNum = statHandler->calcSpeciesNum(appearMat, appIdealMat);
	int b_vicBus = agentDNA->readValue(msbPos, VIC_BIT_LENGTH);		msbPos += VIC_BIT_LENGTH;
	if (getType()==TYPE_INPODER || getType()==TYPE_COLLECTOR)
		vicinityBus = b_vicBus+VIC_ADD_INP; //4 or 5
	else if (getType()==TYPE_EFFECTOR) {
		vicinityBus = b_vicBus+VIC_ADD_EFF; //5 or 6
	} else {
		std::cout <<"ERROR: agent constr vicinityBus" << std::endl;
		pressSpaceToQuit();
	}
	//unsigned int b_maxLT = agentDNA->readValue(msbPos,1);	msbPos += 1;
	maxLifeTime = statHandler->AGENT_LIFETIME_MAX;//200 + 100*b_maxLT;							//FACTOR = 4 (2)
	unsigned int b_bidPoF = agentDNA->readValue(msbPos,BIDLEVELS_LENGTH);	msbPos += BIDLEVELS_LENGTH;
	bidAgressionLevel = b_bidPoF; //0,1,2 or 3
	if (agentType == TYPE_COLLECTOR) {
		bidPoF = BIDP_BASE_COL + BIDP_STEP_COL*b_bidPoF;
	} else if (agentType == TYPE_INPODER) {
		//if (b_bidPoF<2)
		//	b_bidPoF += 4;
		bidPoF = statHandler->BIDP_BASE_INP + (statHandler->BIDP_STEP_INP)*b_bidPoF;
	} else if (agentType == TYPE_EFFECTOR) {		
		//if (b_bidPoF<1)
		//	b_bidPoF += 4;
		bidPoF = statHandler->BIDP_BASE_EFF + statHandler->BIDP_STEP_EFF*b_bidPoF;
	} else
		bidPoF = 0; //error
	
	//------------- end DNA values ---------------------

	if (msbPos != N_DNA_AG) {
		std::cout << "ERROR: DNA bit count in Agent \n" << std::endl;
		pressSpaceToQuit();
	}

	//----set variuous attributs:-------------
	// TEMPTEST speciesNum = statHandler->calcSpeciesNum( appearMat, appIdealMat);
	vicinityMat = VICINITY_MAT;
	matingThrStart = (initFitn * statHandler->C_MATSTART_PRC)/100;
	matingThrStop = (initFitn * C_MATSTOP_PRC)/100;
	if (getType()==TYPE_COLLECTOR)
		minBusinessFitness = initFitn * MIN_BUS_FITN_COL_PRC/100;
	else if (getType()==TYPE_INPODER)
		minBusinessFitness = initFitn * MIN_BUS_FITN_INP_PRC/100;
	else if (getType()==TYPE_EFFECTOR)
		minBusinessFitness = initFitn * MIN_BUS_FITN_EFF_PRC/100;
	else {
		std::cout << "ERROR: agent const minbusfitn" << std::endl;
		pressSpaceToQuit();
	}
	//------- initialize various values: -------
	currentNeighbours = new CList<Observable*, Observable*>(10);
	//bidders = new CList<Observable*, Observable*>(10);  	
//	numOfObsAgents = 0;
	numOfOffspring = 0; //use restored
	lastProcessedInSess = lastPrSess; //syncMan->getSessionNum();
	lastBidUpdateInCycle = -1; //Ok since: when restoring all agents start as drifters, so no bids have been made this cycle
	setStatus(ST_OBSERVER); //to initialize to something (before changing to drifter in syncMan)
	lastStatusUpdateInCycle = -5; //to initialize to something (before changing to drifter below)
	//statHandler->adjustObservers(1,getType()); //because 1 is subtracted in setToDrifter
	//setStatusForNextDrifter(); //all agents start out by being drifters;
	bid = 0;
	lastNeighboursUpdateCycle = -1;
	hasBeenActiveInSession = false;
	//--------------------------------------

}

Agent::~Agent()
{
	//std::cout << "info: deletes allocated mem for agent" << std::endl;
	delete agentDNA;
	delete appearMat;
	delete appIdealMat;
	//delete appPickiMat;
	delete currentNeighbours;
	//delete bidders;	
	/*agentPosition->freeCell();
	env->receiveFitness( fitness );
	statHandler->adjustFitnessEnvironment( fitness );
	setFitness(0);*/
	//syncMan->adjustFitnessColline(-fitness, getType() );
	//std::cout << "(ok) delete for agent" << std::endl;
}

unsigned int Agent::getMaxLifeTime() {
	return maxLifeTime;
}

CList<Observable*, Observable*>* Agent::getCurrentNeighbours() {
	if (syncMan->getCycleNum() != lastNeighboursUpdateCycle) {
		std::cout << "ERROR: neighbours not updated" << std::endl;
		pressSpaceToQuit();
	}
	return currentNeighbours;
}

int Agent::getLastProcessedInSession() {
	return lastProcessedInSess;
}

void Agent::setCurrentNeighbours( CList<Observable*, Observable*>* nb) {
	lastNeighboursUpdateCycle = syncMan->getCycleNum();
	delete currentNeighbours;
	currentNeighbours = nb;
}


//used on startup to prevent that all agents die in the same cycle
void Agent::TESTscatterMaxLifeTime(int change) {
	maxLifeTime += change;
}

void Agent::TESTsetFitness(int newValue) {
	fitness = newValue;
}

void Agent::TESTsetReadyToMate(bool val) {
	readyToMate = val;
}

int Agent::getInitialFitness() {
	return initialFitness;
}

/*void Agent::TESTsetStatus(char status) {
	agentStatus = status;
}*/

CString Agent::toStringObserveBusApp() {
	return "\n Error agent bus app \n";
}

void Agent::makeBidUpdate(Observable* seller) {
	//std::cout <<"debug: bid is made" << std::endl;
	bid = (fitness*bidPoF)/100;
	if (bid < MIN_BID)
		bid = MIN_BID;
	if (bid > fitness) {
		//std::cout << "WARNING: bid percent is more than 100% (set to 100%), adjust parameters?" << std::endl;
		bid = fitness;
	}
	//lastBidUpdateInCycle = syncMan->getCycleNum();
	//std::cout << "checking type" << std::endl;
	//int type = getType();
	//std::cout << "type found: " << type << std::endl;
	if (getType() == TYPE_EFFECTOR) {
		if (bid*2 > fitness)
			setStatusForNextDead();
		else
			seller->addToBidders(this);
	} else {
		if (bid > fitness)
			setStatusForNextDead();
		else
			seller->addToBidders(this);
	}
	//std::cout << "end checking type" << std::endl;
}

//Note consider 'bid-stick': remove when bidding, replace when auction result exist
//MOVE to observable? ..SKIP this method and let second effector bid be smaller (and use 'gentlemenagreement')
//this method is only used by effectors when buying second input (bid not updated)
void Agent::makeBidNoUpdate(Observable* seller) {
	if (getType() != TYPE_EFFECTOR) {
		std::cout << "ERROR: makebidnoupdate" << std::endl;
		pressSpaceToQuit();
	}
	/*if (lastBidUpdateInCycle != syncMan->getCycleNum()) {
		std::cout << "info: makeBidNoUpdate, " << std::endl;
		std::cout << "lastBidUpdateInCycle = " << lastBidUpdateInCycle << std::endl;
		std::cout << "This cycle: " << syncMan->getCycleNum() << std::endl;
		std::cout << "lastStatusUpdate: " << lastStatusUpdateInCycle << std::endl;
		pressSpaceOrQuit();
	}*/
	//std::cout << "DEBUG: effector makes second bid" << std::endl;
	//std::cout << toString() << std::endl;
	//pressSpaceOrQuit();
	seller->addToBidders(this);
}


/*CString Agent::toStringStatus() {
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
}*/

void Agent::setFitness(int value) {
	int change = value - fitness;
	fitness = value;
	statHandler->adjustFitnessColline(change, getType());
}

CString Agent::toStringAge() {
	CString info;
	char strAge[6];
	_itoa(age, strAge, 10);
	char strMax[6];
	_itoa(maxLifeTime, strMax, 10);
	info = strAge;
	info += " (";
	info += strMax;
	info += ")";
	return info;
}	

CString Agent::toStringAgentSpec() {
	CString info;
	char strFit[6];
	_itoa(fitness, strFit, 10);
	char strVicm[2];
	_itoa(vicinityMat, strVicm, 10);


	info ="\n| Age: ";
	info += toStringAge();
	info += "  Fitness: ";
	info += strFit;
	info += "  Vicinity (mating): ";
	info += strVicm;
	info += "\n| Mating app: ";
	info += appearMat->toStringBits();
	info += "  Looks for (mating): ";
	info += appIdealMat->toStringBits();//appPickiMat);
	return info;
}

//NOTE: add 'toStringTypeDetails' where every info needed about the spec types are returned
/*CString Agent::toString() {
	CString info;
	char strFit[6];
	_itoa(fitness, strFit, 10);
	char strId[7];
	_itoa(getId(), strId, 10);
	Message* appBus = getAppearBus();
	info = "------ Agent info -----";
	info += "\n| Id: ";
	info += strId;
	info += "\n| Type: ";
	info += toStringType();
	info += "\n| Age: ";
	info += toStringAge();
	info += "\n| Business app.: ";
	info += appBus->toStringBits();
	info += "\n| Mating app   : ";
	info += appearMat->toStringBits();
	info += "\n| Fitness: ";
	info += strFit;
	info += toStringObserveBusApp();
	info += toStringObservedBy();
	info += "\n| Current status: ";
	info += toStringStatus();	
	info += "\n-----------------------";
	return info;
}*/

void Agent::observe(Observable* obj) {
	//This  = EFFECTOR
	//- obj = effector
	//        addToObserversProc()
	//- obj = inpoder or collector
	//        addToObserversState()
	//Not interested in: fountains

	//This  = INPODER
	//- obj = collector
	//        addToObserversProc()
	//- obj = fountain
	//        addToObserversProc()
	//Not interested in: -

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

	std::cout << "ERROR.Agent: Observe must be overloaded in derived classes!" << std::endl;
	pressSpaceToQuit();
}

bool Agent::updateAge() {
	age = syncMan->getCycleNum() - bornInCycle;
	if (age > maxLifeTime) {
		setStatusForNextDead();
		return false;
	}
	return true;
}

//collectors and inpoders use this method, effectors use derived method
void Agent::turnFocusTo(Observable* seller) {
	std::cout << "ERROR: turnFocus to must be overloaded in derived classes!" << std::endl;
	pressSpaceToQuit();
}

int Agent::auctionWon(Observable* seller, Message* bMess) {
	std::cout << "ERROR: auctionwon must be overloaded in derived classes!" << std::endl;
	pressSpaceToQuit();
	return 0;
}

//when using getStatus() the const returned tells what the agent is _about_ to do in this cycle
int Agent::getStatusAndActive() {
	if (lastStatusUpdateInCycle == syncMan->getCycleNum())
		return ST_FINISHED_THIS_CYCLE; //agent has been active this cycle and can not participate in (1) being observed (2) mating
	int stat = getStatus();
	return stat;
}

/*char Agent::getStatus() {
	return agentStatus;
}*/

GridCell* Agent::getPosition() {
	return agentPosition;
}

int Agent::getBid() {
	return bid;
}

int Agent::getFitness() {
	return fitness;
}

/*void Agent::setBid(unsigned int nBid) {
	bid = nBid;
}*/

unsigned int Agent::getBidPoF() {
	return bidPoF;
}

int Agent::getNumOfOffspring() {
	return numOfOffspring;
}


//bids are sorted by bid-size
void Agent::addToBidders(Observable* bidder) {
	Observable* tempBidder;
	int newBid = bidder->getBid();
	//bool hasStateMess = bidder->hasOneStateMessage();
	int numOfBidders = bidders.GetCount();
	int i = 0;
	POSITION pos;
	bool inserted = false;
	while (i<numOfBidders && !inserted) {
		pos = bidders.FindIndex(i);
		tempBidder = bidders.GetAt( pos );
		if (newBid > tempBidder->getBid()) {
			bidders.InsertBefore(pos, bidder);
			inserted=true;
		}
		i++;
	}
	if (!inserted) //add to end
		bidders.AddTail(bidder);
	if (bidders.GetCount() != numOfBidders+1) {
		std::cout << "ERROR: addToBidders...";
		pressSpaceToQuit();
	}
	//bidders.AddTail(bidder);
	//bidders.push_back((Agent*)bidder);
}

bool Agent::hasOneStateMessage() {
	std::cout << "ERROR: agent hasOne state mess.." << std::endl;
	pressSpaceToQuit();
	return false;
}


//note the vicinity of this agent is used; maybe nbAgent has different vicinity... check for this in method
void Agent::informNeighbourhoodOfNewcomer() {
	if (NEW_OBSERVERS_NOTIFY) {
		/*if ((getId() == 317) || (getId()==1696) ) {
			std::cout << "agent " << getId() << ", type " << getType() << ", informs neigbbours of pot new business" << std::endl;
			std::cout << "agent " << getStatus() << " status before inform: " << std::endl;
			std::cout << toString() << std::endl;
		}*/
		CList<Observable*, Observable*>* currObsInNb = collineGrid->getObservers(agentPosition, vicinityBus, false);
		Agent* subj;
		int numOfNewConn = 0;
		int listSize = currObsInNb->GetCount();
		for (int i=0; i<listSize; i++) {
			subj = (Agent*)currObsInNb->GetAt(currObsInNb->FindIndex(i));
			if (subj->checkForNewObserveConnection(this))
				numOfNewConn++;
		}
		/*if ((getId() == 317) || (getId()==1696) ) {
			std::cout << "agent " << getStatus() << " status after inform: " << std::endl;
			std::cout << toString() << std::endl;
		}*/
		
	}
}

bool Agent::checkForNewObserveConnection(Agent* newObserverInNb) {
	std::cout << "ERROR: checkForNewObs.. must be defined in derived classes" << "\n";
	return false;
}

void Agent::setStatusForNextObserver() {
	/*if (getId() == 213) {
		std::cout << "debug: 213 ids observer in next cycle" << std::endl;
		pressSpaceOrQuit();
	}*/
	if (lastStatusUpdateInCycle == syncMan->getCycleNum()) {
		std::cout << "ERROR: status already updated in this cycle, loop problem? [observer]" << std::endl;
		pressSpaceToQuit();
	}
	if (getStatus() == ST_DRIFTER)
		statHandler->adjustDrifters(-1, getType());
	else {
		std::cout << "ERROR: set to observer" << std::endl;
		pressSpaceToQuit();
	}
	statHandler->adjustObservers( 1, getType() );
	setStatus( ST_OBSERVER ); //agentStatus = ST_OBSERVER;
	lastStatusUpdateInCycle = syncMan->getCycleNum();
	informNeighbourhoodOfNewcomer();
	statHandler->changeGridView(this, agentPosition);
}

//note: test later that no problems occur for processors at session-shifts (agents gets dead-locked/ observes each other)
void Agent::setStatusForNextProcessor() {
	/*if (getId() == 213) {
		std::cout << "debug: 213 is proc in next cycle" << std::endl;
		pressSpaceOrQuit();
	}*/
	if (lastStatusUpdateInCycle == syncMan->getCycleNum()) {
		std::cout << "ERROR: status already updated in this cycle, loop problem? [processor]" << std::endl;
		pressSpaceToQuit();
	}
	if (getStatus() != ST_OBSERVER) {
		std::cout << "ERROR: agent, type " << getType() <<", id=" << getId() << ", set status processor: prev status must be observer, was in fact " << getStatus() << std::endl;
		pressSpaceToQuit();
	}
	statHandler->adjustObservers(-1, getType() );
	statHandler->adjustProcessors(1, getType() );
	syncMan->isProcessorNextCycle(this);
	setStatus( ST_PROCESSOR ); //agentStatus = ST_PROCESSOR;
	lastStatusUpdateInCycle = syncMan->getCycleNum();
	statHandler->changeGridView(this, agentPosition);
	hasBeenActiveInSession = true; //used to detect frozen agents
}

void Agent::setStatusForNextDrifter() {

	/*if (getId() == 213) {
		std::cout << "debug: 213 is drifter in next cycle" << std::endl;
		pressSpaceOrQuit();
	}*/
	if (lastStatusUpdateInCycle == syncMan->getCycleNum()) {
		std::cout << "ERROR: status already updated in this cycle, loop problem? [drifter]" << std::endl;
		pressSpaceToQuit();
	}
	lastStatusUpdateInCycle = syncMan->getCycleNum();
	//std::cout << "DEBUG: agent " << getId() << " are drifter next cycle" << std::endl;
	//syncMan->removeIdFromHasNotBeenDrifterList(getId()); //this should be a temporary test
	if (getStatus() == ST_OBSERVER)
		statHandler->adjustObservers(-1,getType());
	else if (getStatus() == ST_DRIFTER)
		statHandler->adjustDrifters(-1,getType() );
	else if (getStatus() == ST_PROCESSOR)
		statHandler->adjustProcessors(-1, getType() );
	else {
		std::cout << "ERROR: set to drifter" << std::endl;
		pressSpaceToQuit();
	}
	if (getStatus() == ST_OBSERVER || getStatus() == ST_PROCESSOR) { //if not obs: bus conn is already empty
		deleteBusinessConnections();
	}
	statHandler->adjustDrifters(1, getType() );
	syncMan->isDrifterNextCycle(this);
	setStatus( ST_DRIFTER );
	statHandler->changeGridView(this, agentPosition);
	hasBeenActiveInSession = true; //used to detect frozen agents
}

void Agent::setStatusForNextDead() {
	//syncMan->removeIdFromHasNotBeenDrifterList(getId()); //this should be a temporary test
	//std::cout << toStringType() << getId() << " dies..." << std::endl;
	//std::cout << toString() << std::endl;
		
	if (getStatus() == ST_DEAD) {
		std::cout << "ERROR: setStatusFor....agent already dead! " << std::endl;
		pressSpaceToQuit();
	}
	statHandler->updateStatAgentDies(this);
	agentPosition->freeCell();
	env->receiveFitness( fitness );
	statHandler->adjustFitnessEnvironment( fitness );
	setFitness(0);	
	syncMan->isDeadNextCycle(this);
	syncMan->removeAgentFromList(this);
	setStatus ( ST_DEAD ); //agentStatus = ST_DEAD;
	lastStatusUpdateInCycle = syncMan->getCycleNum();
	deleteBusinessConnections();  //should be here or in deconsstructor???
	statHandler->changeGridView(this, agentPosition);
}

void Agent::observedAgentIsDrifting(Observable* observedObs) {
	std::cout << "ERROR.Agent: observerAgentIsDrifting must be overloaded in derived classes!" << std::endl;
	/*Agent* observedAgent = (Agent*)observedObs;
	numOfObsAgents--;
	if (numOfObsAgents == 0) {
		setStatusForNextDrifter();
	}*/
}

bool Agent::isReadyToMate() {
	return readyToMate;
}

//Method sets readyToMate;
void Agent::testIfReadyToMate() {
	if (readyToMate) { //Agent is drifter and readyToMate, test if stop
		if (getStatus() != ST_DRIFTER) {
			std::cout << "ERROR.agent: MATER: testifreadytomate, agent should be drifter!" << std::endl;
			pressSpaceToQuit();
		}
		if ((fitness <= matingThrStop) || (numOfOffspring>=MAX_NUM_OFFSPRING)) {
			readyToMate = false;
			statHandler->adjustMaters(-1, getType() );
		}
	} else { //is not-ready-to-mate, test fitness
		if (getStatus() != ST_PROCESSOR) {
			std::cout << "ERROR.agent: NON-mater: testifreadytomate, agent type " << getType() << " should be processor!" << std::endl;
			pressSpaceToQuit();
		}
		if (fitness >= matingThrStart) {
			readyToMate = true;
			//int matingBonusFromEnv = env->giveMatingBonus(initialFitness, fitness);
			//setFitness(fitness + matingBonusFromEnv ); //receive matingBonus from environment (if any)
			statHandler->adjustMaters(1, getType() );
		}
	}
}

//Note: processing and selling must be atomic events to prevent that an agent wins two auctions
void Agent::process() {
	std::cout << "ERROR: process must be overloaded in derived classes!" << std::endl;
	pressSpaceToQuit();

}

void Agent::drift() {
	if (getNumOfPotentialSellers() != 0 || getNumOfPotentialBuyers() != 0) {
		std::cout << "ERROR: agent drift(); agent are observing other agents or are observed by others" << std::endl;
		pressSpaceToQuit();
	}
	if (getStatusAndActive() == ST_FINISHED_THIS_CYCLE) {
		return; //happens when agents mate (parent 'picked up' for mating is also drifter)
		//std::cout << "ERROR: agent drift...this shouldn't happen!" << std::endl;
		//pressSpaceToQuit();
	}
	if (!updateAge())
		return; //agent dies
	if ( !readyToMate && (syncMan->getSessionNum() - lastProcessedInSess >= statHandler->MAX_INACTIVE_SESS)) {
		setStatusForNextDead();
		return;
	}
	//CList<Observable*, Observable*>* allNeighbours;
	if (readyToMate) {
		setCurrentNeighbours( collineGrid->getNeighbours(agentPosition, vicinityMat, false) );
		//allNeighbours = collineGrid->getNeighbours(agentPosition, vicinityMat);
		CList<Agent*, Agent*>* matingSubjects = getMatingSubjects();
		if (matingSubjects->GetCount() == 0 ) { //|| neighbourhoodTooOvercrowdedForMating()) {
			takeRandomStep();
			/*int numOfSteps=1;
			while (isLocatedInNoMatingArea() && numOfSteps<7) { //take up to 7 steps to try to get out of reward area
				takeRandomStep();
				numOfSteps++;
			}*/
			setStatusForNextDrifter(); //keep drifter status
			return;
		}
		else {
			Agent* mate = matingSubjects->GetAt( matingSubjects->FindIndex( 0 )); //pickRandomPosition(matingSubjects->GetCount())));
			//std::cout << "Parent1: " << toString() << std::endl;
			//std::cout << "Parent2: " << mate->toString() << std::endl;
			bool isRoomForOffspring = true;
			while (isReadyToMate() && mate->isReadyToMate() && isRoomForOffspring) {
				if (fitness > mate->getFitness()) {
					//std::cout << "info: " << getId() << " chosen as parent 1 (healthiest)" << std::endl;
					isRoomForOffspring = produceOffspring(mate);
				} else
					isRoomForOffspring = mate->produceOffspring(this);
				testIfReadyToMate();
				mate->testIfReadyToMate();
			}
			if (isReadyToMate())
				setStatusForNextDrifter();      //keep drifter status 
			else
				setStatusForNextDead(); //agent dies after mating

			if (mate->isReadyToMate())
				mate->setStatusForNextDrifter();
			else
				mate->setStatusForNextDead();
			//setStatusForNextDrifter();      //keep drifter status 
			//mate->setStatusForNextDrifter();// do.
			return;
		}
		delete matingSubjects; //consider method 'updateMatingSubj' like for neighbours
	} else {
		//std::cout << "INFO: " << toStringType() << getId() << " drift, about to get neighbors....";
		//CList<Observable*, Observable*>* allNeighbours = collineGrid->getNeighbours(agentPosition, getVicinityBus() );
		setCurrentNeighbours( collineGrid->getNeighbours(agentPosition, getVicinityBus(), false) );
		if ( makeBusinessConnections()) { //scan neighbourhood for business connections
			//std::cout << "INFO: " << toStringType() << getId() << " observes...." << std::endl;
			setStatusForNextObserver();
			return;
		}
		else { //not enough matches
			//std::cout << "INFO: agent takes random step" << std::endl;
			takeRandomStep();
			setStatusForNextDrifter(); //keep drifter status
			return;
		}
	}
}

/*bool Agent::isLocatedInNoRewardArea() {
	int horiPos = getPosition()->getPosX();
	int vertPos = getPosition()->getPosY();
	if (horiPos>=DC_NO_REWARD_AREA_MIN_X && horiPos<=DC_NO_REWARD_AREA_MAX_X && vertPos>=DC_NO_REWARD_AREA_MIN_Y && vertPos<=DC_NO_REWARD_AREA_MAX_Y) //no-reward area
		return true;
	else 
		return false;
	return false;
}*/

/*bool Agent::isLocatedInRewardAreaNorth() {
	int horiPos = getPosition()->getPosX();
	int vertPos = getPosition()->getPosY();
	if (vertPos<=DC_REW_AREA_NORTH_MAX_Y && horiPos>=DC_REW_AREA_COLUMN_MIN_X && horiPos<=DC_REW_AREA_COLUMN_MAX_X)
		return true;
	else 
		return false;
	return false;
}

bool Agent::isLocatedInRewardAreaSouth() {
	int horiPos = getPosition()->getPosX();
	int vertPos = getPosition()->getPosY();
	if (vertPos>=DC_REW_AREA_SOUTH_MIN_Y && horiPos>=DC_REW_AREA_COLUMN_MIN_X && horiPos<=DC_REW_AREA_COLUMN_MAX_X)
		return true;
	else 
		return false;
	return false;
}
*/
/*bool Agent::neighbourhoodTooOvercrowdedForMating() {
	int numOfnb = collineGrid->getNumOfNeighbours(agentPosition, C_MAT_OVERCROWD_SCANRAD,true);
	int numOfPosTotal = (C_MAT_OVERCROWD_SCANRAD*2+1)*(C_MAT_OVERCROWD_SCANRAD*2+1);
	int percentageFilled = (numOfnb*100)/numOfPosTotal;
	if (percentageFilled>C_MAT_OVERCROWD_PRC) {
		//std::cout << "INFO: neighbourhood too filled for mating" << std::endl;
		return true;
	}else
		return false;
	return false;
}
*/

/*bool Agent::isLocatedInNoMatingArea() {
	int horiPos = getPosition()->getPosX();
	int vertPos = getPosition()->getPosY();
	if (horiPos>=NO_MATING_AREA_MIN_X && horiPos<=NO_MATING_AREA_MAX_X && vertPos>=NO_MATING_AREA_MIN_Y && vertPos<=NO_MATING_AREA_MAX_Y) //reward area
		return true;
	else 
		return false;
	return false;
}*/



Message* Agent::getAgentDNA() {
	return agentDNA;
}

Message* Agent::getTypeDNA() {
	std::cout << "ERROR: getTypeDNA must be overloaded in derived classes!" << std::endl;
	pressSpaceToQuit();
	return (new Message(0,0));
}

int Agent::getNumOfTypeDNAbits() {
	std::cout << "ERROR: getNumOfTypeDNA must be overloaded in derived classes!" << std::endl;
	pressSpaceToQuit();
	return -1;
}

//return value true: offspring produced, false: no offspring produced (no room for new agents)
bool Agent::produceOffspring(Agent* mate) {
	//std::cout << "Producing offspring...";
	int randomCut1, randomCut2;
	Message* offspringAppBusSmall;
	Message* offspringAppMat;
	Message* offspringDNA_AG;
	Message* offspringDNA_TYPE;
	Message* thisAppBusSmall = getAppearBusSmall();
	Message* thisAppMat = getAppearMat();
	Message* thisDNA_AG = getAgentDNA();
	Message* thisDNA_TYPE = getTypeDNA();
	Message* mateAppBusSmall = mate->getAppearBusSmall();
	Message* mateAppMat = mate->getAppearMat();
	Message* mateDNA_AG = mate->getAgentDNA();
	Message* mateDNA_TYPE = mate->getTypeDNA();

	//Find offspring mating appearance:
	randomCut1 = pickRandomPosition( APPEAR_SIZE_MAT + 1 ); //random between 0 and APPEAR_SIZE_MAT
	randomCut2 = pickRandomPosition( APPEAR_SIZE_MAT + 1 ); //do.
	if (randomCut1 > randomCut2) { //switch
		int temp = randomCut1;
		randomCut1 = randomCut2;
		randomCut2 = temp;
	}
	offspringAppMat = thisAppMat->replacePart(randomCut1, randomCut2-1, mateAppMat);

	//find offspring business appearance:
	randomCut1 = pickRandomPosition( APPEAR_SIZE_BUS_SMALL + 1 ); //random between 0 and APPEAR_SIZE_BUS_SMALL
	randomCut2 = pickRandomPosition( APPEAR_SIZE_BUS_SMALL + 1 ); //do.
	if (randomCut1 > randomCut2) { //switch
		int temp = randomCut1;
		randomCut1 = randomCut2;
		randomCut2 = temp;
	}
	offspringAppBusSmall = thisAppBusSmall->replacePart(randomCut1, randomCut2-1, mateAppBusSmall);
	//find offspring dna:
	int numOfDNAbits = N_DNA_AG + getNumOfTypeDNAbits();
	int startPosAG = 0; //stopPosOBS + 1;
	int stopPosAG = N_DNA_AG - 1;
	int startPosTYPE = stopPosAG + 1;
	int stopPosTYPE = startPosTYPE + getNumOfTypeDNAbits() - 1;
	//find DNA section to be replaced (randomly):
	//the randomCut value gives the pos of the first bit to be replaced, randomCut2 - 1 (NOTE!) gives the last pos to be replaced
	randomCut1 = pickRandomPosition( numOfDNAbits + 1 ); //gives random pos between 0 and numOfDNAbits
	randomCut2 = pickRandomPosition( numOfDNAbits + 1 ); //do.
	if (randomCut1 > randomCut2) { //switch
		int temp = randomCut1;
		randomCut1 = randomCut2;
		randomCut2 = temp;
	}	
	offspringDNA_AG = getOffspringPart(randomCut1, randomCut2 - 1, startPosAG, stopPosAG, thisDNA_AG, mateDNA_AG);
	offspringDNA_TYPE = getOffspringPart(randomCut1, randomCut2 - 1, startPosTYPE, stopPosTYPE, thisDNA_TYPE, mateDNA_TYPE);

	//std::cout << "After mutation: " << std::endl;
	int numMut;
	int addMutation = 0;
	int dice = env->getRandNumBetwZeroAnd(99);
	if (dice < statHandler->HIGH_MUT_AGENTS_PRC)
		addMutation = MUTATION_RATE_HIGH_ADD_PM;
	else
		addMutation = 0;
	numMut = offspringAppMat->flipBits(statHandler->MUTATION_RATE/*_SPEC_PM*/ + addMutation);
	numMut += offspringAppBusSmall->flipBits(statHandler->MUTATION_RATE/*_BUS_APP_PM*/ + addMutation);
	numMut += offspringDNA_AG->flipBits(statHandler->MUTATION_RATE/*_DNA_AG_PM*/ + addMutation);
	numMut += offspringDNA_TYPE->flipBits(statHandler->MUTATION_RATE/*_DNA_TYPE_PM*/ + addMutation);
	//std::cout << "Ofspring DNA obs : " << (LPCTSTR)offspringDNA_OBS->toStringBits() << std::endl;
	//std::cout << "Ofspring DNA ag  : " << (LPCTSTR)offspringDNA_AG->toStringBits() << std::endl;
	//std::cout << "Ofspring DNA type: " << (LPCTSTR)offspringDNA_TYPE->toStringBits() << std::endl;
	//std::cout << "INFO: number of bits mutated in offspring: " << numMut << " (" << addMutation << ")" << std::endl;
	CList<GridCell*, GridCell*>* freeCells = collineGrid->getFreeCells(agentPosition, vicinityMat);
	if (freeCells->GetCount() == 0) { //FIX move this test up before calc of offspringDNA
		//std::cout << "WARNING: Grid over-crowded, no room for new agents! (agent try to move)" << std::endl;
		/*env->receiveFitness( getOffspringFitness() );
		statHandler->adjustFitnessEnvironment( getOffspringFitness() );
		env->receiveFitness( mate->getOffspringFitness() );
		statHandler->adjustFitnessEnvironment( mate->getOffspringFitness() );*/
		delete freeCells;
		return false;
	} else {
		Observable* baby;
		int offspringFitness = 0;
		offspringFitness += getOffspringFitness(); //(subtract fitness from parent)
		offspringFitness += mate->getOffspringFitness();
		//std::cout << " (offspring fitness: " << offspringFitness << ")" << std::endl;
		GridCell* babyPosition = freeCells->GetAt( freeCells->FindIndex( pickRandomPosition(freeCells->GetCount()) ));
		baby = makeBaby(env,collineStamp, collineGrid, babyPosition, syncMan, offspringFitness, offspringAppMat, offspringAppBusSmall, offspringDNA_AG, offspringDNA_TYPE);
		increaseProducedOffspring();
		mate->increaseProducedOffspring();
		delete freeCells;
		return true;
	}
	return true;
}

void Agent::increaseProducedOffspring() {
	numOfOffspring++;
}

Observable* Agent::makeBaby(Environment* e, IdStamp* cStamp, Grid* collineGrid, GridCell* babyPosition, SyncManager* syncMan, int offspringFitness, Message* offspringAppMat, Message* offspringAppBusSmall, Message* offspringDNA_AG, Message* offspringDNA_TYPE) {
	std::cout << " ERROR: agent, makebaby" << std::endl;
	pressSpaceToQuit();
	return new Observable();
}

Message* Agent::getOffspringPart(int repPosStart, int repPosStop, int partPosStart, int partPosStop, Message* thisDNA, Message* mateDNA) {
	Message* offspringPart;
	int repPosStart_local;
	int repPosStop_local;
	if (((repPosStart <= partPosStop) && (repPosStop >= partPosStart)) && (repPosStart < repPosStop)) { //replace bits
		if (repPosStart >= partPosStart)
			repPosStart_local = repPosStart;
		else
			repPosStart_local = partPosStart;
		if (repPosStop <= partPosStop)
			repPosStop_local = repPosStop;
		else
			repPosStop_local = partPosStop;
		repPosStart_local -= partPosStart; //pos must be adjusted for the 'offset'
		repPosStop_local -= partPosStart; // do.
		//std::cout << "local replacing from: " << repPosStart_local << ", to: " << repPosStop_local << std::endl;
		offspringPart = thisDNA->replacePart(repPosStart_local, repPosStop_local, mateDNA);
	} else
		offspringPart = thisDNA->clone();
	return offspringPart;
}

bool Agent::makeBusinessConnections() {
	std::cout << "ERROR: makeBusinesscon must be overloaded in derived classes!" << std::endl;
	pressSpaceToQuit();
	return false;
}

Message* Agent::getAppearMat() {
	return appearMat;
}

int Agent::getOffspringFitness() {	
	int percentage = 0;
	int dice = env->getRandNumBetwZeroAnd(99); 
	if (dice<C_CHANCE_VERY_HIGH_PRC)
		percentage = C_PRC_TO_OFF_VERY_HIGH;
	else if (dice<C_ACC_CHANCE_HIGH)
		percentage = C_PRC_TO_OFF_HIGH;
	else if (dice<C_ACC_CHANCE_MED)
		percentage = C_PRC_TO_OFF_MED;
	else if (dice<C_ACC_CHANCE_LOW)
		percentage = C_PRC_TO_OFF_LOW;
	else if (dice<C_ACC_CHANCE_VERY_LOW)
		percentage = C_PRC_TO_OFF_VERY_LOW;
	else {
		std::cout << "ERROR: getOff fitness" << std::endl;
		pressSpaceOrQuit();
	}
	if (fitness > ((C_AGENT_HEALTHY_PRC*initialFitness)/100))
		percentage += C_PRC_TO_OFF_BOOST; //if parent is 'boosted' begin by making strong offspring
	//adjust for grid overcrowding/ undercrowding: The higher the overcrowdPercentage the less offspring
	int agentsInGrid = statHandler->getNumOfAgents();
	int overcrowdPercentage;
	if (agentsInGrid < statHandler->MIN_NUM_OF_AGENTS_IN_GRID)
		overcrowdPercentage = -1*C_PRC_TO_OFF_SMALL_POP_SUB;
	else if (agentsInGrid > statHandler->IDEAL_NUM_OF_AGENTS_IN_GRID)
		overcrowdPercentage = C_PRC_TO_OFF_OVERCROWD_ADJ;
	else if (agentsInGrid > (statHandler->IDEAL_NUM_OF_AGENTS_IN_GRID - 100))
		overcrowdPercentage = 0.6 * C_PRC_TO_OFF_OVERCROWD_ADJ;
	else if (agentsInGrid > (statHandler->IDEAL_NUM_OF_AGENTS_IN_GRID - 200))
		overcrowdPercentage = 0.3 * C_PRC_TO_OFF_OVERCROWD_ADJ;
	else if (agentsInGrid > (statHandler->IDEAL_NUM_OF_AGENTS_IN_GRID - 300))
		overcrowdPercentage = 0.1 * C_PRC_TO_OFF_OVERCROWD_ADJ;
	else 
		overcrowdPercentage = 0;
	if (syncMan->getSessionNum() < statHandler->MAX_INACTIVE_SESS)
		overcrowdPercentage = 0; //do not adjust with overcrowdpercentage in the start-up phase
	percentage += overcrowdPercentage; //note: negative when small pop
	//std::cout << "DEBUG: percentage to off: " << percentage << std::endl;
	int amountToOff = (initialFitness*percentage)/100;
	int exceedingFitness = fitness-(initialFitness*C_FITN_ABOVE_GIVES_BONUS)/100;
	if (exceedingFitness<0)
		exceedingFitness=0;

	amountToOff += (exceedingFitness*C_PRC_OF_SURPLUS_TO_OFF)/100; //calc bonus to healthy agents
	if (amountToOff>fitness)
		amountToOff = fitness; //max all fitness
	else if ( amountToOff < ((initialFitness*C_PRC_TO_OFF_VERY_LOW)/100) )
		amountToOff = initialFitness*C_PRC_TO_OFF_VERY_LOW/100; //min lowest percentage
	//std::cout << "DEBUG: amountToOff " << amountToOff << std::endl;
	//pressSpaceOrQuit();

	setFitness( fitness - amountToOff ); //fitness -= amountToOff;
	
	/*OLD_BEG int amountToOff = (fitness*C_PFITNTOOFF_BASE )/100; //fitness*(C_PFITNTOOFF_BASE + (numOfOffspring+1)^2 )/100;
	int maxFitnToOff = (initialFitness*(C_MAX_TO_OFF_PRC-numOfOffspring))/100; 
	int minFitnToOff = (initialFitness*C_MIN_TO_OFF_PRC)/100; 
	if (amountToOff > maxFitnToOff)
		amountToOff = maxFitnToOff;
	if (amountToOff < minFitnToOff) {
		amountToOff = minFitnToOff;
		if (amountToOff > fitness)
			amountToOff = fitness;
	}OLD_END*/
	return amountToOff;
}

/*Message* Agent::getAppearBus() {
	std::cout << "ERROR: getAppearBus must be overloaded in derived classes!" << std::endl;
	pressSpaceToQuit();
	return (new Message());
}*/

Message* Agent::getAppIdealMat() {
	return appIdealMat;
}

/*Message* Agent::getAppPickiMat() {
	return appPickiMat;
}*/

//returns random number between 0 and (numOfPos-1)
int Agent::pickRandomPosition(int numOfPositions) {

	if (numOfPositions == 0) {
		std::cout << "ERROR: pickRandom can't pick from zero!" << std::endl;
		pressSpaceToQuit();
	}
	if (numOfPositions == 1)
		return 0;
	int randomPos = env->getRandNumBetwZeroAnd( numOfPositions - 1 );
	return randomPos;
}

void Agent::setBidPoF(int percent) {
	bidPoF = percent;
}

void Agent::takeRandomStep() {
	int step_size;
	if (readyToMate)
		step_size = vicinityMat;
	else {
		step_size = getVicinityBus(); // when vic is larger than 1, the agent can 'jump' across other agents into a free cell
		if (step_size<MIN_STEPSIZE)
			step_size=MIN_STEPSIZE;
	}
	int oldX = agentPosition->getPosX();
	int oldY = agentPosition->getPosY();
	int newX;
	int newY;
	bool freeCellFound = false;
	int tries=NUM_OF_TAKE_STEP_TRIES;
	int gridWidth = collineGrid->getWidthX();
	int gridHight = collineGrid->getHightY();
	while (tries>0 && !freeCellFound) {
		newX = oldX + (getRandNumBetwZeroAnd(2*step_size) - step_size);
		newY = oldY + (getRandNumBetwZeroAnd(2*step_size) - step_size);
		if (getRandNumBetwZeroAnd(100) < statHandler->GRAVITY_CENTER) { //adjust towards center
			if (newX < gridWidth/2)
				newX++;
			else
				newX--;
			if (newY < gridHight/2)
				newY++;
			else
				newY--;
		}	
		if (newX > (gridWidth-1) )
			newX = (gridWidth-1);
		if (newX < 0)
			newX = 0;
		if (newY > (gridHight-1) )
			newY = (gridHight-1);
		if (newY < 0)
			newY=0;
		if ( !(collineGrid->getCellAt(newX,newY)->isOccupied()) )
			freeCellFound = true;
		tries--;
	}
	if (!freeCellFound) { //keep position
		newX = oldX;
		newY = oldY;
	}
	agentPosition->freeCell();
	GridCell* oldPos = agentPosition;
	agentPosition = collineGrid->getCellAt(newX,newY);
	agentPosition->placeOccupier(this);
	statHandler->changeGridView(this, oldPos);
	// old code: 
	/*agentPosition->freeCell();
	GridCell* oldPos = agentPosition;
	CList<GridCell*, GridCell*>* freeCells = collineGrid->getFreeCells(agentPosition, step_size);
	if (freeCells->GetCount() == 0) {
		std::cout << "WARNING: No free cells, no step taken!" << std::endl;
		agentPosition->placeOccupier(this);
	}
	else {
		agentPosition = freeCells->GetAt( freeCells->FindIndex( pickRandomPosition(freeCells->GetCount()) ));
		agentPosition->placeOccupier(this);
	}
	delete freeCells;
	statHandler->changeGridView(this, oldPos);*/
	//old code, end
}


CList<Agent*, Agent*>*  Agent::getMatingSubjects() {
	int i;
	//TEST search nb for this agent
	/*int thisId = getId();
	for (i=0; i<nb.size(); i++) {
		if (nb.at(i)->getId() == thisId) {
			std::cout << "ERROR: getMatingSubj this is in nb!" << std::endl;
		}
	}*/
	CTypedPtrList<CObList, Agent*>  agents; //CList<Agent*, Agent*>* agents = new CList<Agent*, Agent*>(5);
	CList<Agent*, Agent*>* matingSubjects = new CList<Agent*, Agent*>;
	Message* candAppearMat;
	Message* candAppIdealMat;
	//Message* candAppPickiMat;
	//find similar type and status=drifter:
	Observable* tempObservable;
	int thisType = getType();
	for (i=0; i<currentNeighbours->GetCount(); i++) {
		tempObservable = currentNeighbours->GetAt( currentNeighbours->FindIndex(i));
		if (tempObservable->getType() == thisType) {
			if (tempObservable->getStatusAndActive() == ST_DRIFTER)
				agents.AddTail((Agent*)tempObservable);
		}
	}
	//Test if ready-to-mate and appearance match
	int numOfCandidates = agents.GetCount();
	int maxCandidateFitness = 0;
	Agent* candidate;
	for (i=0; i<numOfCandidates; i++) {
		candidate = agents.GetAt( agents.FindIndex(i) );
		if (candidate->isReadyToMate()) {
			candAppearMat = candidate->getAppearMat();	
			if (appIdealMat->getValue() == candAppearMat->getValue() ) {								//if (appIdealMat->isEqualTo(candAppearMat, appPickiMat) ) { //candidate appearance accepted 
				candAppIdealMat = candidate->getAppIdealMat(); //the candidates ideal appearance for a mate
				if ( candAppIdealMat->getValue() == appearMat->getValue()) {						//if (candAppIdealMat->isEqualTo(appearMat, candAppPickiMat) )  //this appearance accepted by candidate
					if (candidate->getFitness() > maxCandidateFitness) {
						maxCandidateFitness = candidate->getFitness();
						matingSubjects->AddHead( candidate ); //Healthiest agent is at first position
					} else
						matingSubjects->AddTail( candidate );
				}
			}
		}
	}
	//delete agents;
	return matingSubjects;
}


//index 0 is highest
Observable* Agent::findHighestBidder() { //the bidder can be an agent or a decisionCategory
	Observable* tempBidder;
	int numOfBids = bidders.GetCount(); // bidders.size();
	if (numOfBids == 0) {
		std::cout << "ERROR: findhighestbidder assumes bidders!" << std::endl;
		pressSpaceToQuit();
	}
	Observable* maxBidder = bidders.GetAt( bidders.FindIndex(0) );
	//Gentlemen agreement: Effectors have a 'gentlemen' agreement among each other: if a lower bidding effector have _one_ mess, it wins
	if (maxBidder->getType() == TYPE_EFFECTOR) { 
		if (!maxBidder->hasOneStateMessage()) { //check if another effector has a message already
			int bidderNum = 1;
			bool soldToLowerBidding = false;
			while (bidderNum<numOfBids && !soldToLowerBidding) {
				tempBidder = bidders.GetAt( bidders.FindIndex( bidderNum ) );
				if (tempBidder->hasOneStateMessage()) { //only effectors can return true
					maxBidder = tempBidder;
					soldToLowerBidding = true;
				}
				bidderNum++;
			}
		}
	} //end agreement
	return maxBidder;
}

bool Agent::sellMessage(Message* mess) {
	if (bidders.GetCount() == 0) {
		//std::cout << "no bidders...." << std::endl;
		return false;
	}
	else {
		Observable* buyer = findHighestBidder(); //Agent* buyer = findHighestBidder();
		Message* potentialCopyToSell = mess->clone();
		int payment = buyer->auctionWon(this, mess);
		setFitness( fitness + payment);
		//if (getType()==TYPE_EFFECTOR)
		//	std::cout << "DEBUG: effector message sold to: " << buyer->getType() << " , bid: " << payment << std::endl;	
		if (buyer->getType() == TYPE_CATEGORY && DC_ONLY_READS_MESSAGE) {//if message is sold to trainer, second highest bidder can buy (optional)
			bidders.RemoveAt( bidders.FindIndex(0));
			if (!sellMessage( potentialCopyToSell )) //sell to second highest bidder, if any
				delete potentialCopyToSell;
		} else {
			bidders.RemoveAll();
			delete potentialCopyToSell;
		}
	}
	return true;
}

//returns number of buyers
int Agent::sellState(bool state) {
	//std::cout << "selling state info.." << std::endl;
	Message* stateMess;
	int numOfBuyers = bidders.GetCount();
	//std::cout << "num of buyers: " << numOfBuyers << std::endl;
	if (getType()==TYPE_INPODER) {
		if (numOfBuyers > statHandler->MAX_STATE_BUYERS_INP)
			numOfBuyers = statHandler->MAX_STATE_BUYERS_INP;
	} else if (getType()==TYPE_COLLECTOR) {
		if (numOfBuyers > MAX_STATE_BUYERS_COL)
			numOfBuyers = MAX_STATE_BUYERS_COL;
	} else {
		std::cout << "ERROR: sellstate" << std::endl;
		pressSpaceToQuit();
	}
	int payment;
	//std::cout << "INFO: (sellstate) numOfBuyers: " << numOfBuyers << std::endl;
	for (int i=0; i<numOfBuyers; i++) {
		stateMess = new Message(state,1); 
		payment = bidders.GetAt( bidders.FindIndex(i) )->auctionWon(this, stateMess);
		setFitness(fitness + payment); //fitness += payment;
	}
	bidders.RemoveAll();
	//std::cout << "INFO: (sellstate) finished" << std::endl;
	return numOfBuyers;
}

int Agent::getVicinityBus() {
	return vicinityBus;
}

int Agent::getAge() {
	return age;
}

CList<Observable*, Observable*>* Agent::shuffleAgentList(CList<Observable*, Observable*>* listToBeShuffled) {
	Observable* temp;
	Observable* temp2;
	POSITION temp2_pos;
	//POSITION accumPosition, swapWithPosition;
	int i, swapWithIndex;
	//CList<Observable*, Observable*>* shuffledList = new CList<Observable*, Observable*>;
	int listSize = listToBeShuffled->GetCount();
	/*if (listSize<26) {
		std::cout << "before shuffling: " << std::endl;
		for (i=0; i<listSize; i++) {
			temp = listToBeShuffled->GetAt( listToBeShuffled->FindIndex(i) );
			std::cout << (LPCTSTR)temp->toStringId() << std::endl;
		}
	}*/
	//std::cout << "size of list: " << listSize << std::endl;
	for (i=0; i<listSize; i++) {
		//accumPosition = listToBeShuffled->FindIndex(i);
		temp = listToBeShuffled->GetAt( listToBeShuffled->FindIndex(i) );
		swapWithIndex = (rand()*(listSize-1))/RAND_MAX;
		//swapWithPosition = listToBeShuffled->FindIndex( swapWithIndex );
		temp2 = listToBeShuffled->GetAt( listToBeShuffled->FindIndex( swapWithIndex ) );
		//std::cout << "swaps " << i << " with " << swapWithIndex << std::endl;	
		listToBeShuffled->RemoveAt( listToBeShuffled->FindIndex(i) ); //remove temp from orig position
		//std::cout << "inserts temp" << std::endl;
		if (swapWithIndex==(listSize-1))
			listToBeShuffled->AddTail(temp);
		else
			listToBeShuffled->InsertAfter( listToBeShuffled->FindIndex(swapWithIndex) , temp); //insert at new position
		//std::cout << "removes temp2" << std::endl;
		temp2_pos = listToBeShuffled->Find(temp2); //use find because indexes above may be invalid
		listToBeShuffled->RemoveAt( temp2_pos ); //remove temp2 from orig position
		//std::cout << "inserts temp2" << std::endl;
		if (i==(listSize-1))
			listToBeShuffled->AddTail(temp2);
		else
			listToBeShuffled->InsertAfter(listToBeShuffled->FindIndex(i), temp2); //insert at new position
	}
	//test that shuffle is ok:
	if (listToBeShuffled->GetCount() != listSize) {
		std::cout << "ERROR: shuffle list" << std::endl;
		pressSpaceToQuit();
	}
	/*if (listSize<26) {
		std::cout << "after shuffling: " << std::endl;
		for (i=0; i<listSize; i++) {
			temp = listToBeShuffled->GetAt( listToBeShuffled->FindIndex(i) );
			std::cout << (LPCTSTR)temp->toStringId() << std::endl;
		}
		pressSpaceOrQuit();
	} //delete later, fix
	*/
	return listToBeShuffled;
}
		
int Agent::getRandNumBetwZeroAnd(int maximum) {
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


//}//namespace