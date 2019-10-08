// Environment.h: interface for the Environment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENVIRONMENT_H__3F128B7E_F9F9_4B1E_A76E_4637675E4B30__INCLUDED_)
#define AFX_ENVIRONMENT_H__3F128B7E_F9F9_4B1E_A76E_4637675E4B30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "Konst.h"
#include "Message.h"
//#include "StatisticsHandler.h"
#include <afxtempl.h> //used for CList

class Environment  
{
public:
	Environment(int initialMoneyBag, int initialMatBonusBuff);
	virtual ~Environment();
	void receiveFitness(int amount);
	int payFitness(int amount);
	Message* getNextMessageFromPipe(int pipeNum); // When method is called the next mess is returned (is empty if none in pipe)
	int readFitness();
	bool getLoadedCategory();
	std::string getLastMessageString();
	void fillPipesZerosOrOnes(bool loadCat); //when all pipes are empty this method is envoked
	bool fillPipesZerosOrOnes(int sessNum, int perf_cat0, int perf_cat1);
	void fillPipesAOrNotA(int sessNum);
	bool fillPipesCircleOrNotCircle(int sessNum, int perf_cat0, int perf_cat1);//returns loaded category
	void fillPipesCircleOrNotCircle(bool loadCategory); 
	void fillPipesCrossOrRandom();
	void setFitness(int value);
	void receiveMatingBonus(int bonus); //when decCat didnt use reward buffer for the loaded category, the rest goes here
	int giveMatingBonus(int agentInitFitn, int agentCurrFitn); //called by agents when getting maters
	int getMatingBonusBuffer(); //read buffer
	int getRandNumBetwZeroAnd(int maximum);

private:
	Message* getAMessage(int rowNum, int aPosition);
	Message* getCircleMessage(int rowNum, int circlePosition);
	void toStringLastMessage();
	int fitness;
	int matingBonusBuffer;
	int numOfPipes;
	int lastSessCat0;
	int lastSessCat1;
	bool loadedCategory; //the answer (category) for the message(s) transmitted most recently
	std::string lastMessageString;
	Message* EMPTYMESS;
	CList<Message*, Message*>* nextMessInPipes;
	//StatisticsHandler* statHandler;

};

#endif // !defined(AFX_ENVIRONMENT_H__3F128B7E_F9F9_4B1E_A76E_4637675E4B30__INCLUDED_)
