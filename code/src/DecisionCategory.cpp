// DecisionCategory.cpp: implementation of the DecisionCategory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Colline.h"
#include "DecisionCategory.h"
#include "Effector.h"
#include <iostream>
#include "Konst.h"
#include "Utilities.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//NOTE...shouldn't be derived from observable FIX later (although syncManager must 'observe' it)
DecisionCategory::DecisionCategory(int initialFitnBuff0, int initialFitnBuff1, Environment* e, StatisticsHandler* sh, IdStamp* cStamp, SyncManager* s, int maxCollectionSpan) : Observable(e, cStamp, TYPE_CATEGORY, new Message(0,0) )
{
	syncMan = s;
	statHandler = sh;
	rewardOnLevel = new UIntGrid( pow(2,BIDLEVELS_LENGTH), 1); //consider dependence on collectionIndex too
	effAppCrit_ideal_wholeGrid = new Message(63, APPEAR_SIZE_BUS); //11 1111
	/*effAppCrit_ideal_leftGrid = new Message(112, APPEAR_SIZE_BUS);  //11 10**
	effAppCrit_ideal_rightGrid = new Message(60, APPEAR_SIZE_BUS); //11 11**
	effAppCrit_ideal_upperLeft = new Message(112, APPEAR_SIZE_BUS); //11 100*
	effAppCrit_ideal_lowerLeft = new Message(58, APPEAR_SIZE_BUS); //11 101*
	effAppCrit_ideal_upperRight = new Message(60, APPEAR_SIZE_BUS);//11 110*
	effAppCrit_ideal_lowerRight = new Message(62, APPEAR_SIZE_BUS);*///11 111*
	effAppCrit_picki_wholeGrid = new Message(56, APPEAR_SIZE_BUS); //111***
	//effAppCrit_picki_divInTwo = new Message(60, APPEAR_SIZE_BUS);
	//effAppCrit_picki_divInFour = new Message(62, APPEAR_SIZE_BUS);
	//-------initialize values:----------------
	bid = 2000;
	lateResponseRewardPRC = 100;
	fitnessBuffer0_env = 0;
	fitnessBuffer1_env = 0;
	setFitnessBuffer_env(0, initialFitnBuff0); //is 0 if new organism
	setFitnessBuffer_env(1, initialFitnBuff1);
	chosenPickiLevel = 1;
	syncMan->setDecCat(this); //FIX
	bufferZero_reached = false;
	fitnessPaidMax_reached = false;
	fitnPaidMax_reached_cat0 = false;
	fitnPaidMax_reached_cat1 = false;
	numOfOkRespMax_reached = false;
	bufferAtStartOfSession = 0;
	maxRewardThisSession = 0;
}

DecisionCategory::~DecisionCategory()
{

}

void DecisionCategory::setFitnessBuffer_env(bool bufferType, int value) {
	int change = value - getFitnessBuffer_env(bufferType);
	//statHandler->adjustFitnessBuffer(bufferType, change);
	if (bufferType == 0)
		fitnessBuffer0_env = value;
	else
		fitnessBuffer1_env = value;
}

void DecisionCategory::fillFitnessBuffer_env(bool bufferType) {
	int fromEnv = statHandler->getSessionReward();
	int maxBufferSize = fromEnv*DC_MAX_BUFFERSIZE_MUL;
	if ((fromEnv + getFitnessBuffer_env(bufferType)) > maxBufferSize)
		fromEnv = maxBufferSize - getFitnessBuffer_env(bufferType);
	setFitnessBuffer_env(bufferType, getFitnessBuffer_env(bufferType) + env->payFitness( fromEnv ));
	statHandler->adjustFitnessEnvironment( -fromEnv);
	//std::cout << "fitnessBuffer_env for rewards next session: " << fitnessBuffer_env << std::endl;
	statHandler->setThisSesFitnessBuffer(false, fitnessBuffer0_env);
	statHandler->setThisSesFitnessBuffer(true, fitnessBuffer1_env);
	bufferAtStartOfSession = getFitnessBuffer_env(bufferType);
	maxRewardThisSession = ((DC_ALLOWED_OVERPAY_PRC+100) * getFitnessBuffer_env(bufferType))/100;
}

//not used
void DecisionCategory::giveRemainCatBuffToMatBonusBuff(bool bufferType) {
	int remainingFitnInBuff = getFitnessBuffer_env(bufferType); //whats left after training session
	if (remainingFitnInBuff>0) {
		env->receiveMatingBonus(remainingFitnInBuff);
		setFitnessBuffer_env(bufferType, 0);
		statHandler->setThisSesMatingBonusBuffer( env->getMatingBonusBuffer() ); //(strictly not this session, but next session)
	}
}

