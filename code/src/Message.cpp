// Message.cpp: implementation of the Message class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Colline.h"
#include "Message.h"
#include "math.h"
#include "Konst.h"
#include <conio.h> //for press key
//#include "resource.h" //for std::cout
#include <iostream>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Message::Message()
{
	value = 0;
	size = 0;
}

//Is this cons needed?
/*Message::Message(unsigned int val, unsigned int sz) //value can exceed bit length, when using this constructor
{
	if (sz>MAX_MSIZEPART || val>=pow(2,sz) ) {
		std::cout << "ERROR: message pSize error"; 
		while (true) {} //exit(0);
	}
	pSize = sz;
	pValue = makeValueFitSize(val,sz);
	hasConcat = false;
	//concatM = NULL;
}
*/

Message::Message(int val, int sz) //value must always match length in this constructor
{
	if ((val<0) || (sz < 0) ) {
		std::cout << "ERROR: message construct negative!" << std::endl;
		std::cout << "value: " << val << std::endl;
		std::cout << "size : " << sz << std::endl;
		pressSpaceToQuit();
	}
	if (val >= pow(2,sz) ) {
		std::cout << "ERROR: message size error (value too big for length)" << std::endl; 
		std::cout << "value: " << val << std::endl;
		std::cout << "size: " << sz << std::endl;
		pressSpaceToQuit(); //exit(0);
	}
	if (sz > MAX_MSIZE) {
		std::cout << "ERROR: message size too large!" << std::endl;
		pressSpaceToQuit();
	}

	size = sz;
	value = val;
}

Message::~Message()
{

}

//test: ok
int Message::flipBits(int promilleChance) {
	if (promilleChance<0 || promilleChance>1000) {
		std::cout << "ERROR: message flipbits, must be promille" << std::endl;
		pressSpaceToQuit();
	}
	int numFlipped = 0;
	int randVal;
	for (int i=0; i<size; i++) {
		randVal = (rand()*999) / RAND_MAX; //0...999
		if (randVal < promilleChance) {
			flipBit( i ); //flip bit
			numFlipped++;
		}
	}
	return numFlipped;
}

void Message::flipBit(int bNum) {
	if (bNum>= size) {
		std::cout << "ERROR: flipbit" << std::endl;
		pressSpaceToQuit();
	}
	bool isOne = bitNum(bNum);
	int potens = (size-1) - bNum; //bNum is counted from left
	if (isOne)
		value -= pow(2,potens);
	else 
		value += pow(2,potens);
	return;
}



void Message::changeValuesTo(Message *m) {
	value = m->getValue();
	size = m->getSize();
}

int Message::makeValueFitSize(int val, int sz) {
	while (val >= pow(2,sz) ){ //test if val too big
		val = val - pow(2,sz);
	}
	return val;
}

int Message::getValue() {
	return value;
}

int Message::getSize(){
	return size;
}


//when value of message is 1 or 0, this method can be called
bool Message::getBoolValue() {
	bool bVal;
	if (value == 0)
		bVal = false;
	else if (value==1)
		bVal = true;
	else {
		std::cout << "ERROR: wrong getBoolValue call! " << std::endl;
		pressSpaceToQuit();
	}
	return bVal;
}

/*bool Message::bitNum(unsigned int bNum) {
	Message* tempMess;
	bool bNumValue;
	if (bNum < getSize()) {
		//first leftshift then rightshift, so value is 0 or 1
		tempMess = new Message(value, size);
		tempMess = tempMess->readRightMost(size - bNum); //bit # 0 now contains desired value
		tempMess = tempMess->readLeftMost(1);
		bNumValue = tempMess->getBoolValue();
	}
	else {
		std::cout << "ERROR: bitNum exceeds size!" << std::endl;
		while (true) {}
	}
	return bNumValue;
}*/

bool Message::bitNum(unsigned int bNum) {
	if (bNum >= getSize()) {
		std::cout << "ERROR: Message, bitnum2 error" << std::endl;
		pressSpaceToQuit();
	}
	int tempVal = getValue();
	int power;
	int sz = getSize();
	for (int pos=0; pos <bNum; pos++) {
		power = sz - pos - 1; //the value of the position
		if ((tempVal - pow(2,power)) >= 0)
			tempVal -= pow(2,power);
	}
	power = sz - bNum -1;
	if ((tempVal - pow(2,power)) >= 0)
		return 1;
	//else
	return 0;
}

