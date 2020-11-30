/*
2019 DIP Team Project #1
FingerPrint Reconition
by Team1 (권동현, 임정혁, 임현호, 최진우)

Modified by Team 6 (Nicholas Gross, Jamee Wise, Daniel Bliegh, Nate Evans, Matthew Kinnison)
*/

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "filenameOperations.h"
#include "doctor.h"
#include "gabor.hpp"
#include "orientation.hpp"
#include "segmentation.hpp"
#include "thinning.hpp"
#include "Minutiae.hpp"
#include "distance.hpp"

using namespace std;
using namespace cv;


int main() {
	// orientation block size
	// rows, cols must be devided by block size
	int conversionType;
	int block_size = 7;
	int W = 154;                     //width
	int H = 203;                     //height
	int M = 1;                        //number of extracted minutiae
	int SP = 1;                        //singular point
	int X[50];                     //x coordinates of minutiae 
	int Y[50];                     //y coordinates of minutiae
	unsigned char O[50];            //Orientation of minutiae (0~359)
	unsigned char T[50];                     //type of minutiae (1:ending   3:bifurcation   10:core   11:delta)

	Mat src, show, segmented2, gabored, imgt, result, coredelta;
	
	ofstream output("output.bin", ios::out | ios::binary);

	filenameOperations image;
	string input;
	string firstName, lastName;
	int id;
	int menu = 0;
	int prevMenu = 0;
	bool exists = false;

	doctor newDoctor;
	doctor Nick;
	doctor Jamee;
	doctor Matt;

	// intialize 3 doctors
	for (int i = 0; i < 3; i++) {
		if (i == 0) {
			input = "image/etc/50.bmp";
			image = filenameOperations(input);
		}
		else if (i == 1) {
			input = "image/etc/49.bmp";
			image = filenameOperations(input);
		}
		else if (i == 2) {
			input = "image/etc/48.bmp";
			image = filenameOperations(input);
		}


		conversionType = image.convertFile();
		src = imread(input);
		cvtColor(src, src, conversionType);
		Size size = { 154,203 };

		// resize image
		resize(src, src, size);

		Mat segmented;
		// segmantation image
		segmented2 = segmentation(src, segmented);
		// normalize image
		equalizeHist(src, src);


		// block oriented
		pair<Mat, vector<pair<float, float>>> returned = orientation(NULL, NULL, NULL, NULL, NULL, src, block_size);
		show = returned.first;
		vector<pair<float, float>> vec = returned.second;

		pair<Mat, vector<pair<float, float>>> returned2 = orientation(&SP, X, Y, O, T, src, 7, true);
		coredelta = returned2.first;

		// gabor filter
		gabored = gabor(src, vec, block_size) + segmented2;

		Mat gabored_end;
		// binarization
		threshold(gabored, gabored_end, 1, 255, THRESH_BINARY_INV);

		// thinning
		imgt = thinning(gabored_end);

		// find minutiae and visual them
		result = printMinutiae(imgt, segmented2, vec, block_size, size, src, &M, SP, X, Y, O, T);
		// measure distance between ridge
		calculate(imgt, segmented2);

		if (i == 0) {
			Nick = doctor((string)"Nick", (string)"Gross", 1, X, Y);
		}
		else if (i == 1) {
			Jamee = doctor((string)"Jamee", (string)"Wise", 2, X, Y);
		}
		else if (i == 2) {
			Matt = doctor((string)"Matt", (string)"Kennison", 2, X, Y);
		}
	}

	//reset the input after intilialization.
	input = "";

	do {
		
		cout << "\n\nMENU" << endl;
		cout << "(1) Input fingerprint." << endl;
		cout << "(2) Start fingerprint scan." << endl;
		cout << "(3) Exit." << endl;
		cin >> menu;
		
		while (!cin)
		{
			cin.clear();
			cin.ignore(10000, '\n');
			cout << "Please, it must be a number!" << endl;
			cin >> menu;
		} 
		
		if (menu != 2) {
			input = "";
		}

		switch (menu) {
			case 1: {
				while (!exists) {
					cout << "Please input the file path, starting from image directory and including extension: " << endl;
					cin >> input;
					ifstream f(input.c_str());
					exists = f.good();
					f.close();
					if (!exists) {
						cout << "File does not exist." << endl;
						image = filenameOperations(input);
						conversionType = image.convertFile();
					}
					else {
						cout << "File found. Start Scan.\n\n";
						image = filenameOperations(input);
						conversionType = image.convertFile();
					}
				}
				
				break;
			}
			case 2: {
				cout << "Enter your id: " << endl;
				cin >> id;

				cout << "Starting fingerprint scan... " << endl;
				bool fileGiven;
				if (input.empty() == true) {
					cout << "No input file given.\n\n";
					break;
				}
				else {
					
					/*PUT THE OTHER CODE HERE*/
					//check again in case they skip straight here
					//conversionType = image.convertFile();
					src = imread(input);
					cvtColor(src, src, conversionType);
					Size size = { 154,203 };

					// resize image
					resize(src, src, size);

					Mat segmented;
					// segmantation image
					segmented2 = segmentation(src, segmented);
					// normalize image
					equalizeHist(src, src);


					// block oriented
					pair<Mat, vector<pair<float, float>>> returned = orientation(NULL, NULL, NULL, NULL, NULL, src, block_size);
					show = returned.first;
					vector<pair<float, float>> vec = returned.second;

					pair<Mat, vector<pair<float, float>>> returned2 = orientation(&SP, X, Y, O, T, src, 7, true);
					coredelta = returned2.first;

					// gabor filter
					gabored = gabor(src, vec, block_size) + segmented2;

					Mat gabored_end;
					// binarization
					threshold(gabored, gabored_end, 1, 255, THRESH_BINARY_INV);

					// thinning
					imgt = thinning(gabored_end);

					// find minutiae and visual them
					result = printMinutiae(imgt, segmented2, vec, block_size, size, src, &M, SP, X, Y, O, T);
					// measure distance between ridge
					calculate(imgt, segmented2);

					newDoctor = doctor(firstName, lastName, id, X, Y);

					if (id == 1) {
						if (Nick == newDoctor) {
							cout << "\n\n**************************\nFingerprint accepted.\n**************************\n" << endl;
						}
						else {
							cout << "\n\n**************************\nFingerprint denied.\n**************************\n" << endl;
						}
					}
					else if (id == 2) {
						if (Jamee == newDoctor) {
							cout << "\n\n**************************\nFingerprint accepted.\n**************************\n" << endl;
						}
						else {
							cout << "\n\n**************************\nFingerprint denied.\n**************************\n" << endl;
						}
					}
					else if (id == 3) {
						if (Matt == newDoctor) {
							cout << "\n\n**************************\nFingerprint accepted.\n**************************\n" << endl;
						}
						else {
							cout << "\n\n**************************\nFingerprint denied.\n**************************\n" << endl;
						}
					}
					else {
						cout << "Fingerprint denied." << endl;
					}

					pyrUp(src, src);
					imshow("src", src);

					pyrUp(show, show);
					imshow("show", show);

					pyrUp(segmented2, segmented2);
					imshow("segmented area", segmented2);

					gabored.convertTo(gabored, CV_8U);
					pyrUp(gabored, gabored);
					imshow("gabored", gabored);

					imgt.convertTo(imgt, CV_8U);
					pyrUp(imgt, imgt);
					imshow("thinned", imgt);

					pyrUp(result, result);
					imshow("check", result);

					pyrUp(coredelta, coredelta);
					imshow("coredelta", coredelta);

					cout << "End!" << endl;

					//Input Minutiae Data Value
					output.write((char*)&W, sizeof(int));
					output.write((char*)&H, sizeof(int));
					output.write((char*)&M, sizeof(int));
					output.write((char*)&SP, sizeof(int));
					for (int i = 0; i < 50; i++) {
						output.write((char*)&X[i], sizeof(int));
						output.write((char*)&Y[i], sizeof(int));
						output.write((char*)&O[i], sizeof(char));
						output.write((char*)&T[i], sizeof(char));
					}

					waitKey(5000);

					return EXIT_SUCCESS;
				}
				
			}
			
		}
	} while (menu != 3); 
	

	return 0;
}
