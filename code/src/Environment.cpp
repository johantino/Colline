// Environment.cpp: implementation of the Environment class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Colline.h"
#include "Environment.h"
#include "Konst.h"
#include "math.h"
#include <iostream>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Environment::Environment(int initialFitness, int initialMatBonusBuff)
{
	//statHandler = sh;
	fitness = 0;
	setFitness( initialFitness );
	matingBonusBuffer = initialMatBonusBuff;
	
	//numOfPipes = 6; //numOfP;
	loadedCategory = false;
	//lastMessage = new Message(0,0);
	nextMessInPipes = new CList<Message*, Message*>;
	EMPTYMESS = new Message(0,0);
	for (int i=0; i<NUM_OF_PIPES; i++) {
		nextMessInPipes->AddTail(EMPTYMESS);
	}
	lastSessCat0 = 0;
	lastSessCat1 = 0;
}

Environment::~Environment()
{

}

void Environment::setFitness(int value) {
	//int change = value - fitness;
	//statHandler->adjustFitnessEnvironment(change); //can't use StatististicsManager here because 'Observable.h' is incl in StatMan. (loop-conflict)
	fitness = value;
}
	
void Environment::receiveFitness(int amount) {
	setFitness(fitness + amount);
}

void Environment::receiveMatingBonus(int bonus) {
	matingBonusBuffer += bonus;
}

int Environment::giveMatingBonus(int agentInitFitn, int agentCurrFitn) {
	int bonusToAgent = 0;
	/*if (matingBonusBuffer>0) {
		bonusToAgent = (agentInitFitn*C_MATING_BONUS_PRC)/100;
		if (bonusToAgent > matingBonusBuffer)
			bonusToAgent = matingBonusBuffer;
		if ((agentCurrFitn + bonusToAgent) > (C_MAX_BONUS_UP_TO_FITN_PRC*agentInitFitn)/100) { //bonus is only given upto a certain limit
			bonusToAgent = (C_MAX_BONUS_UP_TO_FITN_PRC*agentInitFitn)/100 - agentCurrFitn;
			if (bonusToAgent < 0)
				bonusToAgent = 0;
		}
		matingBonusBuffer -= bonusToAgent;
	}*/
	return bonusToAgent;
}

int Environment::getMatingBonusBuffer() {
	return matingBonusBuffer;
}

int Environment::payFitness(int amount) {
	setFitness(fitness - amount);
	return amount;
}

int Environment::readFitness() {
	return fitness;
}

void Environment::fillPipesCrossOrRandom() {
	Message* tempM;
	Message* nextMessage;
	int i, randVal, fNum;
	int MESS_SIZE_COR = 5;
	//Test that pipes are empty:
	for (i=0; i<NUM_OF_PIPES; i++) {
		tempM = nextMessInPipes->GetAt( nextMessInPipes->FindIndex(i) );
		if (tempM->getSize() != 0 ) {
			std::cout << "ERROR: environment all pipes should be empty" << std::endl;
			while (true) {}
		}
	}
	nextMessInPipes->RemoveAll();
	int numOfFountainGroups = NUM_OF_PIPES/5;//note: make sure that 5 subsequent pipes are 'clustered' in grid
	int dice = getRandNumBetwZeroAnd( 99);  //0-5
	if (dice < 33) {//fill with A's
		for (i=1; i<=numOfFountainGroups; i++) {
			for (fNum = 0; fNum<5; fNum++) {
				if (fNum==2)
					nextMessInPipes->AddTail( new Message(31,5) );
				else
					nextMessInPipes->AddTail( new Message(4,5) );
			}
		}
		loadedCategory = true; //loaded category is 'A'
	} else { //fill with random values
		for (i=0; i<NUM_OF_PIPES; i++) {
			randVal = getRandNumBetwZeroAnd( pow(2,MESS_SIZE_COR)-1); 
			nextMessage = new Message(randVal, MESS_SIZE_COR);
			nextMessInPipes->AddTail( nextMessage ) ;
		}
		loadedCategory = false; //category is not-A (risk that random values happen to match 'A' are ignored)
	}
	toStringLastMessage(); //make string to be shown later
}

