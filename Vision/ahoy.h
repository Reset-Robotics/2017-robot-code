//Reset Robotics 2017
//NVIDIA Jetson Vision Tracking Header

//Libraries
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <math.h>
#include <stdint.h>
#include <string>
#include <vector>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d.hpp"



//Variables
using namespace cv;
using namespace std;

Mat detected_edges;
Mat distance_output;
Mat imgHSV;
int TOLERANCE = 10;
int IMG_WIDTH = 640;
int IMG_HEIGHT = 480;
int targetHeight4FeetFromHighGoal = 39;
int targetHeight4FeetFromGearPeg = 121;
double cameraWidth2Feet = 29.9375;
double pi = 3.1415926535898;
double radiansPerSec = 7.0; //Just a guess. The number of radians
//the robot can turn a second by doing
//drivetrain->set(1.0,-1.0) or vice versa
int bestDistFromHighGoal = 36; //this value may be off
double robotSpeed = 120; //inches per second, may be off

bool isAiming = false;
bool isGearing = false;

double distanceTrackingOutput;
//4 feet = height 121 for gear peg
//4 feet = height 39 for high goal 4 inch high tape

//24 in = 29.9375 in width


// constexpr int RANGE_FINDER_MODULE = 1;



//ahoy methods
#ifndef _A_HOY
#define _A_HOY

double distFromHighGoal() { //Find distance from the center of the boiler, in inches.
  cvtColor(distance_output, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
  distance_output = imread(detected_edges,CV_LOAD_IMAGE_GRAYSCALE);
  int maxArea = 0;
  double bestHeight = 0.0;
  std::vector<std::vector<cv::Point> > contours = detected_edges;
  for(int c=0;c<contours.size();c++) {
    if(contourArea(contours[c])>maxArea) {
      maxArea=contourArea(contours[c]);
      bestHeight = boundingRect(contours[c]).height;
    }
  }
  distanceTrackingOutput = 48.0*bestHeight/targetHeight4FeetFromHighGoal;
  //Pixel height and distance are inversely proportional
  return distanceTrackingOutput;
  //cout << distanceTrackingOutput << endl;
}

 double distFromGearPeg() { //Find distance from the gear peg, in inches.
  int maxArea = 0;
  double bestHeight = 0.0;
  std::vector<std::vector<cv::Point> > contours = detected_edges;
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

 std::vector<cv::Point> contourCenters(std::vector<std::vector<cv::Point> > contours) {
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
  std::vector<cv::Point> centers = contourCenters(detected_edges);
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

#endif
