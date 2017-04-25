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

//Mat detected_edges;
std::vector<std::vector<cv::Point>> contours;
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
