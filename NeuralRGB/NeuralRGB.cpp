#include "stdafx.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <fstream>
#include <iostream>
#include <map>
#include <ios>
#include <sstream>
#include <string>
#include <vector>

#define NUM_FILE 10
#define NUM_COLOR 3

struct Color {
	std::string colorName;
	cv::Scalar bgr;
	double tolerance = 1;
};

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

void training(std::vector<Color> &color) {
	for (int j = 0; j < NUM_COLOR; j++) {
		std::ifstream file;
		std::string nfname = "../TrainData/" + std::to_string(j + 1) + "/0.txt";
		file.open(nfname);
		std::string colorName;
		file >> colorName;
		file.close();
		std::vector<cv::Scalar> imgData;
		for (int i = 1; i <= NUM_FILE; i++) {
			std::string fname = "../TrainData/" + std::to_string(j + 1) + "/" + std::to_string(i) + ".jpg";
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

bool isColor(double percentage, double tolerance) {
	return percentage >= tolerance;
}

Color getHighestMatchColor(std::vector<Color> color, std::vector<double> accuracy) {
	double bestMatch = *std::max_element(accuracy.begin(), accuracy.end());
	int distance = std::distance(accuracy.begin(), std::find(accuracy.begin(), accuracy.end(), bestMatch));
	std::cout << color[distance].colorName;
	return color[distance];
}

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

double getColorAccuracy(cv::Scalar color1, cv::Scalar color2) {
	cv::Scalar difference;
	double accuracy = 0;
	for (int i = 0; i < 3; i++)
		difference[i] = fabs(color1[i] - color2[i]);
	for (int i = 0; i < 3; i++)
		accuracy += difference[i];
	return 1 - ((accuracy / 3) / 255);
}

void test(Color &color, std::string fname, std::string testedColorName) {
	cv::Mat image = cv::imread(fname, cv::IMREAD_COLOR);
	cv::Scalar imgBgr = cv::mean(image);
	std::cout << imgBgr << std::endl;

	double accuracy = getColorAccuracy(color.bgr, imgBgr);
	std::cout << accuracy << std::endl;

	bool isBlue = isColor(accuracy, color.tolerance);

	std::string message = testedColorName + (isBlue ? " is " : " is not ") + color.colorName;
	std::cout << message << std::endl;

	bool correct = getBoolInput();
	color.tolerance = getNewTolerance(color.tolerance, isBlue, correct);
	std::cout << color.tolerance << std::endl << std::endl;
}

Color colorGuest(std::vector<Color> color, std::string fname) {
	cv::Mat image = cv::imread(fname, cv::IMREAD_COLOR);
	cv::Scalar imgBgr = cv::mean(image);

	std::vector<double> accuracy;

	for (int i = 0; i < color.size(); i++)
		accuracy.push_back(getColorAccuracy(imgBgr, color[i].bgr));

	double maxVal = *std::max_element(accuracy.begin(), accuracy.end());
	int loc = std::distance(accuracy.begin(), std::find(accuracy.begin(), accuracy.end(), maxVal));
	Color bestColor = color[loc];
	std::cout << bestColor.colorName << std::endl;

	return bestColor;
}

int main() {
	std::cout << "1.05" << std::endl << std::endl;

	std::vector<Color> color;
	training(color);

	/*
	while (true) {
		test(color[0], "../TestData/blue.jpg", "blue");
		test(color[0], "../TestData/orange.jpg", "orange");
		test(color[0], "../TestData/purple.jpg", "purple");
	}
	*/
	colorGuest(color, "../TestData/green.jpg");
	colorGuest(color, "../TestData/red.jpg");
	colorGuest(color, "../TestData/blue.jpg");
	while (1);
}