void Environment::fillPipesCircleOrNotCircle(bool fillWithCategory) {
	Message* tempM;
	Message* nextMessage;
	//bool chooseCatCircle = false;
	int i, circleVariant, randVal, fNum;
	int MESS_SIZE_CRC = 5;
	//int MAX_REPEAT_CAT = 4; //max repetitions of the same category
	//Test that pipes are empty:
	for (i=0; i<NUM_OF_PIPES; i++) {
		tempM = nextMessInPipes->GetAt( nextMessInPipes->FindIndex(i) );
		if (tempM->getSize() != 0 ) {
			std::cout << "ERROR: environment all pipes should be empty" << std::endl;
			while (true) {}
		}
	}
	nextMessInPipes->RemoveAll();
	
	int numOfFountainGroups = NUM_OF_PIPES/5;//note: make sure that 5 subsequent pipes are 'clustered' in grid
	if (fillWithCategory == 1) {//fill with Circle
		//lastSessCat1 = sessNum;
		for (i=1; i<=numOfFountainGroups; i++) {
			circleVariant = getRandNumBetwZeroAnd( 3 );  //0-3 = 4 different positions for the circle
			circleVariant = 0; //NB! override random selection NB! ...always pos 0
			for (fNum = 0; fNum<5; fNum++) {
				nextMessInPipes->AddTail( getCircleMessage(fNum, circleVariant) );
			}
		}
		loadedCategory = true; //loaded category is circle
	} else { //fill with random values
		//lastSessCat0 = sessNum;
		for (i=0; i<NUM_OF_PIPES; i++) {
			randVal = getRandNumBetwZeroAnd( pow(2,MESS_SIZE_CRC)-1 ); 
			nextMessage = new Message(randVal, MESS_SIZE_CRC);
			nextMessInPipes->AddTail( nextMessage ) ;
		}
		loadedCategory = false; //category is not-A (risk that random values happen to match 'A' are ignored)
	}
	//std::cout << "DEBUG; loadedcat: " << loadedCategory << std::endl;
	toStringLastMessage(); //make string to be shown later
}

bool Environment::fillPipesCircleOrNotCircle(int sessNum, int perf_cat0, int perf_cat1) {
	//Message* tempM;
	//Message* nextMessage;
	bool chooseCatCircle = false;
	//int i, circleVariant, randVal, fNum;
	//int MESS_SIZE_CRC = 5;
	int MAX_REPEAT_CAT = 4; //max repetitions of the same category
	int chanceForCat0 = 500; //default
	int disharmony = perf_cat0 - perf_cat1; //how big is the difference in how cat0 and cat1 are handled?
	if (abs(disharmony)>6)
		chanceForCat0 = 800; 
	else if (abs(disharmony)>4)
		chanceForCat0 = 700;
	else if(abs(disharmony)>2)
		chanceForCat0 = 600;
	else 
		chanceForCat0 = 500; //used when almost same performance
	if (disharmony>0) //handles cat0 better than cat1, so switch chance
		chanceForCat0 = 1000 - chanceForCat0;
	std::cout << "disharmony: " << disharmony << " , chanceCat0: " << chanceForCat0 << std::endl;
	/*if (perf_cat0 >= (MAX_PERF_SCORE*.75) && perf_cat1 < (MAX_PERF_SCORE/2) )
		chanceForCat0 = 200; //handles cat0 pretty well but not cat1 so increase cat1 training
	else if (perf_cat1 >= (MAX_PERF_SCORE*.75) && perf_cat0 < (MAX_PERF_SCORE/2) )
		chanceForCat0 = 800; //vice versa
		*/
	int dice = getRandNumBetwZeroAnd(999); 
	if (dice < chanceForCat0) { //choose cat 0 unless cat 1 has not been loaded a long time
		if ((sessNum-lastSessCat1) <= MAX_REPEAT_CAT) //has cat A been present recently?
			chooseCatCircle = false; //yes, so its ok to choose not-A
		else
			chooseCatCircle = true; //no it is a long time since cat A was loaded: load cat A
	} else { //choose cat1 unless cat0 has not been loaded in a while
		if ((sessNum-lastSessCat0) <= MAX_REPEAT_CAT) //has cat not-A been present recently?
			chooseCatCircle = true; //yes, so its ok to choose A
		else
			chooseCatCircle = false; //no, load cat not-A
	}
	fillPipesCircleOrNotCircle(chooseCatCircle);
	if (chooseCatCircle)
		lastSessCat1 = sessNum;
	else
		lastSessCat0 = sessNum;
	return chooseCatCircle;
}


