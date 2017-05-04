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

struct Color {
	std::string colorName;
	cv::Scalar bgr;
	int tolerance = 0;
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
	std::ifstream file;
	file.open("../TrainData/1/0.txt");
	std::string colorName;
	file >> colorName;
	file.close();
	std::vector<cv::Scalar> imgData;
	for (int i = 1; i <= 10; i++) {
		std::string fname = "../TrainData/1/" + std::to_string(i) + ".jpg";
		cv::Mat image = cv::imread(fname, cv::IMREAD_COLOR);
		cv::Scalar imgBgr = cv::mean(image);
		imgData.push_back(imgBgr);
		cv::waitKey(1);
	}
	Color currentColor;
	currentColor.colorName = colorName;
	currentColor.bgr = getAvg(imgData);
	color.push_back(currentColor);
	std::cout << color[0].colorName << " : " << color[0].bgr << std::endl << std::endl;
}

bool isInRange(cv::Scalar tested, cv::Scalar color, int tolerance) {
	for (int i = 0; i < 3; i++)
		if (tested[i] < color[i] - tolerance)
			return false;
	for (int i = 0; i < 3; i++)
		if (tested[i] > color[i] + tolerance)
			return false;
	return true;
}

bool isColor(cv::Mat img, cv::Scalar colorBgr, int tolerance) {
	cv::Scalar imgBgr = cv::mean(img);
	std::cout << imgBgr << std::endl;
	return isInRange(imgBgr, colorBgr, tolerance);
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

int getNewTolerance(int tolerance, bool attempt, bool isCorrect) {
	if (isCorrect == true)
		return tolerance;
	else if (attempt == true)
		return tolerance - 10;
	else if (attempt == false)
		return tolerance + 10;
	else
		return tolerance;
}

void test(Color &color, std::string fname, std::string testedColorName) {
	cv::Mat image = cv::imread(fname, cv::IMREAD_COLOR);
	bool isBlue = isColor(image, color.bgr, color.tolerance);

	std::string message = testedColorName + (isBlue ? " is " : " is not ") + color.colorName;
	std::cout << message << std::endl;
	
	bool correct = getBoolInput();
	color.tolerance = getNewTolerance(color.tolerance, isBlue, correct);
	std::cout << color.tolerance << std::endl << std::endl;
}

int main() {
	std::cout << "1.02" << std::endl << std::endl;

	std::vector<Color> color;
	training(color);

	while (true) {
		test(color[0], "../TestData/blue.jpg", "blue");
		test(color[0], "../TestData/orange.jpg", "orange");
		test(color[0], "../TestData/purple.jpg", "purple");
	}
}