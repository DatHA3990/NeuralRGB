// visual studio mandatory include file
#include "stdafx.h"

// open cv libraries
#include <opencv2\opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

// windows library
#include <Windows.h>

// std libraries
#include <fstream>
#include <iostream>
#include <ios>
#include <string>
#include <vector>

#define NUM_FILE 10
#define NUM_COLOR 5

// define a new color that we learned of compare
struct Color {
	std::string colorName; // name of the color, ex. red, blue
	cv::Scalar bgr;        // blue, green, and red values in that order
	cv::Scalar difference; // what is the difference between, blue and green, green and red, and red and blue
};

// get the average BGR of a vector of images BGR value
cv::Scalar getAvg(std::vector<cv::Scalar> imgData) {
	cv::Scalar avg;
	for (int i = 0; i < 3; i++)
		avg[i] = 0;
	int num = imgData.size();
	for (int rgb = 0; rgb < 3; rgb++)
		for (int i = 0; i < num; i++)
			avg[rgb] += imgData[i][rgb];
	for (int i = 0; i < 3; i++)
		avg[i] /= num;
	return avg;
}

// get the difference between, blue and green, green and red, and red and blue
cv::Scalar getBgrDifference(cv::Scalar bgr) {
	cv::Scalar difference;
	difference[0] = bgr[0] - bgr[1];
	difference[1] = bgr[1] - bgr[2];
	difference[2] = bgr[2] - bgr[0];
	return difference;
}

// train the neural network
void training(std::vector<Color> &color) {
	for (int j = 0; j < NUM_COLOR; j++) {
		std::ifstream file;
		std::string nfname = "../TrainData/" + std::to_string(j) + "/name.txt";
		file.open(nfname);
		std::string colorName;
		file >> colorName;
		file.close();
		std::vector<cv::Scalar> imgData;
		for (int i = 0; i < NUM_FILE; i++) {
			std::string fname = "../TrainData/" + std::to_string(j) + "/" + std::to_string(i) + ".jpg";
			cv::Mat image = cv::imread(fname, cv::IMREAD_COLOR);
			cv::Scalar imgBgr = cv::mean(image);
			imgData.push_back(imgBgr);
			cv::waitKey(1);
		}
		Color currentColor;
		currentColor.colorName = colorName;
		currentColor.bgr = getAvg(imgData);
		currentColor.difference = getBgrDifference(currentColor.bgr);
		color.push_back(currentColor);
		std::cout << color[j].colorName << " : " << color[j].bgr << std::endl;
	}
	std::cout << std::endl;
}

// get, in percentage, the ressemblance between 2 color
double getColorAccuracy(cv::Scalar color1, cv::Scalar color2) {
	cv::Scalar difference;
	double accuracy = 0;
	for (int i = 0; i < 3; i++)
		difference[i] = fabs(color1[i] - color2[i]);
	for (int i = 0; i < 3; i++)
		accuracy += difference[i];
	return 1 - ((accuracy / 3) / 255);
}

// guest the color
Color colorGuest(std::vector<Color> color, cv::Mat image) {
	cv::Scalar imgBgr = cv::mean(image);
	cv::Scalar imgDifference = getBgrDifference(imgBgr);
	std::vector<double> accuracy;

	for (int i = 0; i < color.size(); i++)
		accuracy.push_back((getColorAccuracy(imgDifference, color[i].difference) + getColorAccuracy(imgBgr, color[i].bgr)) / 2);

	double maxVal = *std::max_element(accuracy.begin(), accuracy.end());
	int loc = std::distance(accuracy.begin(), std::find(accuracy.begin(), accuracy.end(), maxVal));
	Color bestColor = color[loc];

	std::cout << imgBgr << std::endl;
	for (int i = 0; i < color.size(); i++)
		std::cout << color[i].colorName << " : " << accuracy[i] << std::endl;
	std::cout << bestColor.colorName << std::endl << std::endl;
	return bestColor;
}

// main
int main() {
	std::cout << "1.14" << std::endl << std::endl; // print code version

	std::vector<Color> color; // color vector
	training(color);          // train neural net and store learned color in vector

	// TESTTING SEGMENTS

	colorGuest(color, cv::imread("../TestData/darkblue.jpg", cv::IMREAD_COLOR));
	colorGuest(color, cv::imread("../TestData/lightblue.jpg", cv::IMREAD_COLOR));
	while (1);
}