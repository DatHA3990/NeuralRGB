// visual studio mandatory include file
#include "stdafx.h"

// open cv libraries
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

// std libraries
#include <fstream>
#include <iostream>
#include <map>
#include <ios>
#include <sstream>
#include <string>
#include <vector>

#define NUM_FILE 10
#define NUM_COLOR 4

// define a new color that we learned of compare
struct Color {
	std::string colorName;
	cv::Scalar bgr;
	double tolerance = 1;
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
		color.push_back(currentColor);
		std::cout << color[j].colorName << " : " << color[j].bgr << std::endl;
	}
	std::cout << std::endl;
}

// is the color close enough to the one we want
bool isColor(double percentage, double tolerance) {
	return percentage >= tolerance;
}

// get the best match of color, in other words, it will get the color that looks the most alike to the one we inputed
Color getHighestMatchColor(std::vector<Color> color, std::vector<double> accuracy) {
	double bestMatch = *std::max_element(accuracy.begin(), accuracy.end());
	int distance = std::distance(accuracy.begin(), std::find(accuracy.begin(), accuracy.end(), bestMatch));
	std::cout << color[distance].colorName;
	return color[distance];
}

// get a boolean input from user
bool getBoolInput() {
	std::cout << "Was I correct? 1:Yes 0:No >>>";
	std::string x;
	std::cin >> x;
	if (x == "1")
		return true;
	else if (x == "0")
		return false;
	else
		getBoolInput();
}

// change to tolerance according to what we get so that we can get better
double getNewTolerance(double tolerance, bool attempt, bool isCorrect) {
	if (isCorrect == true)
		return tolerance;
	else if (attempt == true)
		return tolerance + 0.1;
	else if (attempt == false)
		return tolerance - 0.1;
	else
		return tolerance;
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

// test if we could see if a color is the one we want
void test(Color &color, std::string fname, std::string testedColorName) {
	cv::Mat image = cv::imread(fname, cv::IMREAD_COLOR);
	cv::Scalar imgBgr = cv::mean(image);
	std::cout << imgBgr << std::endl;

	double accuracy = getColorAccuracy(color.bgr, imgBgr);
	std::cout << accuracy << std::endl;

	bool isInputColor = isColor(accuracy, color.tolerance);

	std::string message = testedColorName + (isInputColor ? " is " : " is not ") + color.colorName;
	std::cout << message << std::endl;

	bool correct = getBoolInput();
	color.tolerance = getNewTolerance(color.tolerance, isInputColor, correct);
	std::cout << color.tolerance << std::endl << std::endl;
}

// guest the color
Color colorGuest(std::vector<Color> color, std::string fname) {
	cv::Mat image = cv::imread(fname, cv::IMREAD_COLOR);
	cv::Scalar imgBgr = cv::mean(image);

	std::vector<double> accuracy;

	for (int i = 0; i < color.size(); i++)
		accuracy.push_back(getColorAccuracy(imgBgr, color[i].bgr));

	double maxVal = *std::max_element(accuracy.begin(), accuracy.end());
	int loc = std::distance(accuracy.begin(), std::find(accuracy.begin(), accuracy.end(), maxVal));
	Color bestColor = color[loc];

	for (int i = 0; i < color.size(); i++)
		std::cout << color[i].colorName << " : " << accuracy[i] << std::endl;
	std::cout << bestColor.colorName << std::endl;
	return bestColor;
}

// main
int main() {
	std::cout << "1.07" << std::endl << std::endl; // print code version

	std::vector<Color> color; // color vector
	training(color);          // train neural net and store learned color in vector

	// TESTS

	/*
	while (true) {
		test(color[0], "../TestData/blue.jpg", "blue");
		test(color[0], "../TestData/orange.jpg", "orange");
		test(color[0], "../TestData/purple.jpg", "purple");
	}
	*/
	colorGuest(color, "../TestData/orange.jpg");
	colorGuest(color, "..\TestData/red.jpg");

	while (1);
}