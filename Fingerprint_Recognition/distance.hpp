#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>
#include <math.h>

using namespace cv;
using namespace std;

int distance(Mat& src, Point& pt1, Point& pt2) {

	int distanceX = 0, distanceY = 0; // Distance between X coordinates, distance between Y coordinates
	double distance = 0, indiDistance = 0; // Distance between two points, distance between ridges
	vector<Point> line1; // Vector storing coordinates between points
	line1.push_back(pt1); // Push the initial starting point

	//Distance calculation
	distanceX = abs(pt1.x - pt2.x); // Linear distance between x coordinates
	distanceY = abs(pt1.y - pt2.y); // Linear distance between y coordinates
	distance = sqrt(distanceX*distanceX + distanceY * distanceY); // Euclidean plane

	bool chk = false; // Flag for odd-even calculation
	Point temp;
	if (distanceX >= distanceY) { // Diverging depending on whether the distance between X is wider or the distance between Y is wider
		double rate = (double)(distanceX / (distanceY + DBL_EPSILON)); // inclination
		while (line1[line1.size() - 1].x != pt2.x && line1[line1.size() - 1].y != pt2.y) { // Until reaching
			//You have to add alternately to get the right straight line
			if (pt1.y <= pt2.y) {
				if (line1[line1.size() - 1].y >= pt2.y) // exit if y exceeds
					break;
				if (chk) {
					temp = { (int)(line1[line1.size() - 1].x + rate + 1), line1[line1.size() - 1].y + 1 };
					chk = false;
				}
				else {
					temp = { (int)(line1[line1.size() - 1].x + rate), line1[line1.size() - 1].y + 1 };
					chk = true;
				}
			}
			else {
				if (line1[line1.size() - 1].y <= pt2.y) // exit if y exceeds
					break;
				if (chk) {
					temp = { (int)(line1[line1.size() - 1].x + rate + 1), line1[line1.size() - 1].y - 1 };
					chk = false;
				}
				else {
					temp = { (int)(line1[line1.size() - 1].x + rate), line1[line1.size() - 1].y - 1 };
					chk = true;
				}
			}
			line1.push_back(temp);
		}
	}
	else {
		double rate = (double)(distanceY / (distanceX + DBL_EPSILON)); // inclination
		while (line1[line1.size() - 1].x != pt2.x&&line1[line1.size() - 1].y != pt2.y) {
			//You have to add alternately to get the right straight line
			if (pt1.y <= pt2.y) {
				if (line1[line1.size() - 1].y >= pt2.y)
					break;
				if (chk) {
					temp = { line1[line1.size() - 1].x + 1,  (int)(line1[line1.size() - 1].y + rate + 1) };
					chk = false;
				}
				else {
					temp = { line1[line1.size() - 1].x + 1,  (int)(line1[line1.size() - 1].y + rate) };
					chk = true;
				}
			}
			else {
				if (line1[line1.size() - 1].y <= pt2.y)
					break;
				if (chk) {
					temp = { line1[line1.size() - 1].x + 1,  (int)(line1[line1.size() - 1].y - rate - 1) };
					chk = false;
				}
				else {
					temp = { line1[line1.size() - 1].x + 1,  (int)(line1[line1.size() - 1].y - rate) };
					chk = true;
				}
			}
			line1.push_back(temp);
		}
	}
	int count = 0; // Number of ridges
	for (int i = 0; i < line1.size() - 1; i++) {
		if (src.at<uchar>(line1[i].y, line1[i].x) >= 127 && src.at<uchar>(line1[i + 1].y, line1[i + 1].x) < 127) { // When it goes from white to black
			count++;
		}
	}
	// If the number of ridges is less than 3, it is judged as meaningless.
	if (count < 3)
		return 0;

	indiDistance = distance / (double)count;

	return indiDistance;
}


void calculate(Mat imgt, Mat src) {
	imgt /= 255;
	vector<Minutiae> minutiaes = findMinutiae(imgt, src);
	imgt *= 255;

	vector<Point> ending;
	vector<Point> bif;
	int endingN = 0, bifN = 0, coreN = 0, deltaN = 0;

	// Measure the distance between ending point and bifurcation
	for (int i = 0; i < minutiaes.size(); i++) {
		if (minutiaes[i].type == 1) {
			Point temp = { minutiaes[i].x, minutiaes[i].y };
			ending.push_back(temp);
			endingN++;
		}
		else if (minutiaes[i].type == 2) {
			Point temp = { minutiaes[i].x, minutiaes[i].y };
			bif.push_back(temp);
			bifN++;
		}
	}

	int count = 1;
	int distanceMean = 0, distanceMax = 0, distanceMin = 987654321;
	int distanceN = 0;
	for (int i = 0; i < ending.size(); i++) {
		for (int j = 0; j < bif.size(); j++) {
			Point temp1 = { ending[i].x, ending[i].y };
			Point temp2 = { bif[j].x, bif[j].y };
			if (temp1.x <= temp2.x) // The second argument where x is left.
				distanceN = distance(imgt, temp1, temp2);
			else
				distanceN = distance(imgt, temp2, temp1);

			// 0 is returned when the ridge is less than 3, so count is incremented only when it is not
			if (distanceN > 0)
				count++;
			else
				continue;
			// Max
			if (distanceN > distanceMax)
				distanceMax = distanceN;
			// Min
			if (distanceN < distanceMin && distanceN > 0)
				distanceMin = distanceN;
			// Sum for calculate Mean
			distanceMean += distanceN;
		}
	}

	distanceMean /= count;
	cout << "Max : " << distanceMax << endl;
	cout << "Min : " << distanceMin << endl;
	cout << "Mean : " << distanceMean << endl;
}