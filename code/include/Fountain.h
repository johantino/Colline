// Fountain.h: interface for the Fountain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FOUNTAIN_H__B0A44185_E69B_49E9_B216_19C2BA6A2EAC__INCLUDED_)
#define AFX_FOUNTAIN_H__B0A44185_E69B_49E9_B216_19C2BA6A2EAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "Observable.h"
#include "Agent.h"
#include "Environment.h"
#include "SyncManager.h"
#include "StatisticsHandler.h"

class Fountain : public Observable  //Fountain does not inherit agent methods (a fountain is a 'disguised' agent)
{
public:
	Fountain(Environment* e,
		StatisticsHandler* sh,
		IdStamp* cStamp,
		Grid* cGrid,
		GridCell* pos,
		SyncManager* s,
		Message* observablePart,
		int lSlot
		);
	virtual ~Fountain();
	void addToBidders(Observable* bidder);
	void receiveMessage(Message* newMess); //from environment
	int getStatusAndActive();
	CString toString();
	void process();
	void drift();
	int getLoadingSlot();
	GridCell* getPosition();
	void setHasBeenActiveInCycle(bool value){hasBeenActiveInSession = value;}


	CTypedPtrList<CObList, Observable*> bidders; // CList<Observable*, Observable*>* bidders; //
	//CPtrList bidders;

	//int testDEBUG;

private:
	Agent* findHighestBidder(); //move to obs?
	void setStatusForNextObserver();
	void setStatusForNextProcessor();
	void setStatusForNextDrifter();
	bool sellMessage(Message* mess);
	//int sellState(bool val);

	Grid* collineGrid;
	GridCell* position;
	int lastStatusUpdateInCycle;
	int loadingSlot;

	Message* EMPTYMESS;
	Message* messFromEnv;
	//Environment* env;
	SyncManager* syncMan;
	StatisticsHandler* statHandler;
	bool hasBeenActiveInSession;

};

#endif // !defined(AFX_FOUNTAIN_H__B0A44185_E69B_49E9_B216_19C2BA6A2EAC__INCLUDED_)