/*void DecisionCategory::chooseLevel() {
	return; //NB! the pickiLevel is ignored
	int okTotal = statHandler->getLastCorrectRespTotal( env->getLoadedCategory() );
	//std::cout << "number of OK responses total: " << okTotal << std::endl;
	if (okTotal < DC_MIN_OK_RESP_EFF) {
		chosenPickiLevel--;
		if (chosenPickiLevel<1)
			chosenPickiLevel=1; //level 0 reserved for internal processing
	} else if (okTotal > DC_MAX_OK_RESP_EFF) {
		chosenPickiLevel++;
		if (chosenPickiLevel>(NUM_OF_PICKI_LEVELS - 1))
			chosenPickiLevel = NUM_OF_PICKI_LEVELS - 1;
	}
	//std::cout << "DEBUG: pickilevel = " << chosenPickiLevel << std::endl;
}*/

//NOTE: make method to calc estimated number of OK resp
//(actually paid OK's is used to estimate next reward value if potentially more OKs exist use lower reward)
void DecisionCategory::setRewardsOnLevels() {
	//assumes that fitnessBuffer_env (0 or 1) has been updated
	//picki level has just been chosen and pipes are filled with new category data
	//assumes that statistics are transferred to 'lastSes'-data
	bool loadedCategory = env->getLoadedCategory();
	int rewardsFromBidLevel = statHandler->DC_REW_FROM_BID_AGR_LEVEL;
	int numOfLevelsObserved = NUM_OF_BID_LEVELS - rewardsFromBidLevel;
	int totalRewardForEachLevel = getFitnessBuffer_env(loadedCategory)/ numOfLevelsObserved;
	
	int level;
	int estNumOfEffOK_level;
	int levelReward = 0;
	for (level=0; level<rewardsFromBidLevel; level++) {
		rewardOnLevel->setValue(0, level, 0);
		//std::cout << "reward on level " << level << " : 0" << std::endl;
	}
	for (level=rewardsFromBidLevel; level<NUM_OF_BID_LEVELS; level++) {
		estNumOfEffOK_level = getEstimatedNumOfOkRespEff(loadedCategory, level);
		levelReward = totalRewardForEachLevel / estNumOfEffOK_level; 
		if (levelReward > MAX_REWARD)
			levelReward = MAX_REWARD; 
		if (levelReward < MIN_REWARD)
			levelReward = MIN_REWARD;
		//std::cout << "reward on level " << level << " : " << levelReward << std::endl;
		rewardOnLevel->setValue(levelReward, level, 0);
	}
	statHandler->setThisSesRewardOnLevels(rewardOnLevel->clone());
	setLateResponseRewardPRC(); //effectors that responds in the first legal collectionIndex are paid less, later responses gets a bonus (later -> more processing -> better answer)
	
	/* Recent code (same reward on all levels)
	bool loadedCategory = env->getLoadedCategory();
	int estimateOkResp = statHandler->getLastCorrectRespTotal(loadedCategory); //same reward for all levels
	if (loadedCategory == 0) {
		if (fitnPaidMax_reached_cat0) //potential for more responses
			estimateOkResp += (estimateOkResp*DC_ADDITIONAL_EST_PRC)/100;
	} else {
		if (fitnPaidMax_reached_cat1) //potential for more responses
			estimateOkResp += (estimateOkResp*DC_ADDITIONAL_EST_PRC)/100;
	}
	if (estimateOkResp < DC_MIN_ACC_OK_TOTAL)
		estimateOkResp = DC_MIN_ACC_OK_TOTAL;
	int reward = getFitnessBuffer_env(loadedCategory)/estimateOkResp;
	if (reward > MAX_REWARD)
		reward = MAX_REWARD; 
	if (reward < MIN_REWARD)
		reward = MIN_REWARD;

	for (int level=0; level < pow(2,BIDLEVELS_LENGTH); level++) {
		rewardOnLevel->setValue(reward, level, 0);
	}
	statHandler->setThisSesRewardOnLevels(rewardOnLevel->clone());
	setLateResponseRewardPRC();*/
}

//A good estimate is important since if the estimate is too low, the fitness rewards paid will reach the maximum allowed, 
//this means that some effectors will not be paid.
//If the estimate os too high the colline agents don't get enough fitness
int DecisionCategory::getEstimatedNumOfOkRespEff(bool loadedCat, int level) {
	int estimate = statHandler->getLastCorrectRespOnLevel(loadedCat, level);
	if (loadedCat == 0) {
		if (fitnPaidMax_reached_cat0) //potential for more responses
			estimate += (estimate*DC_ADDITIONAL_EST_PRC)/100;
	} else {
		if (fitnPaidMax_reached_cat1) //potential for more responses
			estimate += (estimate*DC_ADDITIONAL_EST_PRC)/100;
	}

	if (estimate < DC_MIN_ACC_OK_LEVEL)
		estimate = DC_MIN_ACC_OK_LEVEL;
	statHandler->setThisSesEstimatedOkResp(estimate, level);
	return estimate;
}

