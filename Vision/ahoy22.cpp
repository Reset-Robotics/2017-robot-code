//Reset Robotics 2017
//NVIDIA Jetson Vision Tracking Testing

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
#include "RobotConstants.h"
#include "VisionMethods.h"


//Variables
using namespace cv;
using namespace std;

//Materials
Mat imgOriginal;
Mat imgHSV;
Mat imgThresholded;

//Camera id
int activeCamera = 0;

//Hue Values
double iLowH = 133.89830508474574;
double iHighH = 180.0;
//Saturation Values
double iLowS = 0;
double iHighS = 60.58510638297872;
//Value Values
double iLowV = 230.50847457627117;
double iHighV = 255;

double hsvThresholdHue[] = {133.89830508474574, 180.0};
double hsvThresholdSaturation[] = {0.0, 60.58510638297872};
double hsvThresholdValue[] = {230.50847457627117, 255.0};


//Main
int main(int argc, char** argv)
{
  namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

  //Create trackbars in "Control" window
  cvCreateTrackbar("LowH ", "Control", &iLowH, 255); //Hue minimum (0 - 255)
  //popen("zenity  --title  'Input the Low Hue Value' --entry --text 'enter an integer between 0 and 255'", "-r"); // use this instead of lame trackbars
  //FILE *lowHue = popen("zenity --entry", "r"); // find a way to convert this to the iLowH value
  cvCreateTrackbar("HighH", "Control", &iHighH, 255); //Hue maximum (0 - 255)
  //FILE *highHue = popen("zenity --entry", "r");
  cvCreateTrackbar("LowS ", "Control", &iLowS, 255); //Saturation minimum (0 - 255)
  cvCreateTrackbar("HighS", "Control", &iHighS, 255); //Saturation maximum (0 - 255)
  cvCreateTrackbar("LowV ", "Control", &iLowV, 255); //Value minimum (0 - 255)
  cvCreateTrackbar("HighV", "Control", &iHighV, 255); //Value maximum (0 - 255)

  cvCreateTrackbar("Active Camera ", "Control", &activeCamera, 4); //Set active camera id (0 - 4); 1 is not assigned to a camera

  //VideoCapture cap(0);

  while (true)
  {
       bool bSuccess = cap.read(imgOriginal); // read a new frame from video

       if (!bSuccess) //if not success, break loop
       {
            cout << "Cannot read a frame from video stream" << endl;
            break;
       }

       cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

       inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

       //morphological opening (remove small objects from the foreground)
       erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
       dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

       //morphological closing (fill small holes in the foreground)
       dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
       erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));



       imshow("Thresholded Image", imgThresholded); //show the thresholded image
       imshow("Original", imgOriginal); //show the original image
    }
  }
