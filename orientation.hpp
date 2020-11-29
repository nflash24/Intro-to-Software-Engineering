#ifndef ORENTATION_HPP
#define ORENTATION_HPP

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <queue>

using namespace std;
using namespace cv;

pair<Mat, vector<pair<float, float>>> orientation(int* SP, int X[], int Y[], unsigned char O[], unsigned char T[], Mat src, int size = 8, bool coredelta = false) {
	Mat inputImage = src;

	inputImage.convertTo(inputImage, CV_32F, 1.0 / 255, 0);

	medianBlur(inputImage, inputImage, 3);

	int blockSize = size;

	Mat fprintWithDirectionsSmoo = inputImage.clone();
	Mat coredeltaPrint = inputImage.clone();
	cvtColor(coredeltaPrint, coredeltaPrint, COLOR_GRAY2BGR);

	Mat tmp(inputImage.size(), inputImage.type());
	Mat coherence(inputImage.size(), inputImage.type());
	Mat orientationMap = tmp.clone();

	//Gradiants x and y
	Mat grad_x, grad_y;

	Sobel(inputImage, grad_x, inputImage.depth(), 1, 0, 3);
	Sobel(inputImage, grad_y, inputImage.depth(), 0, 1, 3);

	Mat Fx(inputImage.size(), inputImage.type()),
		Fy(inputImage.size(), inputImage.type()),
		Fx_gauss,
		Fy_gauss;
	Mat smoothed(inputImage.size(), inputImage.type());

	int width = inputImage.cols;
	int height = inputImage.rows;
	int blockH;
	int blockW;

	vector<pair<float, float>> vec;
	vector<int> cnt;
	for (int i = 0; i < height; i += blockSize) {
		for (int j = 0; j < width; j += blockSize) {
			float Gsx = 0.0;
			float Gsy = 0.0;
			float Gxx = 0.0;
			float Gyy = 0.0;

			//for check bounds of img
			blockH = ((height - i) < blockSize) ? (height - i) : blockSize;
			blockW = ((width - j) < blockSize) ? (width - j) : blockSize;

			//average at block W¬çW
			for (int u = i; u < i + blockH; u++) {
				for (int v = j; v < j + blockW; v++) {
					Gsx += (grad_x.at<float>(u, v)*grad_x.at<float>(u, v)) - (grad_y.at<float>(u, v)*grad_y.at<float>(u, v));
					Gsy += 2 * grad_x.at<float>(u, v) * grad_y.at<float>(u, v);
					Gxx += grad_x.at<float>(u, v)*grad_x.at<float>(u, v);
					Gyy += grad_y.at<float>(u, v)*grad_y.at<float>(u, v);
				}
			}

			float coh = sqrt(pow(Gsx, 2) + pow(Gsy, 2)) / (Gxx + Gyy);
			//smoothed
			float fi = 0.5f * fastAtan2(Gsy, Gsx) * CV_PI / 180.0f;

			Fx.at<float>(i, j) = cos(2 * fi);
			Fy.at<float>(i, j) = sin(2 * fi);

			//fill blocks
			for (int u = i; u < i + blockH; u++) {
				for (int v = j; v < j + blockW; v++) {
					orientationMap.at<float>(u, v) = fi;
					Fx.at<float>(u, v) = Fx.at<float>(i, j);
					Fy.at<float>(u, v) = Fy.at<float>(i, j);
					coherence.at<float>(u, v) = (coh < 0.85f) ? 1.0f : 0.0f;
				}
			}
		}
	}

	GaussianBlur(Fx, Fx_gauss, Size(5, 5), 1, 1);
	GaussianBlur(Fy, Fy_gauss, Size(5, 5), 1, 1);

	vector<vector<float>> vec_mymy(height, vector<float>(width, 0.0f));
	for (int m = 0; m < height; m++) {
		for (int n = 0; n < width; n++) {
			smoothed.at<float>(m, n) = 0.5f * fastAtan2(Fy_gauss.at<float>(m, n), Fx_gauss.at<float>(m, n)) * CV_PI / 180.0f;
			if ((m % blockSize) == 0 && (n % blockSize) == 0) {
				int x = n;
				int y = m;
				int ln = sqrt(2 * pow(blockSize, 2)) / 2;
				float dx = ln * cos(smoothed.at<float>(m, n) - CV_PI / 2.0f);
				float dy = ln * sin(smoothed.at<float>(m, n) - CV_PI / 2.0f);
				vec.push_back({ dx,dy });

				float my = dy / (dx + FLT_EPSILON);

				float mymy = my;

				// Quantazation with 4
				if (2.0f <= mymy)
					mymy = FLT_MAX;
				else if (0.5f <= mymy && mymy < 2.0f)
					mymy = 1.0f;
				else if (-0.5f <= mymy && mymy < 0.5f)
					mymy = 0.0f;
				else if (-2.0f <= mymy && mymy < -0.5f)
					mymy = -1.0f;
				else if (mymy < -2.0f)
					mymy = FLT_MAX;

				vec_mymy[m][n] = mymy;

				int xx = (blockH / 2) / sqrt(pow(my, 2) + 1);
				int yy = my * xx;

				if (coredelta) {
					if (mymy == 1.0f) {
						xx = blockSize / 2 - 1;
						yy = blockSize / 2 - 1;
					}
					else if (mymy == -1.0f) {
						xx = blockSize / 2 - 1;
						yy = -(blockSize / 2 - 1);
					}
					else if (mymy == 0.0f) {
						xx = blockSize / 2 - 1;
						yy = 0;
					}
					else if (mymy == FLT_MAX) {
						xx = 0;
						yy = blockSize / 2 - 1;
					}
				}

				int mid_x = n + blockH / 2;
				int mid_y = m + blockH / 2;
				if (xx == 0 && yy == 0)
					yy = blockH / 2;

				if (!coredelta)
					line(fprintWithDirectionsSmoo, Point(mid_x + xx, mid_y + yy), Point(mid_x - xx, mid_y - yy), Scalar::all(255), 1, LINE_AA, 0);
			}
		}
	}

	priority_queue<pair<int, pair<int, int>>> pq_core;
	priority_queue<pair<int, pair<int, int>>> pq_delta;

	if (coredelta) {
		for (int m = 0; m < height; m++) {
			for (int n = 0; n < width; n++) {
				if (m % blockSize == 0 && n % blockSize == 0) {
					if (0 <= m - blockSize && m + blockSize < height &&
						0 <= n - blockSize && n + blockSize < width) {
						// If left middle: +, middle middle: =, middle right: -, find the horizontal and vertical values from the top and bottom respectively
						if (vec_mymy[m][n - blockSize] == -1.0f && vec_mymy[m][n] == 0.0f && vec_mymy[m][n + blockSize] == 1.0f) {
							int up_up = 0;
							int up_side = 0;
							int down_up = 0;
							int down_side = 0;

							// Upper left
							if (vec_mymy[m - blockSize][n - blockSize] == FLT_MAX)
								up_up += 2;
							else if (vec_mymy[m - blockSize][n - blockSize] == 1.0f || vec_mymy[m - blockSize][n - blockSize] == -1.0f) {
								up_up++;
								up_side++;
							}
							else if (vec_mymy[m - blockSize][n - blockSize] == 0.0f)
								up_side += 2;

							// Middle stomach
							if (vec_mymy[m - blockSize][n] == FLT_MAX)
								up_up += 2;
							else if (vec_mymy[m - blockSize][n] == 1.0f || vec_mymy[m - blockSize][n] == -1.0f) {
								up_up++;
								up_side++;
							}
							else if (vec_mymy[m - blockSize][n] == 0.0f)
								up_side += 2;

							// Upper right
							if (vec_mymy[m - blockSize][n + blockSize] == FLT_MAX)
								up_up += 2;
							else if (vec_mymy[m - blockSize][n + blockSize] == 1.0f || vec_mymy[m - blockSize][n + blockSize] == -1.0f) {
								up_up++;
								up_side++;
							}
							else if (vec_mymy[m - blockSize][n + blockSize] == 0.0f)
								up_side += 2;


							// Lower left
							if (vec_mymy[m + blockSize][n - blockSize] == FLT_MAX)
								down_up += 2;
							else if (vec_mymy[m + blockSize][n - blockSize] == 1.0f || vec_mymy[m + blockSize][n - blockSize] == -1.0f) {
								down_up++;
								down_side++;
							}
							else if (vec_mymy[m + blockSize][n - blockSize] == 0.0f)
								down_side += 2;

							// Middle down
							if (vec_mymy[m + blockSize][n] == FLT_MAX)
								down_up += 2;
							else if (vec_mymy[m + blockSize][n] == 1.0f || vec_mymy[m + blockSize][n] == -1.0f) {
								down_up++;
								down_side++;
							}
							else if (vec_mymy[m + blockSize][n] == 0.0f)
								down_side += 2;

							// right below
							if (vec_mymy[m + blockSize][n + blockSize] == FLT_MAX)
								down_up += 2;
							else if (vec_mymy[m + blockSize][n + blockSize] == 1.0f || vec_mymy[m + blockSize][n + blockSize] == -1.0f) {
								down_up++;
								down_side++;
							}
							else if (vec_mymy[m + blockSize][n + blockSize] == 0.0f)
								down_side += 2;

							// core, delta °è»ê
							int cnt_core = up_side + down_up;
							int cnt_delta = up_up + down_side;

							// Push only if the difference is more than 2, core if there are more cores, delta if there are more deltas
							if (abs(cnt_delta - cnt_core) > 2) {
								if (cnt_delta <= cnt_core) {
									pq_core.push({ cnt_core - cnt_delta,{ m, n } });
									T[*SP] = 10;
								}
								else {
									pq_delta.push({ cnt_delta - cnt_core,{ m, n } });
									T[*SP] = 11;
								}
								X[*SP] = m;
								Y[*SP] = n;
								O[*SP] = 0;
								*SP++;
							}
						}
					}
				}
			}
		}
	}
	else {
		// Remove if there is a transverse scar
		int my_index = 0;
		for (int m = 0; m < height; m++) {
			for (int n = 0; n < width; n++) {
				if (m % blockSize == 0 && n % blockSize == 0) {
					if (0 <= m - blockSize && m + blockSize < height) {
						// If the top and bottom are vertical
						if (vec_mymy[m - blockSize][n] == FLT_MAX && vec_mymy[m + blockSize][n] == FLT_MAX) {
							// If the current orientation includes horizontal
							if (vec_mymy[m][n] == 1.0f || vec_mymy[m][n] == -1.0f || vec_mymy[m][n] == 0.0f) {
								// If the orientation is different, insert a new vertical orientation
								if (vec[my_index - width / blockSize].second * vec[my_index + width / blockSize].second < 0.0f)
									vec[my_index] = { 0.0f, FLT_MAX };
								// If the directions are the same, the average of the two values is added vertically.
								else {
									vec[my_index] = {
										(vec[my_index - width / blockSize].first + vec[my_index + width / blockSize].first) / 2 ,
										(vec[my_index - width / blockSize].second + vec[my_index + width / blockSize].second) / 2
									};
								}
							}
						}
					}

					my_index++;
				}
			}
		}
	}

	// If there is a core, draw a red circle
	if (!pq_core.empty())
		circle(coredeltaPrint, Point(pq_core.top().second.second + blockSize / 2, pq_core.top().second.first + blockSize / 2), 5, Scalar(0, 0, 255), 1, 16);

	// If there is a delta, draw a red cross #2
	if (!pq_delta.empty()) {
		int mmm = pq_delta.top().second.first;
		int nnn = pq_delta.top().second.second;

		line(coredeltaPrint, Point(nnn + blockSize / 2, mmm), Point(nnn + blockSize / 2, mmm + blockSize), Scalar(0, 0, 255), 1, LINE_AA, 0);
		line(coredeltaPrint, Point(nnn, mmm + blockSize / 2), Point(nnn + blockSize, mmm + blockSize / 2), Scalar(0, 0, 255), 1, LINE_AA, 0);
		pq_delta.pop();
	}
	if (!pq_delta.empty()) {
		int mmm = pq_delta.top().second.first;
		int nnn = pq_delta.top().second.second;
		line(coredeltaPrint, Point(nnn + blockSize / 2, mmm), Point(nnn + blockSize / 2, mmm + blockSize), Scalar(0, 0, 255), 1, LINE_AA, 0);
		line(coredeltaPrint, Point(nnn, mmm + blockSize / 2), Point(nnn + blockSize, mmm + blockSize / 2), Scalar(0, 0, 255), 1, LINE_AA, 0);
	}


	normalize(orientationMap, orientationMap, 0, 1, NORM_MINMAX);
//	imshow("Orientation field", orientationMap);

	orientationMap = smoothed.clone();

	normalize(smoothed, smoothed, 0, 1, NORM_MINMAX);

	pyrUp(smoothed, smoothed);
//	imshow("Smoothed orientation field", smoothed);
//	imshow("Coherence", coherence);

	pair<Mat, vector<pair<float, float>>> returning;

	if (coredelta)
		returning = { coredeltaPrint, vec };
	else
		returning = { fprintWithDirectionsSmoo, vec };

	return returning;
}

#endif