void DecisionCategory::setLateResponseRewardPRC() {
	//std::cout << "DEBUG: about to set late resp..." << std::endl;
	bool loadedCat = env->getLoadedCategory();
	int estNumOfRespTotal = statHandler->getLastCorrectRespTotal( loadedCat );
	if (estNumOfRespTotal < DC_MIN_ACC_OK_LEVEL*NUM_OF_BID_LEVELS)
		estNumOfRespTotal = DC_MIN_ACC_OK_LEVEL*NUM_OF_BID_LEVELS;
	int estNumOfRespIndex0 = statHandler->getLastCorrectRespAtCollIndex(loadedCat,0); //zero is ok
	int estNumOfRespIndex1 = statHandler->getLastCorrectRespAtCollIndex(loadedCat,1);
	int estNumOfRespIndex2 = statHandler->getLastCorrectRespAtCollIndex(loadedCat,2);
	int estNumOfRespIndex3 = statHandler->getLastCorrectRespAtCollIndex(loadedCat,3);
	//int estNumOfRespIndex4 = statHandler->getLastCorrectRespAtCollIndex(loadedCat,4);
	//int estNumOfRespIndex5 = statHandler->getLastCorrectRespAtCollIndex(loadedCat,5);
	int meanReward = getFitnessBuffer_env(loadedCat)/ estNumOfRespTotal;
	int estExtraReward = (100 - statHandler->DC_EARLY_RESP_REW_I0_PRC)*(meanReward*estNumOfRespIndex0)/100 + (100 - statHandler->DC_EARLY_RESP_REW_I1_PRC)*(meanReward*estNumOfRespIndex1)/100 + (100 - statHandler->DC_EARLY_RESP_REW_I2_PRC)*(meanReward*estNumOfRespIndex2)/100;
	int estNumOfRespLate = estNumOfRespTotal - (estNumOfRespIndex0 + estNumOfRespIndex1 + estNumOfRespIndex2 + estNumOfRespIndex3);// index 0,1,2,3,4 gets no bonus
	if (estNumOfRespLate < (DC_MIN_ACC_OK_LEVEL*NUM_OF_BID_LEVELS))
		estNumOfRespLate = DC_MIN_ACC_OK_LEVEL*NUM_OF_BID_LEVELS; //just to set to something
	lateResponseRewardPRC = (((estNumOfRespLate*meanReward) + estExtraReward)*100)/(estNumOfRespLate*meanReward);
	std::cout << "\n lateResponseReward Calc to : " << lateResponseRewardPRC << std::endl;
	//pressSpaceOrQuit();
	if (lateResponseRewardPRC> DC_MAX_LATE_RESP_PRC) {
		lateResponseRewardPRC = DC_MAX_LATE_RESP_PRC;
		std::cout << "\n WARNING: lateResponseRewPRC is " << lateResponseRewardPRC << " ...ok?\n" << std::endl;
		//pressSpaceOrQuit();
	}
	if (lateResponseRewardPRC < 99) {
		std::cout << "ERROR: lateResponseRewardPRC" << std::endl;
		Utilities::pressSpaceToQuit();
	}
}

int DecisionCategory::getFitnessBuffer_env(bool bufferType) {
	if (bufferType==0)
		return fitnessBuffer0_env;
	else
		return fitnessBuffer1_env;
	return -1;
}

void DecisionCategory::observedAgentIsDrifting(Observable* leavingAgent) {
	removeFromAgentsObserved(leavingAgent);
}


