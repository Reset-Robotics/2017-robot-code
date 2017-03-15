//opencv imports
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/gpu/gpu.hpp"

//standard imports
#include <iostream>
#include <time.h>
#include <cstdio>
#include <cmath>
#include <cstdlib>

int main() {
	
	printf("Current exposure settings:\n");
        system("v4l2-ctl -d /dev/video2 -C exposure_auto");
        system("v4l2-ctl -d /dev/video2 -C exposure_absolute");

        //set exposure to 5
        system("v4l2-ctl -d /dev/video2 -c exposure_auto=1");
        system("v4l2-ctl -d /dev/video2 -c exposure_absolute=5");

        //print the exposures of the cameras
        printf("New exposure settings:\n");
        system("v4l2-ctl -d /dev/video2 -C exposure_absolute");
        

	int blur_size = 3; //size of the median blur kernel
	// int img_scale_factor = 2; //halves the size of the picture


	//HSV Thresholding Defaults (green LED ring and Exposure of 5 on camera)
	int h_low = 0;
	int h_up = 92;
	int s_low = 0;
	int s_up = 10;
	int v_low = 0;
	int v_up = 255;

	//images
	cv::Mat imgRaw; //input image
	cv::Mat imgResize; //image resized to 320x240
	cv::Mat imgBlur; //median blur
	cv::Mat imgHSV; //switch to HSV colorspace
	cv::Mat imgThreshold; //binary image from HSV thresholding
	cv::Mat imgContour; //finding countours messes up the threshold image
	cv::Mat imgOutput; //final image

	cv::gpu::GpuMat src, resize, blur, hsv, threshold;

	//HSV Threshold Sliders
	cv::namedWindow("HSV Thresholding");
	cv::createTrackbar("Hue Lower Bound", "HSV Thresholding", &h_low, 179);
	cv::createTrackbar("Hue Upper Bound", "HSV Thresholding", &h_up, 179);
	cv::createTrackbar("Saturation Lower Bound", "HSV Thresholding", &s_low, 255);
	cv::createTrackbar("Saturation Upper Bound", "HSV Thresholding", &s_up, 255);
	cv::createTrackbar("Value Lower Bound", "HSV Thresholding", &v_low, 255);
	cv::createTrackbar("Value Upper Bound", "HSV Thresholding", &v_up, 255);

	std::vector<std::vector<cv::Point> > contours; //array of contour points



	//start the video
	cv::VideoCapture input(2);
	cv::VideoWriter output;

	//loop for each frame
	for (;;) {
		//reads the image and checks if we dont have video feed
		if (!input.read(imgRaw)) {
			break;
		}

	//gpu accel
		//upload regular Mat to GPU as a GpuMat
		src.upload(imgRaw);

		//gpu accelerated image transformations
		/* cv::gpu::resize(src, resize, cv::Size(input.get(CV_CAP_PROP_FRAME_WIDTH) / img_scale_factor,
			input.get(CV_CAP_PROP_FRAME_HEIGHT) / img_scale_factor), CV_INTER_CUBIC);
		resize.download(imgResize); */
		//there is no gpu accelerated median blur :(
		//cv::medianBlur(resize, blur, blur_size);
		cv::gpu::cvtColor(src, hsv, CV_BGR2HSV);

		//download GpuMat from the GPU
		hsv.download(imgHSV);

	//end gpu accel

		//I think inrange and contours can be accelerated, so we're not done here
		cv::inRange(imgHSV, cv::Scalar(h_low, s_low, v_low), cv::Scalar(h_up, s_up, v_up), imgThreshold);
		imgThreshold.copyTo(imgContour);
		cv::findContours(imgContour, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		//create bounding rectangles to identify what contours are actual targets
		std::vector<cv::RotatedRect> boundRects(contours.size());
		for(int i = 0; i < contours.size(); i++) {
			boundRects[i] = cv::minAreaRect(cv::Mat(contours[i]));
		}

		//identify contours which could be actual targets
		std::vector<std::vector<cv::Point> > poss_targets(0);
		for(int i = 0; i < boundRects.size(); i++) {
			cv::Size s = boundRects[i].size;

			//filter based on how much of the bounding rectangle is filled up by the contour and rectangles that are too small
			if (s.height * s.width < 1.25 * cv::contourArea(contours[i]) && s.height * s.width > 30) {
				poss_targets.insert(poss_targets.end(), contours[i]);
			}
		}

		//if we identify two possible targets then they are correct targets
		std::vector<std::vector<cv::Point> > targets(0);
		if (poss_targets.size() == 2) {
			targets = poss_targets;
		}

		imgOutput = imgResize.clone(); //output image is the blurred image with information on it

		cv::drawContours(imgOutput, contours, -1, cv::Scalar(0, 255, 0)); //draw all contours in green

		//if we identify targets calculate centerX
		if (!targets.empty()) {
			cv::Rect r = boundingRect(targets[0])
			cv::Rect r1 = boundingRect(targets[1])
			centerX= r.x + (r.width/2); // calculate the centerX of each
			centerX1= r1.x + (r1.width/2);
			double distanceFromCenterPixels= ((centerX + centerX1) / 2) - (640 / 2); // 640 is camera width
			

		}

		//save and show the output image
		output.write(imgOutput);
		cv::imshow("Threshold image", imgThreshold);

		//if spacebar is pressed, quit
		char c = cv::waitKey(1);
		if (c == ' ') {
			printf("Stream has ended.");
			break;
		}
	}
}
