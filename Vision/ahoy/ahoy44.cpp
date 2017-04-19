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



int main() {

	zmq::context_t context (1);
	zmq::socket_t publisher(context, ZMQ_PUB);
	publisher.bind("tcp://*:5801"); // Available ports are 1180-1190 and 5800 to 5810. Do not use 1181.

	std::cout << "starting server\n";

    printf("Current exposure settings:\n");
        system("v4l2-ctl -d /dev/video0 -C exposure_auto");
        system("v4l2-ctl -d /dev/video0 -C exposure_absolute");
	system("v4l2-ctl -d /dev/video1 -C exposure_auto");
        system("v4l2-ctl -d /dev/video1 -C exposure_absolute");

        //set exposure to 5
        system("v4l2-ctl -d /dev/video0 -c exposure_auto=1");
        system("v4l2-ctl -d /dev/video0 -c exposure_absolute=5");
	system("v4l2-ctl -d /dev/video1 -c exposure_auto=1");
        system("v4l2-ctl -d /dev/video1 -c exposure_absolute=5");

        //print the exposures of the cameras
        printf("New exposure settings:\n");
        system("v4l2-ctl -d /dev/video0 -C exposure_absolute");
	system("v4l2-ctl -d /dev/video1 -C exposure_absolute");
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
    
    cv::Mat rightImgRaw; //input image
    cv::Mat rightImgResize;
    cv::Mat rightImgHSV; //switch to HSV colorspace
    cv::Mat rightImgThreshold; //binary image from HSV thresholding
    cv::Mat rightImgContour; //finding countours
    cv::Mat rightImgOutput; //final image

    cv::gpu::GpuMat src, resize, blur, hsv, threshold;
    cv::gpu::GpuMat rightSrc, rightResize, rightHsv, rightThreshold;


    //HSV Threshold Sliders (currently commented out cause unnecessary)
    cv::namedWindow("HSV Thresholding");
    cv::createTrackbar("Hue Lower Bound", "HSV Thresholding", &h_lowerb, 179);
    cv::createTrackbar("Hue Upper Bound", "HSV Thresholding", &h_upperb, 179);
    cv::createTrackbar("Saturation Lower Bound", "HSV Thresholding", &s_lowerb, 255);
    cv::createTrackbar("Saturation Upper Bound", "HSV Thresholding", &s_upperb, 255);
    cv::createTrackbar("Value Lower Bound", "HSV Thresholding", &v_lowerb, 255);
    cv::createTrackbar("Value Upper Bound", "HSV Thresholding", &v_upperb, 255);

    std::vector<std::vector<cv::Point> > contours; //array of contours (which are each an array of points)
    std::vector<std::vector<cv::Point> > rightContours; 



    //start the video
    cv::VideoCapture leftCamera(0);
    cv::VideoCapture rightCamera(1);
    cv::VideoWriter output;

    double left = 0;
    double right = 0;

    //loop for each frame
    for (;;) {
        //reads the image and checks if we dont have video feed
        if (!leftCamera.read(imgRaw)) {
            break;
        }
        
        if (!rightCamera.read(rightImgRaw)) {
            break;
        }

        //gpu accel
        //upload regular Mat to GPU as a GpuMat
        src.upload(imgRaw);
        rightSrc.upload(rightImgRaw);

        //gpu accelerated image transformations
        cv::gpu::resize(src, resize, cv::Size(leftCamera.get(CV_CAP_PROP_FRAME_WIDTH) / img_scale_factor,
                                              leftCamera.get(CV_CAP_PROP_FRAME_HEIGHT) / img_scale_factor), CV_INTER_CUBIC);
                                     
        cv::gpu::resize(rightSrc, rightResize, cv::Size(rightCamera.get(CV_CAP_PROP_FRAME_WIDTH) / img_scale_factor,
                                              rightCamera.get(CV_CAP_PROP_FRAME_HEIGHT) / img_scale_factor), CV_INTER_CUBIC);
        resize.download(imgResize);
        rightResize.download(rightImgResize);

        
        cv::gpu::cvtColor(resize, hsv, CV_BGR2HSV);
        cv::gpu::cvtColor(rightResize, rightHsv, CV_BGR2HSV);

        //download GpuMat from the GPU
        hsv.download(imgHSV);
        rightHsv.download(rightImgHSV);

        //end gpu accel

        
        cv::inRange(imgHSV, cv::Scalar(h_lowerb, s_lowerb, v_lowerb), cv::Scalar(h_upperb, s_upperb, v_upperb), imgThreshold);
        cv::inRange(rightImgHSV, cv::Scalar(h_lowerb, s_lowerb, v_lowerb), cv::Scalar(h_upperb, s_upperb, v_upperb), rightImgThreshold);
        imgThreshold.copyTo(imgContour);
        rightImgThreshold.copyTo(rightImgContour);
        cv::findContours(imgContour, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
        cv::findContours(rightImgContour, rightContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

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
	    
	    
	std::vector<cv::RotatedRect> boundRectsR(rightContours.size());
        for(int i = 0; i < rightContours.size(); i++) {
            boundRectsR[i] = cv::minAreaRect(cv::Mat(rightContours[i]));
        }

        //identify contours which could be actual targets
        std::vector<std::vector<cv::Point> > poss_targets_r(0);
        for(int i = 0; i < boundRectsR.size(); i++) {
            cv::Size sr = boundRectsR[i].size;

            //filter based on how much of the bounding rectangle is filled up by the contour and rectangles that are too small
            if (sr.height * sr.width < 1.25 * cv::contourArea(rightContours[i]) && sr.height * sr.width > 30) {
                poss_targets_r.insert(poss_targets_r.end(), rightContours[i]);
            }
        }

        //if we identify two possible targets then they are correct targets
        std::vector<std::vector<cv::Point> > targetsR(0);
        if (poss_targets_r.size() == 2) {
            targets = poss_targets;
        }

        imgOutput = imgResize.clone(); //output image is the blurred image with information on it
	rightImgOutput = rightImgResize.clone();

        cv::drawContours(imgOutput, contours, -1, cv::Scalar(0, 255, 0)); //draw all contours in green

          rightImgOutput = rightImgResize.clone();
            
          cv::drawContours(rightImgOutput, rightContours, -1, cv::Scalar(0, 255, 0)); //draw all contours in green
            
            //if we identify all targets calculate centers
            if (!targets.empty()) {
                cv::Rect r = boundingRect(targets[1]);
                cv::Rect  r1 = boundingRect(targets[0]);
                double centerX= r.x + (r.width/2);
                double centerX1= r1.x + (r1.width/2);
                left= ((centerX + centerX1) / 2);
                std::cout << "Left pix" << left << std::endl;
		//zmq::message_t message(20);
            	//snprintf ((char *) message.data(), 20, "displacement left %f", leftPixels);
            	//publisher.send(message);
                // Send message
       		 }
             else{
                left = 0; //if no target is detected, send 0
             }   
	    
	    if (!targetsR.empty()) {
                cv::Rect r = boundingRect(targetsR[1]);
                cv::Rect  r1 = boundingRect(targetsR[0]);
                double centerX= r.x + (r.width/2);
                double centerX1= r1.x + (r1.width/2);
                right= imageWidth - ((centerX + centerX1) / 2);
                std::cout << "Right pix" << right << std::endl;
		//zmq::message_t message(20);
            	//snprintf ((char *) message.data(), 20, "displacement left %f", rightPixels);
            	//publisher.send(message);
                // Send message
       		 }
             else{
                right = 0; //if no target is detected, send 0
             }

            zmq::message_t message(20);
            	snprintf ((char *) message.data(), 20, "displacement %f %f", left, right);
            	publisher.send(message);
            //send message
            
            //save and show the output image
            //rightOutput.write(rightImgOutput);
            cv::imshow("streamRight", rightImgThreshold);
	        cv::imgshow("streamRight", imgThreshold);
            
            //if spacebar is pressed, quit
            char c = cv::waitKey(1);
            if (c == ' ') {
                printf("Stream has ended.");
                break;
            }
            
        

    }
