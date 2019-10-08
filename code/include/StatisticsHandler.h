// StatisticsHandler.h: interface for the StatisticsHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATISTICSHANDLER_H__5DBC75E2_3BA9_4726_BA0B_0BF321E01DD4__INCLUDED_)
#define AFX_STATISTICSHANDLER_H__5DBC75E2_3BA9_4726_BA0B_0BF321E01DD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "Observable.h"
#include "UIntGrid.h"
//#include "Environment.h"
//#include "WndCollineText.h"
//#include "WndColline.h"
//#include "GridCell.h"
//#include "Observable.h"
#include "Grid.h"

class StatisticsHandler  
{
public:
	StatisticsHandler(bool isNewOrganism, int restoredSesReward, /*Environment* environ,*/ Grid* aGrid, byte fIDVal, int restDnaSamNum);
	virtual ~StatisticsHandler();
	void accumulateBorn(); //move to adjustAgents
	void accumulateDead();
	void adjustFitnessEnvironment(int change);
	//void adjustFitnessBuffer(bool bufferType, int change);
	void adjustFitnessColline(int change, int type);
	void adjustAgents(int value, int type);
	void adjustObservers(int value, int type);
	void adjustDrifters(int value, int type);
	void adjustProcessors(int value, int type);
	void adjustMaters(int value, int type);
	void addToNumOfProducedBits(int numOfBits); //for training statistics
	void addToNumOfProcessedBits(int numOfBits); //for training statistics
	void addToNumOfEatenBits(int numOfBits);
	void addToNumOfDiscUBits(int numOfBits); //for training statistics
	void addToNumOfDiscPBits(int numOfBits); //for training statistics
	void addToNumBitsFromEnv(int numOfBits);
	//void numOfProcEffectorsAdd();
//	void addToNumOfProcEffectors_total(); //accumulates all proc. effectors in session (also the ones not seen by decCat)
	void addToNumOfProcEffectors(int bidAgrLevel, int cIndex);
	void addToFitnessPaid(int value, int pLevel, int cIndex);
	void addToCorrectResp(int pLevel, int cIndex);
	void addToWrongResp(int pLevel, int cIndex);

	void setLastSesNumOfProcEff(UIntGrid* lastSesValues);
	void setLastSesFitnessPaid(UIntGrid* lastSesValues);
	void setLastSesCorrectResp(UIntGrid* lastSesValues);
	void setLastSesWrongResp(UIntGrid* lastSesValues);
	void resetSessionData( bool newLoadedCat );
	//void setThisSesCorrectCategory(bool cat);
	void setThisSesChosenPickiLevel(int chosenLevel);
	void setThisSesEstimatedOkResp(int val, int level);
	void setThisSesRewardOnLevels(UIntGrid* values);
	void setThisSesFitnessBuffer(bool bufferType, int buffer);
	void setThisSesMatingBonusBuffer(int value);
	//void setEnvironment(Environment* e) {env = e;}//move to constructor! (problems when tried)
	void updateSessionReward();
	void setSessionCategory(bool answer);
	bool getSessionCategory();
	int getSessionReward();
	int getLastCorrectRespOnLevel(int level); //returns # of correct effectors on level
	int getLastCorrectRespOnLevel(bool cat, int level); //returns the last # of correct effectors on level and category
	int getLastCorrectRespAtCollIndex(bool cat, int collIndex);
	//int getLastEarlyCorrectResp(bool cat);
	int getLastWrongRespOnLevel(int level); //returns # of wrong effectors on level
	int getLastRespOnLevel(int level); //returns # of effectors processing on level
	int getLastCorrectRespTotal();
	int getLastCorrectRespTotal(bool cat);
	int getThisCorrectRespTotal(); //returns number of correct resp in this session (sofar)
	int getThisFitnessPaidTotal(); //returns amount of fitness paid sofar in session
	int getMaxFitnRewSess();
	int getFitnessColline();
	int getNumOfAgents();
	int getPerformance(bool category); //returns performance for cat. rounded to int
	//void updateAgentView(int cNum );
	//void updateTrainingView(int sNum );
	void changeGridView(Observable* changingAgent, GridCell* oldPos); //called by agents when they move or change color
	//void updateGridView();
	//void updateAllViews(int cNum, int sNum);
	void saveCycleData(int cycle); //saves data to file (for graph display in matlab)
	void saveSessionData();
	void storePopulation(int cycNum, int sesNum, int matBonusBuff, int fitnBuff0, int fitnBuff1, int idCountVal); //saves dna info, fitness and position for all agents for later restoring
	//void restorePopulation(); //restores a previously saved population (all starts as drifters)
	void closeFiles();
	void saveDNAinfo(); //saves dna information from a sample of the agents
	void updateCurrentSpeciesCount(int type, Message* appMating, Message* appIdeal, bool isNewBorn);
	void updateAvgAgentAge(int dyingAge);
	void updateAvgStateBuyers_inp(int numOfBuyers);
	void updateStatAgentBorn(Observable* nAgent);
	void updateStatAgentDies(Observable* dyingAgent);
	int getEnvFitnIndex();
	int getSysFitnIndex();
	int calcSpeciesNum(Message* appMating, Message* appIdeal);

