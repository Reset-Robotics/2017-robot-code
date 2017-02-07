//Reset Robotics 2017
//NVIDIA Jetson Vision Tracking


//Libraries
#include <iostream>
#include <stdio.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

//Variables
using namespace cv;
using namespace std;

//Main
int main( int argc, char** argv )
{
   VideoCapture cap(0); //capture the video from web cam

   if ( !cap.isOpened() )  // if not success, exit program
   {
        cout << "Cannot open the web cam" << endl;
        return -1;
   }

   namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

int iLowH = 0;
int iHighH = 255;

int iLowS = 0;
int iHighS = 255;

int iLowV = 0;
int iHighV = 255;

//Create trackbars in "Control" window
cvCreateTrackbar("LowH", "Control", &iLowH, 255); //Hue (0 - 255)
cvCreateTrackbar("HighH", "Control", &iHighH, 255);

cvCreateTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
cvCreateTrackbar("HighS", "Control", &iHighS, 255);

cvCreateTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
cvCreateTrackbar("HighV", "Control", &iHighV, 255);

   while (true)
   {
       Mat imgOriginal;

       bool bSuccess = cap.read(imgOriginal); // read a new frame from video

        if (!bSuccess) //if not success, break loop
       {
            cout << "Cannot read a frame from video stream" << endl;
            break;
       }

Mat imgHSV;

cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

Mat imgThresholded;

inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

//morphological opening (remove small objects from the foreground)
erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

//morphological closing (fill small holes in the foreground)
dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

imshow("Thresholded Image", imgThresholded); //show the thresholded image
imshow("Original", imgOriginal); //show the original image

       if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
      {
           cout << "esc key is pressed by user" << endl;
           break;
      }
   }

  return 0;

}