Message* Message::readLeftMost(int numOfBits) {
	if (numOfBits < 0)
		return new Message(0,0);
	int newVal; 
    int rightShift = getSize() - numOfBits;
	if (rightShift<0) {
		rightShift = 0;
		std::cout << "ERROR: readLeftMost, message too small!" << std::endl;
		pressSpaceToQuit();
	}
	newVal = getValue() / pow(2,rightShift); //newVal >> rightShift;	
	Message* newM = new Message(newVal, numOfBits);
	return newM;
}


Message* Message::readRightMost(int numOfBits) {
	if (numOfBits < 0)
		return new Message(0,0);
	int newVal = 0;
	int powerCnt = 0;
	int startPos = getSize() - numOfBits;
	int endPos = getSize() - 1;
	for (int i=endPos; i>=startPos; i--) {
		if (bitNum(i)) {
			newVal += pow(2,powerCnt);
		}
		powerCnt++;
	}
	Message* newM = new Message(newVal, numOfBits);
	return newM;
}

Message* Message::takeLeftMost(unsigned int numOfBits) {
	char bitsLeft = getSize() - numOfBits;
	if ( numOfBits > getSize() ) { //cant use bitsLeft because of unsigned to signed conversion
		std::cout << "ERROR: takeLeftMost numOfBits less than zero!" << std::endl;
		pressSpaceToQuit();
	}
	Message* retMess = readLeftMost(numOfBits);
	Message* whatsLeft = readRightMost( bitsLeft );
	changeValuesTo( whatsLeft); //cumbersome way to copy
	return retMess;
}

Message* Message::takeRightMost(unsigned int numOfBits) {
	char bitsLeft = getSize() - numOfBits;
	if ( numOfBits > getSize() ) { //cant use bitsLeft because of unsigned to signed conversion
		std::cout << "ERROR: takeRightMost numOfBits less than zero!" << std::endl;
		pressSpaceToQuit();
	}
	Message* retMess = readRightMost(numOfBits);
	Message* whatsLeft = readLeftMost( bitsLeft );
	changeValuesTo( whatsLeft );
	return retMess;
}


std::string Message::toString() {
	std::string text;
	
	char tszStr[2];
	char tvaStr[20];

	int tsz = getSize();
	int tva = getValue();
	_itoa(tsz,tszStr,10);
	_itoa(tva,tvaStr,10);
	text = "----Message info----";	
	text += "\n# bits: ";
	text += tszStr;
	text += "\nValue : ";
	text += tvaStr;
	text += "\nBitval: ";
	text += toStringBits();
	text += "\n--------------------";
	return text;
}

std::string Message::toStringBits() {
	if (getSize() == 0)
		return "(empty)";
	char bStr[MAX_MSIZE];
	int bitv = getValue();
	_itoa(bitv,bStr,2);
	std::string bCStr = bStr;
	int numOfMissZeros = getSize() - bCStr.size();
	std::string zeros;
	while (numOfMissZeros>0) {
		zeros += "0";
		numOfMissZeros--;
	}
	bCStr = zeros + bCStr;
	return bCStr;
}

std::string Message::toStringBits(Message* filter) {
	std::string info;
	if (getSize() != filter->getSize()) {
		std::cout << "ERROR: toStringBits size error!" << std::endl;
		pressSpaceToQuit();
	}
	if (getSize() == 0)
		return "empty";
	for (int i=0; i<getSize(); i++) {
		if (filter->bitNum(i) == 1) {
			if (bitNum(i)==1)
				info += "1";
			else
				info += "0";
		}else
			info += "#";
	}
	return info;
}



std::string Message::toStringBits(unsigned int bitv, unsigned int size) {
	char bStr[MAX_MSIZE];
	_itoa(bitv,bStr,2);
	std::string bCStr = bStr;
	int numOfMissZeros = size - bCStr.size();
	std::string zeros;
	while (numOfMissZeros>0) {
		zeros += "0";
		numOfMissZeros--;
	}
	bCStr = zeros + bCStr;
	return bCStr;
}

bool Message::clear() {
	value = 0;
	size = 0;
	return true;
}


Message* Message::readRange(unsigned int startPos, int numOfBits) {
	if (numOfBits < 1)
		return new Message(0,0);
	if ((startPos + numOfBits) > getSize()) {
		std::cout << "ERROR; readRange() position error!" << std::endl;
		pressSpaceToQuit();
	}		
	Message* newM;
	newM = readRightMost(getSize() - startPos);
	newM = newM->readLeftMost(numOfBits);
	return newM;
}

int Message::readValue(unsigned int startPos, int numOfBits) {
	Message* temp = readRange(startPos,numOfBits);
	int val = temp->getValue();
	delete temp;
	return val;
}

