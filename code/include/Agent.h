// Agent.h: interface for the Agent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGENT_H__A70FF6AB_0BDD_4C5E_AC17_2D986C20B22B__INCLUDED_)
#define AFX_AGENT_H__A70FF6AB_0BDD_4C5E_AC17_2D986C20B22B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Observable.h"
#include "math.h"
//#include "Appearance.h"
//#include "Resource1.h"
#include "Message.h"
#include "SyncManager.h"
#include "StatisticsHandler.h"
#include "Grid.h" //includes observable in grid.h -> potential loop conflict
#include "GridCell.h"
//#include <bitset>
//using std::bitset;
#include <list>

//namespace jf_w3_agent {

class Agent : public Observable  
{
public:
	Agent();
	Agent(Environment* e,
		StatisticsHandler* sh,
		IdStamp* cStamp,
		Grid* cGrid,
		  GridCell* agentPos,
		  int agentType,
		  SyncManager* s,
		  int initFitn,
		  int currFitn,
		  int initAge,
		  bool matStatus,
		  int lastPrSess,
		  Message* appearanceMating,
		  Message* appearanceBusiness,
		  Message* agentPart
		  );

	virtual ~Agent();
	virtual void observe(Observable* );
	virtual void observedAgentIsDrifting(Observable* observedAgent);
	virtual int auctionWon(Observable* seller, Message* boughtMessage); //This method is also used for effectors buying states, where no real auction takes place (every bidder buys)
	virtual void process(); //used by syncMan to initiate processor
	bool produceOffspring(Agent* mate);
	void drift(); //used by syncMan to initiate drifters
	virtual void turnFocusTo(Observable*);
	virtual Message* getTypeDNA();
	virtual bool hasOneStateMessage();
	int getFitness();
	int getInitialFitness();
	int getBid(); //MOVE to observable?
	
	void addToBidders(Observable* bidder);
	//void setBid(unsigned int nBid);
	bool sellMessage(Message* mess); //returns true if message sold
	int sellState(bool state); //returns number of buyers
	unsigned int getBidPoF();
	GridCell* getPosition();
	int getOffspringFitness();
	//char getStatus(); //returns the agentStatus (without info about activities this cycle)
	int getStatusAndActive(); //returns the agentStatus if agent is still waiting to 'act' this cycle otherwise ST_FINISHED_THIS_CYCLE
	int getAge();
	Message* getAppearMat();
	Message* getAppIdealMat();
	//Message* getAppPickiMat();
	Message* getAgentDNA();
	unsigned int getMaxLifeTime();
	int getNumOfOffspring();
	int getLastProcessedInSession();
	CList<Observable*, Observable*>* getCurrentNeighbours();
	//virtual Message* getAppearBus();
	virtual std::string toStringObserveBusApp();
	void setBidPoF(int percent);
	void setStatusForNextDrifter();
	void setStatusForNextObserver();
	void setStatusForNextProcessor();
	void setStatusForNextDead();
	bool isReadyToMate(); //returns readyToMate; 
	//bool isLocatedInNoRewardArea(); //returns true if agent position is within no-reward area (from environment)
	//bool isLocatedInRewardAreaNorth();
	//bool isLocatedInRewardAreaSouth();
	//bool isLocatedInNoMatingArea(); //returns true if agent position is within 'no-mating' area
	int pickRandomPosition(int numOfPositions); //proteceted
	//CString toString();
	std::string toStringAgentSpec();
	void setHasBeenActiveInSession(bool value) {hasBeenActiveInSession = value;}
	bool getHasBeenActiveInSession(){return hasBeenActiveInSession;}
	int getRandNumBetwZeroAnd(int maximum);

	//-----methods used for test purpose:-----------
	void TESTsetFitness(int newValue);
	void TESTsetReadyToMate(bool val);
	void TESTscatterMaxLifeTime(int change);
	//void TESTsetStatus(char status);