void Environment::fillPipesAOrNotA(int sessNum) {
	Message* tempM;
	Message* nextMessage;
	bool chooseCatA = false;
	int i, aVariant, randVal, fNum;
	int MESS_SIZE_ANA = 5;
	int MAX_REPEAT_CAT = 4; //max repetitions of the same category
	//Test that pipes are empty:
	for (i=0; i<NUM_OF_PIPES; i++) {
		tempM = nextMessInPipes->GetAt( nextMessInPipes->FindIndex(i) );
		if (tempM->getSize() != 0 ) {
			std::cout << "ERROR: environment all pipes should be empty" << std::endl;
			while (true) {}
		}
	}
	nextMessInPipes->RemoveAll();
	int numOfFountainGroups = NUM_OF_PIPES/5;//note: make sure that 5 subsequent pipes are 'clustered' in grid
	int dice = getRandNumBetwZeroAnd( 99 ); 
	if (dice < 50) { //66
		if ((sessNum-lastSessCat1) <= MAX_REPEAT_CAT) //has cat A been present recently?
			chooseCatA = false; //yes, so its ok to choose not-A
		else
			chooseCatA = true; //no it is a long time since cat A was loaded: load cat A
	} else {
		if ((sessNum-lastSessCat0) <= MAX_REPEAT_CAT) //has cat not-A been present recently?
			chooseCatA = true; //yes, so its ok to choose A
		else
			chooseCatA = false; //no, load cat not-A
	}

	if (chooseCatA) {//fill with A's
		lastSessCat1 = sessNum;
		for (i=1; i<=numOfFountainGroups; i++) {
			aVariant = getRandNumBetwZeroAnd( 5 );  //0-5 = 6 different positions for the 'A'
			for (fNum = 0; fNum<5; fNum++) {
				nextMessInPipes->AddTail( getAMessage(fNum, aVariant) );
			}
		}
		loadedCategory = true; //loaded category is 'A'
	} else { //fill with random values
		lastSessCat0 = sessNum;
		for (i=0; i<NUM_OF_PIPES; i++) {
			randVal = getRandNumBetwZeroAnd( pow(2,MESS_SIZE_ANA)-1 ); 
			nextMessage = new Message(randVal, MESS_SIZE_ANA);
			nextMessInPipes->AddTail( nextMessage ) ;
		}
		loadedCategory = false; //category is not-A (risk that random values happen to match 'A' are ignored)
	}
	toStringLastMessage(); //make string to be shown later
}

void Environment::fillPipesZerosOrOnes(bool fillWithCat) {
	//std::cout << "DEBUG: filling pipes..."<< std::endl;
	int MESS_SIZE = 6;
	int MIN_MAJORITY = 5; //the least amount of 0's (1's) for a zero-message (one-message)
	Message* tempM;
	//Test that pipes are empty:
	for (int i=0; i<NUM_OF_PIPES; i++) {
		tempM = nextMessInPipes->GetAt( nextMessInPipes->FindIndex(i) );
		if (tempM->getSize() != 0 ) {
			std::cout << "ERROR: environment all pipes should be empty" << std::endl;
			while (true) {}
		}
	}
	nextMessInPipes->RemoveAll();
	Message* nextMessage;
	nextMessage = new Message(0,0);
	int randVal;
	int numOfZeros;
	int messNum;
	if (!fillWithCat) { //cat0
		for (messNum=0; messNum<NUM_OF_PIPES; messNum++) {
			numOfZeros = 1;
			while (numOfZeros < MIN_MAJORITY) {
				delete nextMessage;
				randVal = getRandNumBetwZeroAnd( pow(2,MESS_SIZE)-1 ); 
				nextMessage = new Message(randVal, MESS_SIZE);
				numOfZeros = nextMessage->getNumOfZeros();
			}
			nextMessInPipes->AddTail( nextMessage );
			nextMessage = new Message(0,0);
		}
		loadedCategory = false; //mainly zeros
	} else { //cat1
		for (messNum=0; messNum<NUM_OF_PIPES; messNum++) {
			numOfZeros = MESS_SIZE;
			while (numOfZeros > (MESS_SIZE - MIN_MAJORITY)) {
				delete nextMessage;
				randVal = getRandNumBetwZeroAnd( pow(2,MESS_SIZE)-1 ); 
				nextMessage = new Message(randVal, MESS_SIZE);
				numOfZeros = nextMessage->getNumOfZeros();
			}
			nextMessInPipes->AddTail( nextMessage );
			nextMessage = new Message(0,0);
		}
		loadedCategory = true; //mainly ones
	}
	delete nextMessage;
	toStringLastMessage(); //make string to be shown later
}

