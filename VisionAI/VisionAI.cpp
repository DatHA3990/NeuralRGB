#include "stdafx.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <vector>

const int NUM_FILE = 10;
const int NUM_COLOR = 7;
const float NUM_VERSION = 2;
const std::string TRAIN_DATA_FOLDER = "../TrainData/";

struct Color {
	std::string colorName;
	cv::Scalar bgr;
	cv::Scalar difference;
};

cv::Scalar getAverage(std::vector<cv::Scalar> imgData) {
	cv::Scalar avg{ 0 };
	for (auto const& img : imgData)
		avg += img;
	double const n = imgData.size();
	return avg / n;
}

cv::Scalar getBgrDifference(cv::Scalar bgr) {
	return { bgr[0] - bgr[1], bgr[1] - bgr[2], bgr[2] - bgr[0] };
}

std::vector<Color> getTrainedColorVector() {
	std::vector<Color> color;
	for (int j = 0; j < NUM_COLOR; j++) {
		std::string colorName;
		{ std::ifstream file{ TRAIN_DATA_FOLDER + std::to_string(j) + "/name.txt" };
		file >> colorName; }
		std::vector<cv::Scalar> imgData;
		for (int i = 0; i < NUM_FILE; i++) {
			imgData.push_back(cv::mean(cv::imread(TRAIN_DATA_FOLDER + std::to_string(j) + "/" + std::to_string(i) + ".jpg", cv::IMREAD_COLOR)));
			cv::waitKey(1);
		}
		cv::Scalar colorBgr = getAverage(imgData);
		color.push_back({ colorName, colorBgr,  getBgrDifference(colorBgr) });
	}
	return color;
}

double getColorAccuracy(cv::Scalar color1, cv::Scalar color2) {
	double accuracy = 0;
	const auto diff = color1 - color2;
	for (int i = 0; i < 3; i++)
		accuracy += std::abs(diff[i]);
	return 1 - ((accuracy / 3) / 255);
}

Color getColor(std::vector<Color> color, cv::Mat image) {
	cv::Scalar imgBgr = cv::mean(image);
	cv::Scalar imgDifference = getBgrDifference(imgBgr);
	auto it = std::max_element(color.begin(), color.end(), [imgDifference](const Color& a, const Color& b) {
		return getColorAccuracy(imgDifference, a.difference) < getColorAccuracy(imgDifference, b.difference);
	});
	return *it;
}

std::vector<std::vector<cv::Point>> getVertex(cv::Mat src, int thresh) {
	cv::Mat src_gray, threshold_output;
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cvtColor(src, src_gray, CV_BGR2GRAY);
	blur(src_gray, src_gray, cv::Size(3, 3));
	threshold(src_gray, threshold_output, thresh, 255, cv::THRESH_BINARY);
	findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	std::vector<std::vector<cv::Point>> contours_poly(contours.size());
	for (int i = 0; i < contours.size(); i++)
		approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
	return contours_poly;
}

int main() {
	std::cout << NUM_VERSION << '\n';
	int thresh = 240;
	std::vector<std::vector<cv::Point>> vertex = getVertex(cv::imread("../TestData/shapes.jpg"), thresh);
	for (const auto i : vertex) {
		for (const auto j : i)
			std::cout << j << ' ';
		std::cout << "\n\n";
	}
	cv::waitKey(0);
	/*std::vector<Color> color = getTrainedColorVector();
	Color match = getColor(color, cv::imread("../TestData/yellow.jpg", cv::IMREAD_COLOR));
	std::cout << match.colorName;*/
	while (1);
}