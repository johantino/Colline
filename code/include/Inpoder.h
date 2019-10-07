// Inpoder.h: interface for the Inpoder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPODER_H__2EDA3B42_5EA8_48DE_A49C_6011EB7CB84B__INCLUDED_)
#define AFX_INPODER_H__2EDA3B42_5EA8_48DE_A49C_6011EB7CB84B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Agent.h"

class Inpoder : public Agent  
{
public:
	Inpoder(Environment* e,
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
		Message* inpoderPart);
	virtual ~Inpoder();
	void observe(Observable* obj); //obj is collector or fountain
	void observedAgentIsDrifting(Observable* agent);
	void process(); //process and announce auction (called by syncMan)
	void turnFocusTo(Observable*);
	int auctionWon(Observable* seller, Message* boughtMessage); //returns the fitness paid
	CString toStringObserveBusApp();
	CString toStringTypeSpec();
	Message* getTypeDNA();
	Message* getAppBusIdeal();
	Message* getAppBusPicki();
	Message* getFood();
	bool getSmallFood();
	bool hasOneStateMessage() { return false;}
	int clearNumOfObservedAgents();
	//void setNumOfObsAgentsToZero();
	//-----methods for test purpose: -----------
	//Message* TESTgetFood();
	void TESTsetBoughtMessage(Message* m);

	//int testDEBUG;

protected:
	//int getVicinityBus();
	int getNumOfTypeDNAbits();
	bool checkForNewObserveConnection(Agent* newObserver);
	

private:
	Observable* makeBaby(Environment*,IdStamp*,Grid*, GridCell*, SyncManager*, int fitn, Message* appMat, Message* appBus, Message* agent, Message* type);
	bool makeBusinessConnections(); //returns true if matches found
	bool isOkBusApp(Observable* possNewBusConn);
	CList<Observable*,Observable*>* removeNonLegalBusiness(); //return collectors and fountains in list
	CList<Observable*,Observable*>* findOkBusAppear(CList<Observable*,Observable*>* subj);
	
	Message* boughtMessage;
	int numOfObsAgents;
	//int vicinityBus;

	//-------Inpoder DNA -------------
	Message* inpoderDNA;
	//Message* appearBus;
	Message* appIdealBus;
	Message* appPickiBus;
	bool smallFood; //if true the food length is one bit
	Message* food; //try without pickiness first, 1 or 2 bit
	//--------------------------------
};

#endif // !defined(AFX_INPODER_H__2EDA3B42_5EA8_48DE_A49C_6011EB7CB84B__INCLUDED_)
