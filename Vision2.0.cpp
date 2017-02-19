/*
 * VisionMethods.h
 */
#include "Commands/GripPipeline.h"
#include "RobotConstants.h" //If you don't know what some variables are, they were probably defined in here
#include <string>
#include <vector>
#ifndef SRC_VISIONMETHODS_H_
#define SRC_VISIONMETHODS_H_


std::vector<std::vector<cv::Point>> filteredContours(int cameranum) {
	cv::Mat frame;
	cv::VideoCapture cap(cameranum);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, IMG_WIDTH);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, IMG_HEIGHT);
	bool bSuccess = cap.read(frame);
	grip::GripPipeline gp;
	return gp.GripPipeline::process(frame);
}

double distFromHighGoal() { //Find distance from the center of the boiler, in inches.
	int maxArea = 0;
	double bestHeight = 0.0;
	std::vector<std::vector<cv::Point>> contours = filteredContours(cameraPortHigh);
	for(int c=0;c<contours.size();c++) {
		if(contourArea(contours[c])>maxArea) {
			maxArea=contourArea(contours[c]);
			bestHeight = boundingRect(contours[c]).height;
		}
	}
	//Pixel height and distance are inversely proportional
	return 48.0*bestHeight/targetHeight4FeetFromHighGoal;
}

double distFromGearPeg() { //Find distance from the gear peg, in inches.
	int maxArea = 0;
	double bestHeight = 0.0;
	std::vector<std::vector<cv::Point>> contours = filteredContours(cameraPortLow);
	for(int c=0;c<contours.size();c++) {
		if(contourArea(contours[c])>maxArea) {
			maxArea=contourArea(contours[c]);
			bestHeight = boundingRect(contours[c]).height;
		}
	}
	//Pixel height and distance are inversely proportional
	return 48.0*bestHeight/targetHeight4FeetFromGearPeg;
}

cv::Point centerOfContour(std::vector<cv::Point> contour) {
	int totalx=0.0; //given a contour, outputs its center
	int totaly=0.0;
	for(int d=0; d<contour.size();d++) {
		totalx+=contour[d].x;
		totaly+=contour[d].y;
	}
	cv::Point pt;
	pt.x=totalx/contour.size();
	pt.y=totaly/contour.size();
	return pt;
}

std::vector<cv::Point> contourCenters(std::vector<std::vector<cv::Point>> contours) {
	std::vector<cv::Point> centers; //given a vector of contours, outputs a vector consisting of their centers
	double totalx;
	double totaly;
	for(int c=0;c<contours.size();c++) {
		centers.push_back(centerOfContour(contours[c]));
	}
	return centers;
}

std::string initialOrientation() {
	int score=0; //negative for left, positive for right
	std::vector<cv::Point> centers = contourCenters(filteredContours(cameraPortLow));
	for(int c=0;c<centers.size();c++) {
		if(centers[c].x>IMG_WIDTH/2+TOLERANCE) {
			score++;
		}
		else if(centers[c].x<IMG_WIDTH/2-TOLERANCE) {
			score--;
		}
	}
	if(score==0) {return "middle";}
	else if(score<0) {return "right";}
	else {return "left";}
}