bool Environment::fillPipesZerosOrOnes(int sessNum, int perf_cat0, int perf_cat1) {
	//Message* tempM;
	//Message* nextMessage;
	bool chooseCat1 = false;
	//int i, circleVariant, randVal, fNum;
	//int MESS_SIZE_CRC = 5;
	int MAX_REPEAT_CAT = 4; //max repetitions of the same category
	int chanceForCat0 = 500; //default
	int disharmony = perf_cat0 - perf_cat1; //how big is the difference in how cat0 and cat1 are handled?
	if (abs(disharmony)>6)
		chanceForCat0 = 800; 
	else if (abs(disharmony)>4)
		chanceForCat0 = 700;
	else if(abs(disharmony)>2)
		chanceForCat0 = 600;
	else 
		chanceForCat0 = 500; //used when almost same performance
	if (disharmony>0) //handles cat0 better than cat1, so switch chance
		chanceForCat0 = 1000 - chanceForCat0;
	std::cout << "disharmony: " << disharmony << " , chanceCat0: " << chanceForCat0 << std::endl;
	while (true) {}
	/*if (perf_cat0 >= (MAX_PERF_SCORE*.75) && perf_cat1 < (MAX_PERF_SCORE/2) )
		chanceForCat0 = 200; //handles cat0 pretty well but not cat1 so increase cat1 training
	else if (perf_cat1 >= (MAX_PERF_SCORE*.75) && perf_cat0 < (MAX_PERF_SCORE/2) )
		chanceForCat0 = 800; //vice versa
		*/
	int dice = getRandNumBetwZeroAnd(999); 
	if (dice < chanceForCat0) { //choose cat 0 unless cat 1 has not been loaded a long time
		if ((sessNum-lastSessCat1) <= MAX_REPEAT_CAT) //has cat1 been present recently?
			chooseCat1 = false; //yes, so its ok to choose cat0
		else
			chooseCat1 = true; //no it is a long time since cat1 was loaded: load cat1
	} else { //choose cat1 unless cat0 has not been loaded in a while
		if ((sessNum-lastSessCat0) <= MAX_REPEAT_CAT) //has cat0 been present recently?
			chooseCat1 = true; //yes, so its ok to choose cat1
		else
			chooseCat1 = false; //no, load cat0
	}
	fillPipesZerosOrOnes(chooseCat1);
	if (chooseCat1)
		lastSessCat1 = sessNum;
	else
		lastSessCat0 = sessNum;
	return chooseCat1;
}


//the number of pipes must equal the number of fountains.
//remember cycle info to prevent too rapid inflow
Message* Environment::getNextMessageFromPipe(int pipeNum) {
	POSITION pos;
	Message* tempMess;
	//First test if prev. pipes are empty:
	/*for (int i=0; i<pipeNum; i++) {
		pos = nextMessInPipes->FindIndex( i );
		tempMess = nextMessInPipes->GetAt( pos );
		if (tempMess->getSize() != 0) {
			std::cout << "ERROR: pipes must be accessed in order" << std::endl;
			while (true) {}
		}
	}*/
	//end test
	pos = nextMessInPipes->FindIndex( pipeNum );
	tempMess = nextMessInPipes->GetAt( pos );
	if (tempMess->getSize() == 0) {
		std::cout << "ERROR: environment: pipe empty" << std::endl;
		while (true) {}
		//fillPipes();
	}
	pos = nextMessInPipes->FindIndex( pipeNum ); //needed?
	tempMess = nextMessInPipes->GetAt(pos);
	nextMessInPipes->SetAt(pos, EMPTYMESS); //pipe is now empty
	return tempMess;
}

