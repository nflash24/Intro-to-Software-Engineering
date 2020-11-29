#pragma once

#include <stdlib.h>
#include <iostream>

using namespace std;

class doctor {
	public:
		doctor();
		doctor(string fname, string lname, int id, int x[50], int y[50]);
		doctor(const doctor& doctor);
		~doctor();
		void setMinutiaeTotal(int total);
		void setMinutiaeCoords(int x[50], int y[50]);
		int getXCoordAtPos(int pos);
		int getYCoordAtPos(int pos);
		int* getMinutiaeXCoords();
		int* getMinutiaeYCoords();
		int getMinutiaeTotal();
		friend ostream& operator<<(ostream& outs, doctor& doctor);

	private:
		int xCoords[50]; // X coordinate for a minutiae 
		int yCoords[50]; // Y coordinate for a minutiae
		string firstName;
		string lastName;
		int minutiaeTotal;
		int ID;
};

bool operator==(const doctor& d1, const doctor& d2);