//this method return startPos if the pattern subMess is found in _this_ message (any number of times) or else -1
//TEST: test ok(1)
int Message::messageContained(Message* subMess) {
	int startPosPat = -1; //the return value (start pos for subMess)
	int subSize = subMess->getSize();
	if ( subSize > getSize() )
		return startPosPat;
	int maxNumOfComp = getSize() - subSize + 1;
	int cnt=-1;
	int i;
	bool patternFound = false;
	bool thisVal,subVal,match;
	while (!patternFound && (cnt+1) < maxNumOfComp){
		//test bit 0+(cnt),1+(cnt)...(fiSize-1)+(cnt) in message versus 0,1...fiSize-1 in foodIdeal
		match = true;
		cnt++;
		for (i=0; i<subSize; i++) {
			subVal = subMess->bitNum(i);
			thisVal = bitNum(i+cnt);
			if ( subVal != thisVal ) {
				match = false;
				i = subSize; //no need to test the rest
			}
		}
		if (match)
			patternFound = true;
	}
	if (patternFound)
		startPosPat = cnt;
	return startPosPat;
}

//TEST THIS:
bool Message::isEqualTo(Message* compareMess, Message* pickiness) {
	//temp test (delete later):
	if ((getValue() < 0) || (compareMess->getValue() < 0)) {
		std::cout << "ERROR: mess, isEqualTo, value negative" << std::endl;
		pressSpaceToQuit();
	}
	//end temp test
	bool doCare, accepted;
	int numOfBits = compareMess->getSize();
	if (numOfBits != getSize() ) {
		std::cout << "ERROR: isEqualTo length mismatch!" << std::endl;
		pressSpaceToQuit();
	}
	accepted = true;
	for (int i=0; i<numOfBits; i++) {
		doCare = pickiness->bitNum(i);
		if (doCare) {
			if (compareMess->bitNum(i) != bitNum(i)) {
				accepted = false;
				i = numOfBits; //no need to test the rest
			}
		} // else ignore bitnum
	}
	return accepted;
}

void Message::addToEnd(Message* tail) {
	int sizeTail = tail->getSize();
	int valueTail = tail->getValue();
	if ((size+sizeTail) > 31) {
		//std::cout << "ERROR: addToEnd do not support big mess (>31 bit)!" << std::endl;
		//pressSpaceToQuit();
		std::cout << "WARNING: addToEnd do not support big mess (>31 bit). Message truncated." << std::endl;
		pressSpaceOrQuit();
		int newTailSize = 31 - size;
		int newTailValue = valueTail / pow(2, (sizeTail-newTailSize));
		sizeTail = newTailSize;
		valueTail = newTailValue;
	}
	size += sizeTail;
	value = value * pow(2,sizeTail); //value is leftshifted
	value += valueTail; //tail->getValue();
	delete tail;
}

int Message::getNumOfZeros() {
	int numOfZeros = 0;
	for (int i=0; i<size; i++) {
		if (bitNum(i)==false)
			numOfZeros++;
	}
	return numOfZeros;
}


bool Message::AND(Message* inp2) {
	bool val1 = getBoolValue();
	bool val2 = inp2->getBoolValue();
	bool result = val1 & val2;
	return result;
}

bool Message::OR(Message* inp2) {
	bool val1 = getBoolValue();
	bool val2 = inp2->getBoolValue();
	bool result = val1 | val2;
	return result;
}

bool Message::XOR(Message* inp2) {
	bool val1 = getBoolValue();
	bool val2 = inp2->getBoolValue();
	bool result = (val1 ^ val2);
	return result;
}

//TEST: not tested
Message* Message::replacePart(unsigned int startPos, unsigned int endPos, Message* replaceMess) {
	Message* temp;
	int partSize = endPos - startPos + 1;
	if (partSize <= 0)
		return (this->clone());
	temp = readLeftMost(startPos); //temp now contains first part of _this_ to be kept
	temp->addToEnd( replaceMess->readRange(startPos, partSize) );
	if (endPos < (getSize()-1) )
		temp->addToEnd( readRightMost( getSize()-endPos-1 ));
	return temp;
}


Message* Message::clone() {
	return new Message(getValue(), getSize() );
}

void Message::pressSpaceToQuit() {
	int ch;
	std::cout << "press space..." << std::endl;
	while (ch != ' ') {
		ch = _getch();
	}
	exit(0);
}

void Message::pressSpaceOrQuit() {
	int ch;
	std::cout << "press space or 'q' to quit..." << std::endl;
	while ((ch != ' ') && (ch != 'q')) {
		ch = _getch();
	}
	if (ch == 'q')
		exit(0);
}
