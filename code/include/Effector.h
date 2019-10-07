// Effector.h: interface for the Effector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFECTOR_H__1481E5B8_BA6A_44E2_BB27_4C3EA5C66D3E__INCLUDED_)
#define AFX_EFFECTOR_H__1481E5B8_BA6A_44E2_BB27_4C3EA5C66D3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DecisionCategory.h"
#include "Agent.h"

class Effector : public Agent  
{
public:
	Effector(Environment* e,
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
		DecisionCategory* myDC);
	virtual ~Effector();
	bool makeBusinessConnections(); 
	void observe(Observable* obj);
	void turnFocusTo(Observable*);
	void observedAgentIsDrifting(Observable* agent);
	void process(); //process and announce auction (called by syncMan)
	//effector processing is to compare the output/state from a certain group of agents with the output/state from another certain group
	int auctionWon(Observable* seller, Message* boughtMessage); //returns the fitness paid
	int getCatLevel();
	std::string toStringObserveBusApp();
	std::string toStringTypeSpec();
	Message* getTypeDNA();
	int getProcType();
	Message* getAppIdealBus1();
	Message* getAppPickiBus1();
	Message* getAppIdealBus2();
	Message* getAppPickiBus2();
	int getNumOfObsAgents1();
	int getNumOfObsAgents2();
	bool hasOneStateMessage();
	//void setNumOfObsAgentsToZero();
	int clearNumOfObservedAgents();


	//----------methods for test purpose:-----------------
	void TESTsetBoughtMess1(Message* m);
	void TESTsetBoughtMess2(Message* m);
	void TESTsetBoughtMessB(Message* m);
	void TESTsetProcType(int val);

protected:
	//int getVicinityBus();
	int getNumOfTypeDNAbits();
	bool checkForNewObserveConnection(Agent* newObserverInNB);
		

private:
	std::string toStringProcType();
	Observable* makeBaby(Environment*,IdStamp*,Grid*, GridCell*, SyncManager*, int fitn, Message* appMat, Message* appBus, Message* agent, Message* type);
	CList<Observable*,Observable*>* removeNonLegalBusiness(); //removes non-observers and fountains from list
	//vector<Agent*> agentsInput1; //effectors need to keep track on which agents correspond to which input
	//vector<Agent*> agentsInput2; //note: test remove obj from vector and 'isEqual'....delete vectors SOMEWHERE

	//---------------- methods used for bid-decision -------------------
	//char numberOfBidsThisCycle();
	int findObserveCategory(Observable* agent); //returns CAT_INP1, CAT_INP2 or CAT_BOTH
	//char legalBidsLeftThisCycle(char numOfBidsTC); //returns 0, 1 or 2
	//char findPrevBidCat(); //returns the cat. that the first bid/purchase belongs to
	int findBoughtMessCat();
	bool sellerCatOK(char sellerCat, char prevBidCat); //is input free?
	//void updateBidTrackers(char sellerCat, bool bothInputsFree);
	//------------------------------------------------------------------
	int findCategoryLevel(Message* obsPart);
	void wasteMessages();
	int getNumOfBoughtMess();
	bool getProcResult(Message* inpA, Message* inpB, int procType);

	int numOfObsAgents1;
	int numOfObsAgents2;
	int numOfObsAgentsBoth; //needed when one in nb starts drifting, to determine whether to stay or go

	//int vicinityBus;
	Message* boughtMessageInp1; //This message is bought from an CAT_INP1 agent
	Message* boughtMessageInp2; //This message is bought from an CAT_INP2 agent 
	Message* boughtMessageBoth; //This message is bought from an CAT_BOTH agent
	Message* EMPTYMESS; //boughtMessage* points to this when not used
	int bidCategory; //the last bid was made to an agent in this category

	DecisionCategory* effCategory; //this cat object is notified when eff has processed (in initial impl there is only one shared DC object)
	//int catLevel;
	int lastBoughtInCycle;
	int lastBoughtFromAgent;
	//--------Effector DNA------------
	Message* effectorDNA;
	
	int typeOfProc;
	//Message* appearBus; //move to Observable?
	Message* appIdealBus1;
	Message* appPickiBus1;
	Message* appIdealBus2;
	Message* appPickiBus2;

	//--------------------------------

};

#endif // !defined(AFX_EFFECTOR_H__1481E5B8_BA6A_44E2_BB27_4C3EA5C66D3E__INCLUDED_)
