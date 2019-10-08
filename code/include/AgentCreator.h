// AgentCreator.h: interface for the AgentCreator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGENTCREATOR_H__8D8A709B_D00F_4C0B_ACCA_6EFE6279AC03__INCLUDED_)
#define AFX_AGENTCREATOR_H__8D8A709B_D00F_4C0B_ACCA_6EFE6279AC03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "Observable.h"
#include "Environment.h"
#include "StatisticsHandler.h"
#include "SyncManager.h"
#include "Message.h"
#include "Grid.h"
#include "IdStamp.h"
#include "DecisionCategory.h"

class AgentCreator  
{
public:
	AgentCreator(Environment* e, StatisticsHandler* sh, SyncManager* sm, Grid* aGrid, IdStamp* iStamp, DecisionCategory* dc);
	virtual ~AgentCreator();
	void restorePopulation(std::string fname_begin);
	void createFountains();
	void createCollectors();
	void createInpoders();
	void createEffectors();

private:
	void pressSpaceToQuit();
	void pressSpaceOrQuit();
	int getRandNumBetwZeroAnd(int maximum);
	Environment* env;
	StatisticsHandler* statHandler;
	SyncManager* syncMan;
	Grid* agentGrid;
	IdStamp* stamp;
	DecisionCategory* decCat;

};

#endif // !defined(AFX_AGENTCREATOR_H__8D8A709B_D00F_4C0B_ACCA_6EFE6279AC03__INCLUDED_)
