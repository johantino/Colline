
#include "StdAfx.h"
#include "Utilities.h"
#include <conio.h>
#include <iostream>


void Utilities::pressSpaceToQuit() {
	int ch = 0;
	std::cout << "press space..." << std::endl;
	while (ch != ' ') {
		ch = _getch();
	}
	exit(0);
}

void Utilities::pressSpaceOrQuit() {
	int ch = 0;
	std::cout << "press space or 'q' to quit..." << std::endl;
	while ((ch != ' ') && (ch != 'q')) {
		ch = _getch();
	}
	if (ch == 'q') {
		exit(0);
	}
}

void Utilities::pressSpaceWait() {
	int ch = 0;
	std::cout << "press space..." << std::endl;
	while (ch != ' ') {
		ch = _getch();
	}
}