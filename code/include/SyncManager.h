// SyncManager.h: interface for the SyncManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYNCMANAGER_H__93C12759_CE9B_4F65_9F66_3C53AC01D534__INCLUDED_)
#define AFX_SYNCMANAGER_H__93C12759_CE9B_4F65_9F66_3C53AC01D534__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Observable.h"
#include "Environment.h"
#include "StatisticsHandler.h"
#include "IdStamp.h"
#include "WndCollineText.h"
#include "WndColline.h"

//using jf_w3_agent::Agent;

//namespace jf_w3_sync {

class SyncManager  
{
public:
	SyncManager(IdStamp* stamp, Environment* e, StatisticsHandler* sh, WndColline* gView, WndCollineText* aView, WndCollineText* tView, WndCollineText* vsView, int restCycNum, int restSesNum);
	virtual ~SyncManager();
	int getCycleNum(); //returns cycle number
	int getSessionNum();
	int getCollectionIndex(); //returns a number between 0 and maxCollectionSpan; or -1 if the system is not in decision-collection-mode
	void isProcessorNextCycle(Observable* ag);
	void isDrifterNextCycle(Observable* ag);
	//void isObserverNextCycle(Observable* ag);
	void isDeadNextCycle(Observable* ag);
	void addFountain(Observable* f); //fountain position in list determines which pipe it is assigned
	void addAgentToList(Observable* newbornAgent);
	void removeAgentFromList(Observable* deadAgent);
	//void addIdToIdTagList(int newbornAgentId);
	//void removeIdFromIdTagList(int deadAgentId);
	
	//void removeIdFromHasNotBeenDrifterList(int agentId);
	void run();
	void setDecCat(Observable* decCat); //sets decCategory...FIX (mezzy programming)
	int getRandNumBetwZeroAnd(int maximum);
	//void changeGridView(Observable* changingAgent, GridCell* oldPos); //called by agents when they move or change color

private:
	void quit();
	int press12345SaveOrQuit();
	void adjustParameters();

	void updateAllViews();

	void setHasBeenDrifterFlags();
	void loadFountains();
	void loadFountain(int fNum); //only loads fountain fNum
	void initiateProcessors();
	void initiateDrifters();
	void deleteDeadAgents();
	void deleteGrid();
	void prepareForNextCycle();
	void prepareForNextSession();
	void shakeFrozenAgents(int chanceToDrift); //this method sets status of some frozen agents to drifters

	int loadingBeginCycle1st;
	int loadingBeginCycle2nd;
	int loadingBeginCycle3rd;
	bool isLoadingFountains1st; //true during first message inflow from environment
	bool isLoadingFountains2nd;
	bool isLoadingFountains3rd;

	int cycleNum;
	int sessionNum;
	int nextStopSession;

//	bool stopSystem;
	
	//int cycleCollection; //between 0 and maxTimeSpan-1; or -1 if outside collection area
	
	//int gridWidth;
	//int gridHight;

	WndCollineText* agentView;
	WndCollineText* trainingView;
	WndCollineText* varStatView;
	WndColline* gridView;

	IdStamp* systemIdStamp;
	Environment* env;
	StatisticsHandler* statHandler;
	Observable* decCategory; //FIX (mezzy programming)
	//These lists keeps track on the status
	CTypedPtrList<CObList, Observable*>* currentProcessors; //CList<Observable*, Observable*>* currentProcessors;
	CTypedPtrList<CObList, Observable*>* currentDrifters;   //CList<Observable*, Observable*>* currentDrifters; 
	CTypedPtrList<CObList, Observable*>* nextProcessors;    //CList<Observable*, Observable*>* nextProcessors;
	CTypedPtrList<CObList, Observable*>* nextDrifters;      //CList<Observable*, Observable*>* nextDrifters;
	CTypedPtrList<CObList, Observable*>* nextDead;          //CList<Observable*, Observable*>* nextDead;
	CTypedPtrList<CObList, Observable*>* fountains;         //CList<Observable*, Observable*>* fountains;

	//CList<Observable*, Observable*>* list_hasNotBeenDrifters; //this list contains agents that has not been drifters at end of session (frozen agents)
	CTypedPtrList<CObList, Observable*>* list_currentAgents; //CList<Observable*, Observable*>* list_currentAgents;

	//CList<int,int>* idTags_currentAgents; //this list contains the id's of the living agents in grid (used for test/debugging)
	//CList<int,int>* testIdList_hasNotBeenDrifters; //list contains agent-id's that have not yet been drifters (in a given period) - for test/debugging
};

//}//namespace

#endif // !defined(AFX_SYNCMANAGER_H__93C12759_CE9B_4F65_9F66_3C53AC01D534__INCLUDED_)
