#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "filenameOperations.h"

filenameOperations::filenameOperations() {
	// Empty constructor
}

filenameOperations::filenameOperations(std::string filePath) {
	fullPath = filePath;
	parseFilename();
}

filenameOperations::~filenameOperations() {
	// Empty destructor class.
}

std::string filenameOperations::getFilename() {
	return filename;
}

std::string filenameOperations::getFileExtension() {
	return extension;
}

void filenameOperations::setFilename(std::string fname) {
	filename = fname;
}

void filenameOperations::setExtension(std::string ext) {
	extension = ext;
}

int filenameOperations::convertFile() {
	
	std::cout << "Reading File..." << std::endl;

	if (extension == "bmp") {
		return cv::COLOR_RGB2GRAY;
	}
	else if (extension == "png") {
		return cv::COLOR_RGB2GRAY;
	}
	else if (extension == "jpg") {
		return cv::COLOR_RGB2GRAY;
	}
	else {
		std::cout << "No file conversions... file not proper type... ending program" << std::endl;
		exit(EXIT_SUCCESS);
	}
}

/*	   1. We need to parse the whole filename searching for slashes, we will want to keep this information
	   2. We need to find the final "." and grab the final characters... this should give use the file extension
	   3. The filename will then be derived from step 2 as well.
*/

void filenameOperations::parseFilename() {

	std::vector<std::string> arr = split_string(fullPath, '/');
	int arrSize = arr.size();
	
	//we now have all of our arguments, need to find the extension of the last one now.
	std::vector<std::string> extensionArr = split_string(arr[arrSize - 1], '.');
	filename = extensionArr[0];
	extension = extensionArr[1];
	printf("Accepting image file type: %s\n", extension);
}



std::vector<std::string> filenameOperations::split_string(std::string str, char sep) {

	std::vector<std::string> tokens;
	std::stringstream check1(str);
	std::string intermediate;
	while (std::getline(check1, intermediate, sep)) {
		tokens.push_back(intermediate);
	}

	return tokens;
} 