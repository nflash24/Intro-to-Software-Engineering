#pragma once

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class filenameOperations {
public:
	filenameOperations();
	filenameOperations(std::string filePath);
	~filenameOperations();
	std::string getFilename();
	std::string getFileExtension();
	int convertFile(); //convert from one file type to greyscale
	void parseFilename(); //break down the file into the extension etc.
	std::vector<std::string> split_string(std::string str, char sep);

private:
	std::string filename;
	std::string extension;
	std::string fullPath;
	void setFilename(std::string fname);
	void setExtension(std::string ext);
};
