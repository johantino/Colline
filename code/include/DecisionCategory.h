// DecisionCategory.h: interface for the DecisionCategory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DECISIONCATEGORY_H__145AF635_3F73_4D07_9BAC_055EC34990B2__INCLUDED_)
#define AFX_DECISIONCATEGORY_H__145AF635_3F73_4D07_9BAC_055EC34990B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Message.h"
#include "UIntGrid.h"
#include "IdStamp.h"
#include "SyncManager.h"
#include "StatisticsHandler.h"
#include "Observable.h"
#include "Environment.h"

class DecisionCategory : public Observable  
{
public:
	DecisionCategory(int initialFitnBuff0,
					int initialFitnBuff1,
					Environment* e,
					StatisticsHandler* sh,
					IdStamp* cStamp,
					SyncManager* s,
					int maxCS
					);
	virtual ~DecisionCategory();
	//Message* getIdealBA();
	void observedAgentIsDrifting(Observable* leavingAgent);
	void turnFocusTo(Observable* eff); //register all notifiers in array, only bid for chosen pickiLevel
	int auctionWon(Observable* seller, Message* boughtMessage);
	int getBid();
	UIntGrid* getNumOfProcEffec();
	UIntGrid* getNumOfOkRespAgents();
	UIntGrid* getNumOfNotOkRespAgents();
	UIntGrid* getPaidFitnessOnLevel();
	int getFitnessBuffer_env(bool bufferType);
	int getChosenPickiLevel();
	void setCurrentCorrectAnswer(bool answer);
	//bool getCollineAnswer();
	//void setCurrentSessionRewardTotal(int value); //is used every SESS_BETW_REWARD_UPDATE sessions
	void fillFitnessBuffer_env(bool bufferType);
	void giveRemainCatBuffToMatBonusBuff(bool bufferType);
	void setRewardsOnLevels(); //calculate the reward to each ok-resp. effector, use number responding last session as estimate and to find new min-level 
//	void chooseLevel(); //based on response in last session
	bool hasOneStateMessage(){return false;}
	//--------------methods for test:-----------------
	bool TESTgetCurrentCorrectAnswer();

private:
	//int get2dIndex(int row, int column); //returns correct index for use in 'numOfRespAgents'
	int getEstimatedNumOfOkRespEff(bool loadedCat, int level); //returns an estimate for the num of responding effectors on picki level
	//void makeBid(Observable* seller);
	void observe(Observable* eff);
	void setFitnessBuffer_env(bool bufferType, int value);
	void setLateResponseRewardPRC();
	//bool currentCorrectAnswer; //the correct answer for this category; updated for each training session
	int bid; //the bid are updated after each training session to match expected number of correct resp. agents 

	int fitnessBuffer0_env;//before each session were answer is 0 this money bag is filled with 'sessionRewardTotal' fitness points from env, used by decCat to reward effectors
	int fitnessBuffer1_env;// do.   -    -       -    -         1
	int bufferAtStartOfSession;
	int maxRewardThisSession;
	int lateResponseRewardPRC; //late responses gets a bonus
	//int remainingFitnessFromLastSession; //can be negative if more effectors than expected responded in last session

	SyncManager* syncMan;
	StatisticsHandler* statHandler;
	//Environment* env; //to access moneyBag
	//int collTimeSpanMax; //the max num of cycles, over which the effector response is collected
	int numOfPickiLevels; //is the same as size of idealBusApp+1, eg. 5: level 0=[0####], level 1=[10###], level 2=[110##] ...level 5=[11111]
	int chosenPickiLevel; //the level used in current cycle to determine which of the auctioning effectors to make bid to
	//int maxCollectionSpan; //the time span (in cycles) where answers are collected
	//Message* idealBusApp; //note two first bits must match effector type: [11]
	//UIntGrid* numOfProcEffec; //2D array rows=pickiLevel, colums=cycle

	//UIntGrid* numOfOkRespAgents; //2D array
	//UIntGrid* numOfNotOkRespAgents; //2D array (isnt given from numOfResp and numOfOKResp, since not all auctions are won)

	//UIntGrid* paidFitnessOnLevel;
	UIntGrid* rewardOnLevel; //contains the reward used in session, for each level
	bool bufferZero_reached; //when buffer is less than zero, bids are cut by 50% (until fitnessPaidMax_reached)
	bool fitnessPaidMax_reached;
	bool fitnPaidMax_reached_cat0;
	bool fitnPaidMax_reached_cat1;
	bool numOfOkRespMax_reached;

	Message* effAppCrit_ideal_wholeGrid;
	Message* effAppCrit_ideal_leftGrid;
	Message* effAppCrit_ideal_rightGrid;
	Message* effAppCrit_ideal_upperLeft;
	Message* effAppCrit_ideal_lowerLeft;
	Message* effAppCrit_ideal_upperRight;
	Message* effAppCrit_ideal_lowerRight;
	Message* effAppCrit_picki_wholeGrid;
	Message* effAppCrit_picki_divInTwo;
	Message* effAppCrit_picki_divInFour;
};

#endif // !defined(AFX_DECISIONCATEGORY_H__145AF635_3F73_4D07_9BAC_055EC34990B2__INCLUDED_)
