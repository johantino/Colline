// Observable.h: interface for the Observable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBSERVABLE_H__FD9FF8C9_A3BF_4D1C_9971_88BFF5FD613A__INCLUDED_)
#define AFX_OBSERVABLE_H__FD9FF8C9_A3BF_4D1C_9971_88BFF5FD613A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "Konst.h"
#include <afxtempl.h> //used for CList
#include "Message.h"
#include "IdStamp.h"
#include "Environment.h"
//#include "StatisticsHandler.h"

//#include "GridCell.h"
//#include <vector>
//using std::vector;

class Observable  : public CObject
{
public:
	Observable();
	Observable(Environment* e, IdStamp* collStamp, int agentType, Message* appearanceBusiness); //used when constructing inpoders or effectors (agents)
	virtual ~Observable();
	void notifyObserversProc();
	void notifyObserversState();
	void notifyObserversUnproc();
	void deleteBusinessConnections(); //cuts connection to potential sellers and buyers
	virtual void turnFocusTo(Observable*); //Move to Agent.h!!!
	virtual void process();
	virtual void drift();
	virtual bool hasOneStateMessage();
	virtual int auctionWon(Observable* seller, Message* boughtMessage);
	virtual int getBid();
	virtual void observedAgentIsDrifting(Observable* observedAgent); //move to Agent.h!!!
	void potentialBuyerIsDrifting(Observable* leavingAgent);
	Message* getAppearBusFull(); //returns business appearance (first two bits depend on agenttype)
	void addToAgentsObserved(Observable* potentialSeller);
	void removeFromAgentsObserved(Observable* leavingAgent);
	int getNumOfPotentialSellers(); //returns # of agents that _this_ agent is observing
	int getNumOfPotentialBuyers(); //returns # of agents observing _this_ agent
	void addToProcObservers(Observable* potentialBuyer); //observer can be any of the three agent types
	void addToStateObservers(Observable* potentialBuyer); //observer is effector
	void addToUnprocObservers(Observable* potentialBuyer); //observer is collector
	virtual void addToBidders(Observable* bidder);
	Message* getAppearBusSmall(); //returns bus. appearance except the two first agenttype bits
	int getId();
	bool agentIsObserved(Observable* nbAgent); //returns true if nbAgent is observed
	//Environment* getEnvironment();
	virtual int getStatus();          // returns ST_DRIFTER, ST_PROCESSOR, ST_OBSERVER or ST_DEAD 
	virtual int getStatusAndActive(); // as getStatus + status ST_FINISHED_THIS_CYCLE
	//virtual GridCell* getPosition();
	bool isFountain(); //not good style
	bool isEffector(); // - do - 
	bool isInpoder();
	bool isCollector();
	int getType(); //TYPE_FOUNTAIN, TYPE_COLLECTOR, TYPE_INPODER or TYPE_EFFECTOR
	//char getStatus();
	void pressSpaceToQuit();
	void pressSpaceOrQuit();
	virtual std::string toString();
	std::string toStringType();
	std::string toStringId();
	std::string toStringAgentsObserved();
	virtual int clearNumOfObservedAgents(); //returns num of agents cleared
	//void setStatusForNextDrifter();
	//void setStatusForNextObserver();
	//void setStatusForNextProcessor();
	//SyncManager* syncMan; //move to private?
	//virtual void setNumOfObsAgentsToZero();

	//int testDEBUG;

protected:
	Environment* env;
	IdStamp* collineStamp; //a stamp object exist (shared) to give id numbers to new born agents
	std::string toStringObservedBy(); //gives the id's of the agents observing this one
	void setStatus(int status);
	std::string toStringAppBus(); //DELETE
	
	virtual std::string toStringAgentSpec();
	virtual std::string toStringTypeSpec();

private:
	std::string toStringStatus();
	void clearPotentialSellers();
	void clearPotentialBuyers();
	int id;
	int agentStatus; //Drifter, processor or observer (see const declaration)

	void notifyObservers(CTypedPtrList<CObList, Observable*>* listOfObs); //When object has processed. Executes turnFocusTo in observers.
	void informAndClearObservers(CTypedPtrList<CObList, Observable*>* listToBeCleared);
	
	CTypedPtrList<CObList, Observable*>* agentsObserved;//_this_ observes these agents
	
	CTypedPtrList<CObList, Observable*>* observersProc;  //agents observing for my processed message
	CTypedPtrList<CObList, Observable*>* observersUnproc;//agents observing for my unprocessed message
	CTypedPtrList<CObList, Observable*>* observersState; //agents observing for my state 

	//StatisticsHandler* statHandler;
	//---------------------------------------------------
	int type; //fountain, collector, inpoder or effector
	//-------DNA determined attributs:------------------
	Message* appearBusSmall;
	Message* appearBusFull;

};

#endif // !defined(AFX_OBSERVABLE_H__FD9FF8C9_A3BF_4D1C_9971_88BFF5FD613A__INCLUDED_)
