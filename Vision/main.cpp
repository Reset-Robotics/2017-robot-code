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
//socket imports
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

int newsockfd;
int sockfd;

struct timeval serverStartTime, nowTime;
long int serverStartTimeMs = 0;

//vision state:
// -1, off (ends program)
//  0, default state, server on, vision not on
//  1, gear vision
//  2, boiler vision
std::atomic<int> visionState(1);

//prints an error to the screen and exits the program
void error(const char *msg)
{
	perror(msg);
	exit(1);
}

//a loop that gets incoming messages from the RoboRio
void *dataRecvThread(void *threadId) {
	char buffer[256];
	while (1) {
		bzero(buffer, 256);
		int i = 0;
		do {
			int n = 0;
			n = read(newsockfd, &buffer[i], 255);
			if (n < 0) {
				error("Socket Error inside Data Recv Routine\n");
			}
			else {
				i += n;
			}
		} while ((int)buffer[i - 1] != 10); //10 is a newline character
		dataRoutine(buffer);
		if (buffer[0] == 'C') break; // exit loop if message is to close
	}
	pthread_exit(NULL);
}

//start the threaded receive loop
void startRecv(pthread_t *threads) {
	int a = pthread_create(&threads[0], NULL, dataRecvThread, NULL);
	if (a) {
		error("Data Recv Thread Creation Failed\n");
	}
}

//send data to the RoboRio
void sendData(double distance) {
	int n = 0;
	long int nowTimeMs = 0;

	nowTimeMs = nowTime.tv_sec * 1000 + nowTime.tv_usec / 1000;

	std::stringstream timeStream;
	timeStream << std::fixed << std::setprecision(0) << nowTimeMs - serverStartTimeMs;

	std::stringstream distanceStringStream;
	distanceStringStream << std::fixed << std::setprecision(3) << distance;

	std::string stringToSend = distanceStringStream.str() + ','  + timeStream.str() + '\n';
	printf("Sending %s", stringToSend.c_str());
	n = write(newsockfd, stringToSend.c_str(), stringToSend.length());
	if (n < 0) {
		printf("Error sending message.\n");
	}
}

//start the server (open a socket and wait for connection from the RoboRio)
void startServer() { //make own connect method. thread maybe?
					 //jetson has a static ip on it's ethernet port of "10.9.72.2"
	int port = 9720;
	printf("Starting Socket Server on port %d.\n", port);

	int portNum; //port that we're opening a socket on
	socklen_t cliAddrLen; //length of the client's address
	char buffer[256]; //buffer for incoming bytes TODO make smaller
	struct sockaddr_in serv_addr, cli_addr; //makes a socket address for the server and client
	int n;

	portNum = port;
	//if the port value isn't a valid port, throw error
	if (portNum < 2000 || portNum > 65535) {
		fprintf(stderr, "ERROR, port provided is invalid\n");
		exit(1);
	}

	//attempt to open a socket (domain, socket_type (stream or datagram), protocol (0 is auto-choose (TCP for us)))
	printf("Opening Socket...\n");
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		error("ERROR opening socket");
	}

	//zero out all values in buffer
	bzero((char *)&serv_addr, sizeof(serv_addr));

	//save the port number for the arguments
	//portNum = atoi(argv[1]);

	//create our server address (domain, ip, and port)
	serv_addr.sin_family = AF_INET; //domain must be AF_INET
	serv_addr.sin_addr.s_addr = INADDR_ANY; //this is the machines ip TODO make eth3 ip (192.168.1.150) works like this though
	serv_addr.sin_port = htons(portNum); //we chose the port

										 //bind the socket we opened to our address
	printf("Binding Socket...\n");
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		error("ERROR on binding");
	}

	//listen on our open socket with a max backlog queue of 5
	printf("Waiting for connection...\n");
	listen(sockfd, 5); //TODO change to 1 or 2

					   //wait until a client has connected and save the new socket that it's connected on
	cliAddrLen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &cliAddrLen);
	printf("Client connected!\n");
	if (newsockfd < 0) {
		error("ERROR on accept");
	}

	//connection handshake (check that we can receive and send data)
	bzero(buffer, 256);
	printf("Attempting to confirm connection...\n");

	//check for read
	int i = 0;
	do {
		n = read(newsockfd, &buffer[i], 255);
		if (n < 0) {
			//if we did not receive any bytes
			error("Error confirming connection with RoboRio. (Did not receive a confirmation byte.)");
		}
		else {
			i += n;
		}
	} while ((int)buffer[i - 1] != 10);

	//print what we got
	printf("Confirmation received: %s", buffer);

	gettimeofday(&serverStartTime, NULL);
	serverStartTimeMs = serverStartTime.tv_sec * 1000 + serverStartTime.tv_usec/1000;

	//check for write
	n = write(newsockfd, buffer, 255); //TODO only send length of buffer
	if (n < 0) {
		//if we cannot send what we received back
		error("Error confirming connectio RoboRio. (Could not send a confirmation byte.)");
	}

	//if everything works smoothly, we get to here
	printf("Connection with RoboRio confirmed. Starting Connection Loop.\n");
	bzero(buffer, 256);
}
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
    int img_scale_factor = 1; //halves the size of the picture

   //NetworkTable::SetClientMode();
//    NetworkTable::SetIPAddress("10.63.25.1");
 //  NetworkTable visionTable;
 //  visionTable = NetworkTable::GetTable("vision");
    //distance tracking values
    double dist_approx[4] = {4.7720, 2.7183, -0.0381, 0.3698}; //a(b^cr) + d

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

        //save and show the output image
        output.write(imgOutput);
	cv::namedWindow("stream", CV_WINDOW_AUTOSIZE);
	//cv::namedWindow("raw",);
        cv::imshow("stream", imgThreshold);
	//cv::imshow("raw", imgRaw);
        //cv::waitKey(0);
        //if spacebar is pressed, quit
        char c = cv::waitKey(1);
        if (c == ' ') {
            printf("Stream has ended.");
            //break;
        }

        //if we identify targets print all this
        if (!targets.empty()) {
           cv::Rect r = boundingRect(targets[1]); // try regular parentheses
         cv::Rect  r1 = boundingRect(targets[0]);
           double centerX= r.x + (r.width/2);
          double centerX1= r1.x + (r1.width/2);
            double distance2Pixels= ((centerX + centerX1) / 2) - (640 / 2); // 640 is camera width
	std::cout << "Distance pix" << distance2Pixels << std::endl;
  sendData(distance2Pixels);  // cout this

        }
}
}
