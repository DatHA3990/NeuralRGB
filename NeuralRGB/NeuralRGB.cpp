#include "stdafx.h"''

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

cv::Scalar getAvg(std::vector<cv::Scalar> imgData) {
	cv::Scalar avg;
	for (int i = 0; i < 3; i++)
		avg[i] = 0;
	int num = imgData.size();
	for (int rgb = 0; rgb < 3; rgb++) {
		for (int i = 0; i < num; i++) {
			avg[rgb] += imgData[i][rgb];
		}
	}
	for (int i = 0; i < 3; i++)
		avg[i] /= num;
	return avg;
}

void training(cv::Scalar &avg) {
	std::vector<cv::Scalar> imgData;
	for (int i = 0; i <= 6; i++) {
		std::string fname = "../TrainData/" + std::to_string(i) + ".jpg";
		cv::Mat image = imread(fname, IMREAD_COLOR);
		cv::Scalar imgBgr = cv::mean(image);
		imgData.push_back(imgBgr);
		waitKey(1);
	}
	avg = getAvg(imgData);
	std::cout << avg << std::endl << std::endl;
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

bool isColor(cv::Mat img, cv::Scalar colorBGR, int tolerance) {
	cv::Scalar imgBGR = cv::mean(img);
	std::cout << imgBGR << std::endl;
	return isInRange(imgBGR, colorBGR, tolerance);
}

bool getBoolInput() {
	std::cout << "Was I correct? >>>";
	std::string x;
	std::cin >> x;
	if (x == "1")
		return true;
	if (x == "0")
		return false;
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
		getNewTolerance;
}

void test(cv::Scalar colorBGR, std::string fname, int &tolerance, std::string testedColorName, std::string colorName) {
	cv::Mat image = imread(fname, IMREAD_COLOR);
	bool isBlue = isColor(image, colorBGR, tolerance);

	std::string message = testedColorName + (isBlue ? " is " : " is not ") + colorName;
	std::cout << message << std::endl;

	bool correct = getBoolInput();
	tolerance = getNewTolerance(tolerance, isBlue, correct);
	std::cout << tolerance << std::endl << std::endl;
}

int main() {
	cv::Scalar blue;
	training(blue);

	int tolerance = 0;

	while (1) {
		test(blue, "../TestData/blue.jpg", tolerance, "blue", "blue");
		test(blue, "../TestData/orange.jpg", tolerance, "orange", "blue");
		test(blue, "../TestData/purple.jpg", tolerance, "purple", "blue");
	}
}