	//-----attributs-------------
	SyncManager* syncMan; //move to private?
	StatisticsHandler* statHandler;	
	int bidAgressionLevel; 
	//int speciesNum; //given by statHandler
	int tempDEBUG;

	CTypedPtrList<CObList, Observable*> bidders; //CList<Observable*, Observable*>* bidders; //This vector is used for all auctions -> finish auction before next....MOVE to observable?

protected:
	void testIfReadyToMate(); //method sets readyToMate
	void setFitness(int value); //set fitness and update fitnessColline
	void makeBidNoUpdate(Observable* seller);
	void makeBidUpdate(Observable* seller);
	int getVicinityBus();
	CList<Observable*, Observable*>* shuffleAgentList(CList<Observable*, Observable*>* listToBeShuffled);
	virtual bool checkForNewObserveConnection(Agent* newObserverInNb); //returns true if agent starts observing 'newObserverInNb' (false if agent already does)
	
	bool updateAge();
	virtual int getNumOfTypeDNAbits();

	//---------Attributs------------------
	Grid* collineGrid;
	GridCell* agentPosition; //The current cell held by the agent in the grid
	int fitness;
	int minBusinessFitness;
	int lastProcessedInSess;
	int vicinityMat;
	int vicinityBus;
	

private:
	//------methods:--------
	//CString toStringStatus(); //DELETE
	//vector<Observable*> sortBidders();
	void increaseProducedOffspring();
	//bool neighbourhoodTooOvercrowdedForMating(); //returns true if nb is too occupied for mating to occur
	std::string toStringAge();
	std::list<Agent*> getMatingSubjects(); //returns the agents that are ready-to-mate and match app. criteria (mutually)	
	//virtual vector<Agent*> removeNonSimilarAgents(vector<Observable*> nb);
	Observable* findHighestBidder();
	virtual bool makeBusinessConnections(); //returns true if enough matches found
	void takeRandomStep();
	Message* getOffspringPart(int repPosStart, int repPosStop, int partPosStart, int partPosStop, Message* thisDNA, Message* mateDNA);
	virtual Observable* makeBaby(Environment*,IdStamp*,Grid*, GridCell*, SyncManager*, int fitn, Message* appMat, Message* appBus, Message* agent, Message* type);
	void setCurrentNeighbours( CList<Observable*, Observable*>* nb);
	void informNeighbourhoodOfNewcomer(); //informs agents in neighbourhood of this possible new agent to observe. NB! Introduces risk for 'frozen agentpatterns'
	
	//------------Attributs:---------------
	
	int bid;
	int lastBidUpdateInCycle; //only relevant for effectors (other agents always update bid before making bid)
	//int agentStatus; //Drifter, processor or observer (see const declaration)
	bool readyToMate;
	//int numOfObsAgents; //counter that keeps track on # of observed agents

	CList<Observable*, Observable*>* currentNeighbours;

	int matingThrStart; //specifies when to start mating, value based on initialfitness, uses C_MATSTART
	int matingThrStop;  //          -       stop  -     , -                              -    C_MATSTOP
	int initialFitness;

	int lastStatusUpdateInCycle;
	int lastNeighboursUpdateCycle;
	int bornInCycle;
	int age;
	int numOfOffspring;
	//int agentSpeciesNum; //between 0 and 9 for a 2 bit mat.app.

	bool hasBeenActiveInSession;

	//-----------Agent DNA:----------------
	Message* agentDNA; //bitset<N_DNA_AG> agentDNA;

	unsigned int maxLifeTime; //num of cycles granted
	unsigned int bidPoF; //Percentage of fitness used in bid, max 50% for effectors
	Message* appearMat;
	Message* appIdealMat;
	//Message* appPickiMat; //bit # i = 1, means bit # i counts in idealApp

	//-------------------------------------
};

//} //namespace

#endif // !defined(AFX_AGENT_H__A70FF6AB_0BDD_4C5E_AC17_2D986C20B22B__INCLUDED_)
