// Message.h: interface for the Message class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGE_H__87D7175B_1130_4E5A_AE93_9C65ECC8574A__INCLUDED_)
#define AFX_MESSAGE_H__87D7175B_1130_4E5A_AE93_9C65ECC8574A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "Resource1.h"
//#include <vector>
//using std::vector;


//Message format:
//bit num: 0, 1, 2....(totalSize-1)
//        MSB  ....      LSB
// messages in MSB end is filled first, the not-filled is in the LSB-end: xxxx xxxx xx (MAX_MSIZE = 4)
//Messages can max be instantiated with MAX_MSIZE bits, for larger the concat method must be used
//when concatenating or adding bits, it is done in LSB end



class Message  
{
public:
	Message();
	//Message(unsigned int val, unsigned int sz); //needed?
	Message(int val, int sz); //when this constructor is used, value must fit bit length
	
	virtual ~Message();
	int getValue();
	int getSize();
	Message* clone(); //returns an exact copy of this 
	bool getBoolValue(); //when size =1 this message returns value in bool form
	CString toString(); //full info
	CString toStringBits(); //returns the total bitstring only
	CString toStringBits(unsigned int val, unsigned int size); //overloaded , free value. dont really belong to this class
	CString toStringBits(Message* filter); //overloaded...returns '#' when filter value is 0
	bool clear(); //Bad method, pointers should instead point to NULL value ....DELETE method?
	//bool bitNumOld(unsigned int bNum); //return bit number bNum: 0,1,2...length-1
	bool bitNum(unsigned int bNum); //return bit number bNum: 0,1,2...length-1
	Message* readRange(unsigned int startPos, int numOfBits); //returns a new message
	int readValue(unsigned int startPos, int numOfBits); //returns an unsigned int
	Message* readLeftMost(int numOfBits);  //returns numOfBits counted from left  (beginning of message)
	//Message* readRightMostOld(int numOfBits); //returns numOfBits counted from right (end of message)
	Message* readRightMost(int numOfBits); //returns numOfBits counted from right (end of message)
	Message* takeLeftMost(unsigned int numOfBits);  //same as returnLeft... but also removes the bits from _this_
	Message* takeRightMost(unsigned int numOfBits); //same as returnRight...but also removes the bits from _this_
	Message* replacePart(unsigned int startPos, unsigned int endPos, Message* replaceMess); //replaces part if _this_ with replaceMess
	int messageContained(Message* subMess); //Returns startPos if subMess is contained in _this_ otherwise returns -1
	int getNumOfZeros();
	//NOTE char
	bool isEqualTo(Message* compareMess, Message* pickiness); //returns true if this and compareMess are alike
	void addToEnd(Message* tail); //adds tail to this
	bool AND(Message* inp2); //this AND 1bm (only 1 bit length supported)
	bool OR(Message* inp2);  //this OR  1bm (only 1 bit length supported)
	bool XOR(Message* inp2); //this XOR 1bm (only 1 bit length supported)
	int flipBits(int promilleChance); //flip each bit in message with 'promilleChance' probability, returns num of flipped
	void flipBit(int bNum); //move to private

private:
	void pressSpaceToQuit();
	void pressSpaceOrQuit();
	void changeValuesTo(Message* m); //change _this_ to equal 'm'
	int makeValueFitSize(int val, int sz); //this function dumps leftmost bits
	//The message bitstring is specified by:
	int value;  //the value  of _this_ chunk of the bitstring
	int size; //the size (length) of the bitstring = size of message	
};

#endif // !defined(AFX_MESSAGE_H__87D7175B_1130_4E5A_AE93_9C65ECC8574A__INCLUDED_)