	//**********************
	//Adjustable parameters
	int MAX_STATE_BUYERS_INP;
	int BIDP_BASE_INP;
	int BIDP_STEP_INP;
	int BIDP_BASE_EFF;
	int BIDP_STEP_EFF;
	int SESSION_LENGTH;
	int AGENT_LIFETIME_MAX;
	int COLL_MIN_PROCTIME;
	int COLL_MAX_PROCTIME; //(computed)
	int GRID_ZOOM;
	int C_MATSTART_PRC;
	int DC_REW_FROM_BID_AGR_LEVEL;
	int MAX_INACTIVE_SESS;
	int MUTATION_RATE;
	int HIGH_MUT_AGENTS_PRC;
	int MIN_NUM_OF_AGENTS_IN_GRID;
	int IDEAL_NUM_OF_AGENTS_IN_GRID;
	int GRAVITY_CENTER;
	int DC_EARLY_RESP_REW_I0_PRC;
	int DC_EARLY_RESP_REW_I1_PRC;
	int DC_EARLY_RESP_REW_I2_PRC;
	int MAX_NUM_OF_OBS_INP;
	int MAX_NUM_OF_OBS_EFF;
	//end adjustable parameters
	//*************************

	int numOfObservers;
	int numOfObs_foun, numOfObs_coll, numOfObs_inp, numOfObs_eff;
	int numOfDrifters;
	int numOfDrif_foun, numOfDrif_coll, numOfDrif_inp, numOfDrif_eff;
	int numOfProcessors;
	int numOfProc_foun, numOfProc_coll, numOfProc_inp, numOfProc_eff;
	int numOfMaters;
	int numOfMaters_coll, numOfMaters_inp, numOfMaters_eff;
	int numOfNonMaters;
	int agentsAlive; //needed?
	int numOfAgents;
	int numOfAgents_coll;
	int numOfAgents_inp;
	int numOfAgents_eff;
	int numOfAgents_foun;

	int numOfCatZero;
	int numOfCatOne;
	int prevAgentsAlive;
	double performance_cat0; //the value gives number of correct cat0 answers in last N sessions, where N depends on FORGET_FACTOR_PERF
	double performance_cat1;
	int fitnessEnvironment; //fitness in env incl. buffer located in DecisionCategory
	//int fitnessBuffer0_cp; //is placed in decCat
	//int fitnessBuffer1_cp;
	int fitnessColline; //total fitness in system
	int fitnessColline_coll;
	int fitnessColline_inp;
	int fitnessColline_eff;

