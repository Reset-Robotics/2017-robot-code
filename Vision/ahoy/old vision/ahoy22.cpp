//Reset Robotics 2017
//NVIDIA Jetson Vision Tracking

//Libraries
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/gpu/gpu.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <atomic>
#include <unistd.h>
#include <zmq.hpp>


//Main
int main()
{

	zmq::context_t context (1);
	zmq::socket_t publisher(context, ZMQ_PUB);
	publisher.bind("tcp://*:5801"); // Available ports are 1180-1190 and 5800 to 5810. Do not use 1181.

	std::cout << "starting server\n";

  printf("Current exposure settings:\n");
  system("v4l2-ctl -d /dev/video0 -C exposure_auto");
  system("v4l2-ctl -d /dev/video0 -C exposure_absolute");

  //set exposure to 5
  system("v4l2-ctl -d /dev/video0 -c exposure_auto=1");
  system("v4l2-ctl -d /dev/video0 -c exposure_absolute=5");

  //print the exposures of the cameras
  printf("New exposure settings:\n");
  system("v4l2-ctl -d /dev/video0 -C exposure_absolute");
  int blur_size = 3; //size of the median blur kernel
  int img_scale_factor = 1; //halves the size of the picture


  //distance tracking values

  //HSV Thresholding Defaults (green LED ring and Exposure of 5 on camera)
  int h_lowerb = 97;
  int h_upperb = 179;
  int s_lowerb = 185;
  int s_upperb = 255;
  int v_lowerb = 52;
  int v_upperb = 255;

  //images
  cv::Mat imgRaw; //input image
  cv::Mat imgResize; //image resized to 320x240
  cv::Mat imgBlur; //median blur
  cv::Mat imgHSV; //switch to HSV colorspace
  cv::Mat imgThreshold; //binary image from HSV thresholding
  cv::Mat imgContour; //finding countours messes up the threshold image
  cv::Mat imgOutput; //final image

  cv::gpu::GpuMat src, resize, blur, hsv, threshold;


  //HSV Threshold Sliders (currently commented out cause unnecessary)
  cv::namedWindow("HSV Thresholding");
  cv::createTrackbar("Hue Lower Bound", "HSV Thresholding", &h_lowerb, 179);
  cv::createTrackbar("Hue Upper Bound", "HSV Thresholding", &h_upperb, 179);
  cv::createTrackbar("Saturation Lower Bound", "HSV Thresholding", &s_lowerb, 255);
  cv::createTrackbar("Saturation Upper Bound", "HSV Thresholding", &s_upperb, 255);
  cv::createTrackbar("Value Lower Bound", "HSV Thresholding", &v_lowerb, 255);
  cv::createTrackbar("Value Upper Bound", "HSV Thresholding", &v_upperb, 255);

  std::vector<std::vector<cv::Point> > contours; //array of contours (which are each an array of points)



  //start the video
  cv::VideoCapture input(0);
  cv::VideoWriter output;

  //loop for each frame
  for (;;)
	{
      //reads the image and checks if we dont have video feed
      if (!input.read(imgRaw))
			{
          break;
      }

      //gpu accel
      //upload regular Mat to GPU as a GpuMat
      src.upload(imgRaw);

      //gpu accelerated image transformations
      cv::gpu::resize(src, resize, cv::Size(input.get(CV_CAP_PROP_FRAME_WIDTH) / img_scale_factor, input.get(CV_CAP_PROP_FRAME_HEIGHT) / img_scale_factor), CV_INTER_CUBIC);
      resize.download(imgResize);

      //there is no gpu accelerated median blur :(
      //cv::medianBlur(resize, blur, blur_size);

      cv::gpu::cvtColor(resize, hsv, CV_BGR2HSV);

      //download GpuMat from the GPU
      hsv.download(imgHSV);

      //end gpu accel

      //I think inrange and contours can be accelerated, so we're not done here
      cv::inRange(imgHSV, cv::Scalar(h_lowerb, s_lowerb, v_lowerb), cv::Scalar(h_upperb, s_upperb, v_upperb), imgThreshold);
      imgThreshold.copyTo(imgContour);
      cv::findContours(imgContour, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

      //create bounding rectangles to identify what contours are actual targets
      std::vector<cv::RotatedRect> boundRects(contours.size());
      for(int i = 0; i < contours.size(); i++)
			{
          boundRects[i] = cv::minAreaRect(cv::Mat(contours[i]));
      }

      //identify contours which could be actual targets
      std::vector<std::vector<cv::Point> > poss_targets(0);
      for(int i = 0; i < boundRects.size(); i++)
			{
          cv::Size s = boundRects[i].size;

          //filter based on how much of the bounding rectangle is filled up by the contour and rectangles that are too small
          if (s.height * s.width < 1.25 * cv::contourArea(contours[i]) && s.height * s.width > 30)
					{
              poss_targets.insert(poss_targets.end(), contours[i]);
          }
      }

      //if we identify two possible targets then they are correct targets
      std::vector<std::vector<cv::Point> > targets(0);
      if (poss_targets.size() == 2)
			{
          targets = poss_targets;
      }

      imgOutput = imgResize.clone(); //output image is the blurred image with information on it

      cv::drawContours(imgOutput, contours, -1, cv::Scalar(0, 255, 0)); //draw all contours in green

      //save and show the output image
      output.write(imgOutput);
	    cv::namedWindow("stream", CV_WINDOW_AUTOSIZE);
	    cv::namedWindow("raw", CV_WINDOW_AUTOSIZE);
      cv::imshow("stream", imgThreshold);
	    cv::imshow("raw", imgRaw);
      //cv::waitKey(0);
      //if spacebar is pressed, quit
      char c = cv::waitKey(1);
      if (c == ' ')
			{
      	printf("Stream has ended.");
        //break;
      }

      //if we identify targets print all this
      if (!targets.empty())
			{
      	cv::Rect r = boundingRect(targets[1]); // try regular parentheses
      	cv::Rect  r1 = boundingRect(targets[0]);
      	double centerX= r.x + (r.width/2);
        double centerX1= r1.x + (r1.width/2);
        double distance2Pixels= ((centerX + centerX1) / 2) - (640 / 2); // 640 is camera width
				std::cout << "Distance pix" << distance2Pixels << std::endl;

				zmq::message_t message(20);

				snprintf ((char *) message.data(), 20, "displacement %f", distance2Pixels);
				publisher.send(message);
      }
		}
}
