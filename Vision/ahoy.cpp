//Reset Robotics 2017
//NVIDIA Jetson Vision Tracking


//Libraries
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <math.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d.hpp"
#include "ahoy.h"

//Variables
using namespace cv;
using namespace std;
//Materials
Mat dst;
Mat imgOriginal;
Mat imgThresholded;
//threshholds
int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 100;
int kernel_size = 3;
int activeCamera = 0;
int currentCamera = 0;
/*//default hsv vars
int iLowH = 0;
int iHighH = 255;
int iLowS = 0;
int iHighS = 255;
int iLowV = 0;
int iHighV = 255;*/
//light greenhsv
int iLowH = 53;
int iHighH = 90;

int iLowS = 74;
int iHighS = 147;

int iLowV = 160;
int iHighV = 255;



//Main

//CannyThreshold Functions with ration of 1:3
void CannyThreshold(int, void*)
{
  cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

  inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

  //morphological opening (remove small objects from the foreground)
  erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
  dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

  //morphological closing (fill small holes in the foreground)
  dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
  erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

  /// Reduce noise with a kernel 3x3
  blur(imgThresholded, detected_edges, Size(3,3));

  /// Canny detector
  Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*3, 3);

  /// Using Canny's output as a mask, we display our result
  dst = Scalar::all(0);

  imgThresholded.copyTo(dst, detected_edges);
  imshow("Edge Map", detected_edges);
}

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

  int lastCamera = activeCamera;
  cout << lastCamera << endl;

  /// Create a Trackbar for user to enter threshold
  createTrackbar("Min Threshold:", "Control", &lowThreshold, max_lowThreshold, CannyThreshold); //Canny Threshold adjuster
  cvCreateTrackbar("Active Camera ", "Control", &activeCamera, 4); //Set active camera id (0 - 4); 1 is not assigned to a camera

  VideoCapture cap(activeCamera);

  if (!cap.isOpened())  // if not success, exit program
  {
     cout << "Cannot open the web cam" << endl;
     return -1;
  }

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
       /// Show the canny image
       CannyThreshold(0,0);

       if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
       {
           cout << "esc key is pressed by user" << endl;
           break;
       }
   }
  return 0;
}
