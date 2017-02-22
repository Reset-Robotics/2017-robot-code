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
#include "ahoy.h"

//Variables
using namespace cv;
using namespace std;

//Materials
Mat imgOriginal;
Mat imgThresholded;
Mat dst;
//threshholds
int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 100;
int kernel_size = 3;
//Camera id
int activeCamera = 0;

//Hue Values
int iLowH = 17;
int iHighH = 137;
//Saturation Values
int iLowS = 0;
int iHighS = 113;
//Value Values
int iLowV = 222;
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

  cvCreateTrackbar("Active Camera ", "Control", &activeCamera, 4); //Set active camera id (0 - 4); 1 is not assigned to a camera

  VideoCapture cap(0);
  cap.set(CV_CAP_PROP_FRAME_WIDTH, IMG_WIDTH);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT, IMG_HEIGHT);

  distFromHighGoal();
  /* This is java. 
   public static double returnCenterX(){
		double[] defaultValue = new double[0];
			// This is the center value returned by GRIP thank WPI
			if(!tracker.filterContoursOutput.isEmpty() && tracker.filterContoursOutput.size() >= 2){ // checks if there are contours and there are at least 2
				Rect r = Imgproc.boundingRect(tracker.filterContoursOutput.get(1)); // change this to contour map
				Rect r1 = Imgproc.boundingRect(tracker.filterContoursOutput.get(0)); // contour map as well
        height = boundingRect(tracker.filterContoursOutput.get(0)).height;
				centerX = new double[]{r1.x + (r1.width / 2), r.x + (r.width / 2)};
				Imgcodecs.imwrite("output.png", matOriginal);
				//System.out.println(centerX.length); //testing
				// this again checks for the 2 shapes on the target
				if(centerX.length == 2){
					// subtracts one another to get length in pixels
					lengthBetweenContours = Math.abs(centerX[0] - centerX[1]);
				}
			}
		return lengthBetweenContours;
		
	} */
  /* This is C++
  double distFromGearPeg() { //Find distance from the gear peg, in inches.
	int maxArea = 0;
	double bestHeight = 0.0;
	std::vector<std::vector<cv::Point>> contours = filteredContours(cameraPortLow);
	for(int c=0;c<contours.size();c++) {
		if(contourArea(contours[c])>maxArea) { // checks if there is contours
			maxArea=contourArea(contours[c]);
			bestHeight = boundingRect(contours[c]).height; // height is important part. // we could probably use this in above method.
		}
	}
	//Pixel height and distance are inversely proportional
	return 48.0*bestHeight/targetHeight4FeetFromGearPeg;
} */


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



       //output distFromHighGoal value.

       if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
       {
           cout << "esc key is pressed by user" << endl;
           break;
       }
    }
    return 0;
  }
