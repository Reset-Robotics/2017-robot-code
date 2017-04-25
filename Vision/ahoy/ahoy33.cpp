//Reset Robotics 2017
//NVIDIA Jetson Vision Tracking

//Libraries
// OpenCV and GPU Acceleration imports
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/gpu/gpu.hpp"
//standard imports
#include <iostream>
#include <time.h>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <zmq.hpp>



//Main
int main() {
    // setting up socket for ZMQ
    zmq::context_t context (1);
    zmq::socket_t publisher(context, ZMQ_PUB);
    publisher.bind("tcp://*:5801"); // Available ports are 1180-1190 and 5800 to 5810.

// 2nd data will be sent on port 5802

    std::cout << "starting server\n";

    int img_scale_factor = 1; //change to 2 to half the size of the picture (320 x 240)



    //HSV Thresholding Defaults (Purple LED ring and Exposure of 5 on camera)
    int h_lowerb = 97;
    int h_upperb = 179;
    int s_lowerb = 185;
    int s_upperb = 255;
    int v_lowerb = 52;
    int v_upperb = 255;

    //images
    cv::Mat imgRaw; //input image
    cv::Mat imgResize; //image resized to 320x240 if scale factor 2 (kept at 640 x 480 if img scale factor = 1)
    cv::Mat imgHSV; //switch to HSV colorspace
    cv::Mat imgThreshold; //binary image from HSV thresholding
    cv::Mat imgContour; //finding countours
    cv::Mat imgOutput; //final image

    cv::Mat rightImgRaw; //input image
    cv::Mat rightImgResize;
    cv::Mat rightImgHSV; //switch to HSV colorspace
    cv::Mat rightImgThreshold; //binary image from HSV thresholding
    cv::Mat rightImgContour; //finding countours
    cv::Mat rightImgOutput; //final image


    cv::gpu::GpuMat src, resize, hsv, threshold;
    cv::gpu::GpuMat rightSrc, rightResize, rightHsv, rightThreshold;

    //HSV Threshold Sliders
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
    cv::VideoWriter leftOutput;
    cv::VideoWriter rightOutput;

    //loop for each frame
    for (;;) {
        //reads the image and checks if we dont have video feed
        if (!leftCamera.read(imgRaw)) {
            break;
        }
        //gpu acceleration
        //upload Mat to GPU as a GpuMat
        src.upload(imgRaw);

        //resize the image to 320 x 240 if img scale factor is 2. If scale is 1 it will be 640 x 480
        cv::gpu::resize(src, resize, cv::Size(leftCamera.get(CV_CAP_PROP_FRAME_WIDTH) / img_scale_factor,
                                              leftCamera.get(CV_CAP_PROP_FRAME_HEIGHT) / img_scale_factor), CV_INTER_CUBIC);
        // convert to HSV Colorspace
        cv::gpu::cvtColor(resize, hsv, CV_BGR2HSV);

        //download GpuMat from the GPU
        hsv.download(imgHSV);

        //threshold image
        cv::inRange(imgHSV, cv::Scalar(h_lowerb, s_lowerb, v_lowerb), cv::Scalar(h_upperb, s_upperb, v_upperb), imgThreshold);
        imgThreshold.copyTo(imgContour);

        //find contours
        cv::findContours(imgContour, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

        //create bounding rectangles to identify what contours are actual targets
        std::vector<cv::RotatedRect> boundRects(contours.size());
        for(int i = 0; i < contours.size(); i++) {
            boundRects[i] = cv::minAreaRect(cv::Mat(contours[i]));
        }

        //identify which contours could be actual targets
        std::vector<std::vector<cv::Point> > poss_targets(0);
        for(int i = 0; i < boundRects.size(); i++) {
            cv::Size s = boundRects[i].size;

            //filter out images
            if (s.height * s.width < 1.25 * cv::contourArea(contours[i]) && s.height * s.width > 30) {
                poss_targets.insert(poss_targets.end(), contours[i]);
            }
        }

        //if we identify two possible targets they are the right targets (hopefully xD)
        std::vector<std::vector<cv::Point> > targets(0);
        if (poss_targets.size() == 2) {
            targets = poss_targets;
        }


        imgOutput = imgResize.clone();

        cv::drawContours(imgOutput, contours, -1, cv::Scalar(0, 255, 0)); //draw all contours in green

        //if we identify all targets calculate centers
        if (!targets.empty()) {
            cv::Rect  r = boundingRect(targets[0]);
            double centerX= r.x + (r.width/2);
            double centerX1= r1.x + (r1.width/2);
            double leftPixels= ((centerX) / 2) - (imageWidth / 2);
            std::cout << "Left pix" << leftPixels << std::endl;
            // Send message on port using ZMQ
            zmq::message_t message(20);
            snprintf ((char *) message.data(), 20, "displacement left %f", leftPixels);
            publisher.send(message);
        }

        //save and show the output image
        leftOutput.write(imgOutput);
        cv::imshow("stream", imgThreshold);

        //if spacebar is pressed, quit
        char c = cv::waitKey(1);
        if (c == ' ') {
            printf("Stream has ended.");
            break;
        }

        //loop for each frame
        for (;;) {
            //reads the image and checks if we dont have video feed
            if (!rightCamera.read(rightImgRaw)) {
                break;
            }
            //gpu acceleration
            //upload Mat to GPU as a GpuMat
            src.upload(rightImgRaw);

            //resize the image to 320 x 240 if img scale factor is 2. If scale is 1 it will be 640 x 480
            cv::gpu::resize(rightSrc, rightResize, cv::Size(rightCamera.get(CV_CAP_PROP_FRAME_WIDTH) / img_scale_factor,rightCamera.get(CV_CAP_PROP_FRAME_HEIGHT) / img_scale_factor), CV_INTER_CUBIC);
            // convert to HSV Colorspace
            cv::gpu::cvtColor(rightResize, rightHsv, CV_BGR2HSV);

            //download GpuMat from the GPU
            hsv.download(rightImgHSV);

            //threshold image
            cv::inRange(rightImgHSV, cv::Scalar(h_lowerb, s_lowerb, v_lowerb), cv::Scalar(h_upperb, s_upperb, v_upperb), rightImgThreshold);
            rightImgThreshold.copyTo(rightImgContour);

            //find contours
            cv::findContours(rightImgContour, rightContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

            //create bounding rectangles to identify what contours are actual targets
            std::vector<cv::RotatedRect> boundRects(rightContours.size());
            for(int i = 0; i < rightContours.size(); i++) {
                boundRects[i] = cv::minAreaRect(cv::Mat(rightContours[i]));
            }

            //identify which contours could be actual targets
            std::vector<std::vector<cv::Point> > poss_targets(0);
            for(int i = 0; i < boundRects.size(); i++) {
                cv::Size s = boundRects[i].size;

                //filter out images
                if (s.height * s.width < 1.25 * cv::contourArea(rightContours[i]) && s.height * s.width > 30) {
                    poss_targets.insert(poss_targets.end(), rightContours[i]);
                }
            }

            //if we identify two possible targets they are the right targets (hopefully xD)
            std::vector<std::vector<cv::Point> > targets(0);
            if (poss_targets.size() == 2) {
                targets = poss_targets;
            }


            rightImgOutput = rightImgResize.clone();

            cv::drawContours(rightImgOutput, rightContours, -1, cv::Scalar(0, 255, 0)); //draw all contours in green

            //if we identify all targets calculate centers
            if (!targets.empty()) {
                cv::Rect r = boundingRect(targets[1]);
                cv::Rect  r1 = boundingRect(targets[0]);
                double centerX= r.x + (r.width/2);
                double centerX1= r1.x + (r1.width/2);
                double rightPixels= ((centerX + centerX1) / 2) - (imageWidth / 2);
                std::cout << "Right pix" << rightPixels << std::endl;
                // Send message



            }

            //save and show the output image
            rightOutput.write(rightImgOutput);
            cv::imshow("streamRight", rightImgThreshold);

            //if spacebar is pressed, quit
            char c = cv::waitKey(1);
            if (c == ' ') {
                printf("Stream has ended.");
                break;
            }



    }
