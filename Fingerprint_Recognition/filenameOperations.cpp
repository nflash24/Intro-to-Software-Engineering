#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "filenameOperations.h"

/*
	Default empty constructor.
*/
filenameOperations::filenameOperations() {
	// Empty constructor
}

/*
	@param filePath : The path for the fingerprint image.
*/
filenameOperations::filenameOperations(std::string filePath) {
	fullPath = filePath;
	parseFilename();
}

/*
	Destructor.
*/
filenameOperations::~filenameOperations() {
	// Empty destructor class.
}

/*
	Return the filename.
*/
std::string filenameOperations::getFilename() {
	return filename;
}

/*
	Return the extension of the file.
*/
std::string filenameOperations::getFileExtension() {
	return extension;
}

/*
	@param fname : File name to be set.

	Set the filename variable to the fname variable.
*/
void filenameOperations::setFilename(std::string fname) {
	filename = fname;
}

/*
	@param ext : Extension to be set.

	Set the extension variable to the ext variable.
*/
void filenameOperations::setExtension(std::string ext) {
	extension = ext;
}

/*
	Return an enumerated integer that relates to the color conversion
	within opencv. If the file extension is not one of the accepted 
	file types, close the program.
*/
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
		std::cout << "No file conversions... file not proper type... try again." << std::endl;
		//exit(EXIT_SUCCESS);
	}
}

/*
	Perform operations on the object filePath in order 
	to break down the file path further into designated parts.
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

/*
	@param str : The string with which to iterate over and split.
	@param sep : The seperator we are looking for during iterations.

	Iterate over the str variable and place seperated strings into the vector
	based on when the sep variable is found within the str variable. This will
	then return the completed vector of strings that were successfully delimited
	by the seperator.
*/
std::vector<std::string> filenameOperations::split_string(std::string str, char sep) {

	std::vector<std::string> tokens;
	std::stringstream check1(str);
	std::string intermediate;
	while (std::getline(check1, intermediate, sep)) {
		tokens.push_back(intermediate);
	}

	return tokens;
} 