//opencv imports
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/gpu/gpu.hpp"

//standard package imports
#include <iostream>
#include <time.h>
#include <cstdio>
#include <cmath>
#include <cstdlib>

int main() {
	int blur_size = 3; //size of the median blur kernel
	int img_scale_factor = 2; //halves the size of the picture

	//position tracking values (piecewise function)
	double angle_approx[6] = {-18.3069, 113.2350, -253.0048, 228.7990, 4.0635, 15.4875}; //ar^5 + br^4 + cr^3 + dr^2 + er + f (for 0.3 <= r <= 1.8)
	double angle_approx_low[2] = {105.2632, 0.0}; //ar + b (for r < 0.3)
	double angle_approx_high[2] = {3.9761, 127.1074}; //ar + b (for r > 1.8)

	//average of past three frame's values for less fluctuation on angle
	double area_ratio_vals[3] = {1.0, 1.0, 1.0};

	//distance tracking values
	double dist_approx[4] = {4.7720, 2.7183, -0.0381, 0.3698}; //a(b^cr) + d

	//HSV Thresholding Defaults (green LED ring and Exposure of 5 on camera)
	int h_lowerb = 20;
	int h_upperb = 90;
	int s_lowerb = 130;
	int s_upperb = 255;
	int v_lowerb = 25;
	int v_upperb = 130;

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
	for (;;) {
		//reads the image and checks if we dont have video feed
		if (!input.read(imgRaw)) {
			break;
		}

	//gpu accel
		//upload regular Mat to GPU as a GpuMat
		src.upload(imgRaw);

		//gpu accelerated image transformations
		cv::gpu::resize(src, resize, cv::Size(input.get(CV_CAP_PROP_FRAME_WIDTH) / img_scale_factor,
			input.get(CV_CAP_PROP_FRAME_HEIGHT) / img_scale_factor), CV_INTER_CUBIC);
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

		//if we identify targets print all this
		if (!targets.empty()) {
			std::vector<cv::RotatedRect> r(targets[0]); // try regular parentheses
			std::vector<cv::RotatedRect> r1(targets[1]);
			centerX= r.x + (r.width/2);
			centerX1= r1.x + (r1.width/2);
			// cout this

		}

		//save and show the output image
		output.write(imgOutput);
		cv::imshow("stream", imgThreshold);

		//if spacebar is pressed, quit
		char c = cv::waitKey(1);
		if (c == ' ') {
			printf("Stream has ended.");
			break;
		}
	}
}
