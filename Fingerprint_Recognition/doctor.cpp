#include <stdlib.h>
#include <iostream>

#include "doctor.h"

using namespace std;

doctor::doctor() {
	//empty constructor 
}

doctor::doctor(string fname, string lname, int id, int x[50], int y[50]) {

	firstName = fname;
	lastName = lname;
	ID = id;
	for (int i = 0; i < 50; i++) {
		xCoords[i] = x[i];
		yCoords[i] = y[i];
	}
	minutiaeTotal = -1; //default
}

doctor::doctor(const doctor& doctor) {
	firstName = doctor.firstName;
	lastName = doctor.lastName;
	for (int i = 0; i < 50; i++) {
		xCoords[i] = doctor.xCoords[i];
		yCoords[i] = doctor.yCoords[i];
	}
	minutiaeTotal = doctor.minutiaeTotal;
}

doctor::~doctor() {
	//empty destructor
}

void doctor::setMinutiaeTotal(int total) {
	minutiaeTotal = total;
}

void doctor::setMinutiaeCoords(int x[50], int y[50]) {
	for (int i = 0; i < 50; i++) {
		xCoords[i] = x[i];
		yCoords[i] = y[i];
	}
}

int doctor::getXCoordAtPos(int pos) {
	return xCoords[pos];
}
int doctor::getYCoordAtPos(int pos) {
	return yCoords[pos];
}

int* doctor::getMinutiaeXCoords() {
	return xCoords;
}

int* doctor::getMinutiaeYCoords() {
	return yCoords;
}

int doctor::getMinutiaeTotal() {
	return minutiaeTotal;
}

ostream& operator<<(ostream& outs, doctor& doctor) {
	outs << "First name: " << doctor.firstName << "Last name: " << doctor.lastName << "ID: " << doctor.ID << endl;
	return outs;
}

bool operator==(const doctor& d1, const doctor& d2) {
	/*
		Goals: return true if there is a 75% match with the coordinates

		How: Since we know there are a decent amount of points where the x,y has some undefined number
			 we will want to throw out the data that has one extreme in either category. This will then
			 get us our base set. We will set the minutiae total of the object to make future comparisons faster.

			 Once we have the valid points, we can easily discard a scan if there aren't even 75% minutiae of
			 our comparison.

			 Once it passes this test, we will know that we can iterate through each and compare points.
			 The main problem will be how fast this comparison will go.

	*/
	//copy constructors need to be called
	doctor doctor1 = doctor(d1);
	doctor doctor2 = doctor(d2);

	int totalDoctor1 = 0;
	int xCoordDoctor1;
	int yCoordDoctor1;

	int totalDoctor2 = 0;
	int xCoordDoctor2;
	int yCoordDoctor2;

	//get total minutiae points and set the value in the object.
	for (int i = 0; i < 50; i++) {
		xCoordDoctor1 = doctor1.getXCoordAtPos(i);
		yCoordDoctor1 = doctor1.getYCoordAtPos(i);
		xCoordDoctor2 = doctor2.getXCoordAtPos(i);
		yCoordDoctor2 = doctor2.getYCoordAtPos(i);

		if ((xCoordDoctor1 < 155 && xCoordDoctor1 > 0) && (yCoordDoctor1 < 203 && yCoordDoctor1 > 0)) {
			totalDoctor1++;
		}

		if ((xCoordDoctor2 < 155 && xCoordDoctor2 > 0) && (yCoordDoctor2 < 203 && yCoordDoctor2 > 0)) {
			totalDoctor2++;
		}
	}

	doctor1.setMinutiaeTotal(totalDoctor1);
	doctor2.setMinutiaeTotal(totalDoctor2);

	int targetMinimum = 0.75 * doctor1.getMinutiaeTotal(); //leave as an int so it rounds down
	int numberMatching = 0; //will keep track how many match, if it ever reaches the targetMinimum, we will break and return true.

	//first easy comparison can be made now.
	if ((.75 * doctor1.getMinutiaeTotal()) > doctor2.getMinutiaeTotal()) {
		return false;
	}
	else {
		for (int i = 0; i < 50; i++) {
			//checking to see if the minutiae coordinates are the same.
			if ((doctor1.getXCoordAtPos(i) == doctor2.getXCoordAtPos(i)) && (doctor1.getYCoordAtPos(i) == doctor2.getYCoordAtPos(i))) {
				numberMatching++;
			}
			if (numberMatching == targetMinimum) {
				return true;
			}
		}
	}

	return false;
}