	bool lastSesCorrectCategory;
	bool lastSesCollineAnswer;
	int lastSesChosenPickiLevel;
	int lastSesBitsProduced;
	int lastSesBitsProcessed;
	int lastSesBitsEaten;
	int lastSesBitsFromEnv;
	int lastSesBitsDiscU;
	int lastSesBitsDiscP;
	int lastSesFitnBalance_end; // fitness_colline - fitness_env (calculated at _end_ of session), negative: colline needs fitness points
	int lastSesFromEnvToBuff;
	int lastSesFitnessBuffer0_cp; //the buffer used to reward agents last session
	int lastSesFitnessBuffer1_cp;
	int lastSesMatingBonusBuffer_cp;
	UIntGrid* lastSesNumOfProcEff; //contains the number of processing effectors in last session
	UIntGrid* lastSesFitnessPaid;
	UIntGrid* lastSesCorrectResp;
	UIntGrid* lastSesWrongResp;
	UIntGrid* lastSesNumOfCorr_EstForNext;
	UIntGrid* lastSesRewardOnLevels;
	UIntGrid* lastCorrectRespCatZero; //used to estimate num of resp for next cat zero session
	UIntGrid* lastCorrectRespCatOne; //used to estimate num of resp for next cat one session

	std::vector<unsigned int> nextViewFrame;

	UIntGrid* currentSpeciesCount; //{ [Coll] ; [Inp] ; [Eff] }
	UIntGrid* speciesStat_col;
	UIntGrid* speciesStat_inp;
	UIntGrid* speciesStat_eff;
	double avgAgentAge;
	double percThatReachMaxAge; //the percentage of agents that dies because they reach MAX age
	double avgStateBuyers_inp;
	double percAuctionsThatHitMax_inp;

private:

	std::string numToString(int number);
	int getAverage(int fValue, int agents);
	void pressSpaceToQuit();
	void pressSpaceOrQuit();
	bool getCollineAnswer();
	void writeConstants();
	void saveDNAinfo_common(FILE* f, Observable* agent);
	void saveDNAinfo_coll(std::list<Observable*> sample);
	void saveDNAinfo_inp(std::list<Observable*> sample);
	void saveDNAinfo_eff(std::list<Observable*> sample);
	std::list<Observable*> pickSample(std::list<Observable*> fullList, int sampleSize);


	Grid* agentGrid;
	int dnaSampleNum;
	//int max_fitn_rew_sess; //the maximum fitness that Colline can receive in one session (to prevent unstability)
	std::string fname_begin;
	
	//int sessionNum;
	//WndCollineText* agentView;
	//WndCollineText* trainingView;
	//WndColline* gridView;

	bool thisSesCorrectCategory;
	bool thisSesCollineCategory;
	int thisSesChosenPickiLevel;
	int thisSesBitsProduced; //the total bits from fountains + bit produced by effectors
	int thisSesBitsProcessed; //the total number of 'eaten' bits by inpoders and effectors in training-session
	int thisSesBitsEaten;
	int thisSesBitsDiscU; //accumulates the number of unprocessed discarded bits from inpoders, collectors and effectors for each training session
	int thisSesBitsDiscP; //acc. num of processed bits from inpoders
	int thisSesBitsFromEnv;
	int thisSesFitnessBuffer0_cp; //the buffer used to reward agents this session
	int thisSesFitnessBuffer1_cp;
	int thisSesMatingBonusBuffer_cp;
	UIntGrid* thisSesNumOfProcEff;
	UIntGrid* thisSesFitnessPaid; //to effectors from environment
	UIntGrid* thisSesCorrectResp;
	UIntGrid* thisSesWrongResp;
	UIntGrid* thisSesNumOfCorr_EstForNext;
	UIntGrid* thisSesRewardOnLevels;

	int thisBlockSessionReward; //a block is SESS_BETW_UPDATE sessions


	int testNumOfProcEffectors;

	int accBorn;//for control (incl. fountains)
	int accDead;//for control

//	Environment* env;

	FILE* file_constants;
	//cycle data:
	FILE* file_fitnColline;
	FILE* file_fitnBuff;
	FILE* file_fitnEnv;
	//session data:
	FILE* file_maters;
	FILE* file_agents;
	FILE* file_bits_eaten;
	FILE* file_correct_resp;
	FILE* file_wrong_resp;
	FILE* file_category;
	//remember genetic info

};

#endif // !defined(AFX_STATISTICSHANDLER_H__5DBC75E2_3BA9_4726_BA0B_0BF321E01DD4__INCLUDED_)
