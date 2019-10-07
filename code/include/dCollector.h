// dCollector.h: interface for the Collector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCOLLECTOR_H__A174BF22_6FEF_4261_B88C_38522C334D7C__INCLUDED_)
#define AFX_DCOLLECTOR_H__A174BF22_6FEF_4261_B88C_38522C334D7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Agent.h"

class Collector : public Agent  
{
public:
	Collector(Environment* e,
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
		Message* collectorPart);
	virtual ~Collector();
	void observe(Observable* obj);
	void observedAgentIsDrifting(Observable* agent);
	void process(); //process and announce auction (called by syncMan)
	void turnFocusTo(Observable*);
	//void produceOffspring(Agent* mate);
	int auctionWon(Observable* seller, Message* boughtMessage); //returns the fitness paid
	std::string toStringObserveBusApp();
	std::string toStringTypeSpec();
	Message* getTypeDNA();
	Message* getAppBusIdeal();
	Message* getAppBusPicki();
	bool hasOneStateMessage() { return false;}
	//Message* TESTgetAppIdealBus();
	//Message* TESTgetAppPickiBus();
	bool getObservesForProcessed() {return observesForProcessed;}
	int getCollThr() {return collectThreshold;}
	int getCollMax() {return collectMax;}
	//void setNumOfObsAgentsToZero();
	int clearNumOfObservedAgents();

	//-------methods for test purpose: ----------------
	void TESTsetBoughtMessage(Message* m);

protected:
	//int getVicinityBus();
	int getNumOfTypeDNAbits();
	bool checkForNewObserveConnection(Agent* newObserverInNB);
	

private:
	Observable* makeBaby(Environment*, IdStamp*,Grid*, GridCell*, SyncManager*, int fitn, Message* appMat, Message* appBus, Message* agent, Message* type);
	bool makeBusinessConnections(); //returns true if matches found
	bool isOkBusApp(Observable* possNewBusConn);
	CList<Observable*,Observable*>* removeNonLegalBusiness(); //Remove non-observers and collectors
	CList<Observable*,Observable*>* findOkBusAppear(CList<Observable*,Observable*>* subj); //Find subjects that match appearBus
	//Message* getOffspringPart(int repPosStart, int repPosStop, int partPosStart, int partPosStop, Message* thisDNA, Message* mateDNA);

	//Message* EMPTYMESS;
	int numOfObsAgents;
	Message* boughtMessage;
	Message* collectedMessage;
	//int vicinityBus;

	//---------Collector DNA------------
	Message* collectorDNA;
	//Message* appearBus;
	Message* appIdealBus;
	Message* appPickiBus;
	bool observesForProcessed; //only used when obs inpoders; true = only proc mess.
	int collectThreshold; //when the collected mess size is => collectThreshold pState is true otherwise false
	int collectMax; //when the collected message size is > collectMax it is discarded if not sold (at most about 25 because of max mess size of 32 bit)

	//----------------------------------

};

#endif // !defined(AFX_DCOLLECTOR_H__A174BF22_6FEF_4261_B88C_38522C334D7C__INCLUDED_)