bool Environment::getLoadedCategory() {
	return loadedCategory;
}

std::string Environment::getLastMessageString() {
	return lastMessageString;
}

void Environment::toStringLastMessage() {
	std::string info;
	Message* mess;
	info = "\n Messages from pipes (ordered): ";
	for (int pNum=0; pNum<NUM_OF_PIPES; pNum++) {
		mess = nextMessInPipes->GetAt( nextMessInPipes->FindIndex( pNum) );
		info += "\n";
		info += mess->toStringBits();
	}
	lastMessageString = info;
}

Message* Environment::getAMessage(int rowNum, int aPosition) {
	if ((aPosition == 0) || (aPosition == 3)) {
		if ((aPosition == 0) && (rowNum == 4))
			return (new Message(0,5));
		if ((aPosition == 3) && (rowNum == 0))
			return (new Message(0,5));
		if (aPosition==0)
			rowNum += 1;
		if (rowNum % 2 == 0)
			return (new Message(20,5));
		else
			return (new Message(28,5));
	}
	if ((aPosition == 1) || (aPosition == 4)) {
		if ((aPosition == 1) && (rowNum == 4))
			return (new Message(0,5));
		if ((aPosition == 4) && (rowNum == 0))
			return (new Message(0,5));
		if (aPosition==1)
			rowNum += 1;
		if (rowNum % 2 == 0)
			return (new Message(10,5));
		else
			return (new Message(14,5));
	}
	if ((aPosition == 2) || (aPosition == 5)) {
		if ((aPosition == 2) && (rowNum == 4))
			return (new Message(0,5));
		if ((aPosition == 5) && (rowNum == 0))
			return (new Message(0,5));
		if (aPosition==2)
			rowNum += 1;
		if (rowNum % 2 == 0)
			return (new Message(5,5));
		else
			return (new Message(7,5));
	}
	std::cout << "ERROR: env..getAMessage" << std::endl;
	while (true) {}
	return (new Message(0,0));
}

Message* Environment::getCircleMessage(int rowNum, int circlePosition) {
	if ((circlePosition == 0) || (circlePosition == 2)) {
		if ((circlePosition == 0) && (rowNum == 4))
			return (new Message(0,5));
		if ((circlePosition == 2) && (rowNum == 0))
			return (new Message(0,5));
		if (circlePosition==0)
			rowNum++;
		if (rowNum==2 || rowNum==3)
			return (new Message(18,5));
		else
			return (new Message(12,5));
	}
	if ((circlePosition == 1) || (circlePosition == 3)) {
		if ((circlePosition == 1) && (rowNum == 4))
			return (new Message(0,5));
		if ((circlePosition == 3) && (rowNum == 0))
			return (new Message(0,5));
		if (circlePosition==1)
			rowNum++;
		if (rowNum==2 || rowNum==3)
			return (new Message(9,5));
		else
			return (new Message(6,5));
	}
	std::cout << "ERROR: env..getCircleMessage" << std::endl;
	while (true) {}
	return (new Message(0,0));
}

int Environment::getRandNumBetwZeroAnd(int maximum) {
	int randomNum;
	if (maximum<65536)  //no overflow risk
		randomNum = (rand()*(maximum+1)) / RAND_MAX;
	else //large number: use other method
		randomNum = ( ((double)rand()) / ((double)RAND_MAX)  ) *maximum;
	if (randomNum<0) {
		std::cout << "ERROR: getRandomNum" << std::endl;
		while (true) {}
	}
	if (randomNum>maximum)
		randomNum = maximum; //happens when rand()=RAND_MAX (rarely)
	return randomNum;
}
		