void DecisionCategory::turnFocusTo(Observable* seller) {
	if (syncMan->getCollectionIndex() == -1)
		return; //the effectors responding now are resp. too early or too late in training session: Ignore them;
	//std::cout << "INFO: Decision category turns focus to effector " << seller->getId() << std::endl;
	Effector* eff = (Effector*)seller;
	//int effLevel =  eff->getCatLevel();
	Message* effAppear = eff->getAppearBusFull();
	//int effHorisontalPos = eff->getPosition()->getPosX();
	//int effVerticalPos = eff->getPosition()->getPosY();
	//bool effectorIsInRewardArea = true; //!(eff->isLocatedInNoRewardArea());
	//std::cout << "DEBUG: decCat turnFocus: effLevel=" << effLevel << " cycleColl=" << syncMan->getCycleCollection() << std::endl;
	bool okMakeBid = false;
	//do not divide grid
	///*
	if (effAppear->isEqualTo(effAppCrit_ideal_wholeGrid, effAppCrit_picki_wholeGrid)) {
		//std::cout << "DEBUG: effector app accepted...";
		//if (effectorIsInRewardArea) {
		if (eff->bidAgressionLevel >= statHandler->DC_REW_FROM_BID_AGR_LEVEL)
			okMakeBid = true;
			//std::cout << "and bid is made" << std::endl;
		//} else {
			//std::cout << "but not placed in rew area, pos: " << (LPCTSTR) eff->getPosition()->toString() << std::endl;
		//}
	} else {
		//std::cout << "\nDEBUG: effector app not accepted: " << (LPCTSTR)effAppear->toStringBits() << std::endl;
		//std::cout << "must match: " << (LPCTSTR)effAppCrit_ideal_wholeGrid->toStringBits(effAppCrit_picki_wholeGrid) << std::endl;
	}
	//*/
	//end: do not divide

	if (okMakeBid) { //(effLevel >= chosenPickiLevel) {
		bid = rewardOnLevel->readValue( eff->bidAgressionLevel,0);
		int bid0 = bid*statHandler->DC_EARLY_RESP_REW_I0_PRC/100;
		int bid1 = bid*statHandler->DC_EARLY_RESP_REW_I1_PRC/100;
		int bid2 = bid*statHandler->DC_EARLY_RESP_REW_I2_PRC/100;
		//int bid3 = bid*DC_EARLY_RESP_REW_I3_PRC/100;
		int bidWBonus = bid*lateResponseRewardPRC/100;
		if (bidWBonus > MAX_REWARD)
			bidWBonus = MAX_REWARD;
		int collectionIndex = syncMan->getCollectionIndex();
		if (collectionIndex == 0)
			bid = bid0; 
		else if (collectionIndex == 1)
			bid = bid1;
		else if (collectionIndex == 2)
			bid = bid2;
		else if (collectionIndex > 3)
			bid = bidWBonus; //only index 3 is equal exactly to 'bid' (step effect towards late responses)
		bufferZero_reached = (statHandler->getThisFitnessPaidTotal() > bufferAtStartOfSession);
		if (bufferZero_reached)
			bid = (bid/2); //use half bid when buffer goes negative
		fitnessPaidMax_reached = (statHandler->getThisFitnessPaidTotal() > maxRewardThisSession);
		numOfOkRespMax_reached = (statHandler->getThisCorrectRespTotal() > DC_MAX_REWARDED);
		if ((bid>0) && !fitnessPaidMax_reached && !numOfOkRespMax_reached ) {
			//std::cout << "DecCat makes bid...(bid =" << bid << ")" << std::endl;
			eff->addToBidders(this); //make bid
		} //else don't bid
	}
	//statHandler->addToNumOfProcEffectors( eff->bidAgressionLevel, syncMan->getCollectionIndex() ); //MOVE to effector process method!
	if (env->getLoadedCategory() == 0)
		fitnPaidMax_reached_cat0 = fitnessPaidMax_reached;
	else
		fitnPaidMax_reached_cat1 = fitnessPaidMax_reached;

	//std::cout << "debug: finisedhed turn focus to..." << std::endl;
}

int DecisionCategory::auctionWon(Observable* seller, Message* bMess) {
	Effector* eff = (Effector*)seller;
	//std::cout << "Decision category buys [" << bMess->toStringBits() << "] from effector " << seller->getId();
	int reward = -1;
	int collectionIndex = syncMan->getCollectionIndex(); //between 0 and maxCollSpan-1
	bool effAnswer = bMess->getBoolValue();
	delete bMess;
	statHandler->addToNumOfProcessedBits(1);
	if (effAnswer == statHandler->getSessionCategory()) {
		reward = bid;
		statHandler->addToCorrectResp( eff->bidAgressionLevel, collectionIndex);
	} else {
		reward = 0;
		statHandler->addToWrongResp(eff->bidAgressionLevel, collectionIndex);
	}
	//std::cout << " (reward = " << reward << ")" << std::endl;
	//pressSpaceOrQuit();
	statHandler->addToFitnessPaid( reward, eff->bidAgressionLevel , collectionIndex );
	bool bufferType = env->getLoadedCategory();
	setFitnessBuffer_env( bufferType, getFitnessBuffer_env(bufferType) - reward);
	return reward;
}


int DecisionCategory::getBid() {
	return bid;
}

int DecisionCategory::getChosenPickiLevel() {
	return chosenPickiLevel;
}



