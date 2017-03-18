//standard imports
/*
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

//openCV imports
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/gpu/gpu.hpp"

//threading imports
#include <pthread.h>

//sockets that we're connected to
int newsockfd;
int sockfd;

struct timeval serverStartTime, nowTime;
long int serverStartTimeMs = 0;

//vision state:
// -1, off (ends program)
//  0, default state, server on, vision not on
//  1, gear vision
//  2, boiler vision
std::atomic<int> visionState(0);

//prints an error to the screen and exits the program
void error(const char *msg)
{
	perror(msg);
	exit(1);
}

//parse data coming from the RoboRio
void dataRoutine(char *buffer) {
	printf("We received %c.\n", buffer[0]);
	if (buffer[0] == 'C') {
		//close socket
		printf("Closing sockets...\n");
		close(newsockfd);
		close(sockfd);
		visionState = -1;
	}
	if (buffer[0] == 'B') {
		//start boiler vision
		printf("Start boiler vision.\n");
		visionState = 2;
	}
	if (buffer[0] == 'G') {
		//start gear vision
		printf("Start gear vision.\n");
		visionState = 1;
	}
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

	std::stringstream angleStringStream;
	angleStringStream << std::fixed << std::setprecision(3) << angle;

	std::string stringToSend = distanceStringStream.str() + ',' + angleStringStream.str() + ':' + timeStream.str() + '\n';
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
	printf("Waiting for conneca
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

// In the targets if statement
		//send to RoboRio
		sendData(dist_d);

		//print distance and angle on output image
		char angle_c[0];
		char dist_c[0];
		sprintf(angle_c, "%d", (int)angle_d);
		sprintf(dist_c, "%.2f", dist_d);
		std::string angle = angle_c;
		std::string dist = dist_c;
		std::string disp_angle = "Angle = " + angle + " degrees";
		std::string disp_dist = "Distance = " + dist + " meters";
		cv::putText(imgOutput, disp_angle, cv::Point(2, 10), CV_FONT_HERSHEY_PLAIN, 0.8, cv::Scalar(255, 255, 255));
		cv::putText(imgOutput, disp_dist, cv::Point(2, 22), CV_FONT_HERSHEY_PLAIN, 0.8, cv::Scalar(255, 255, 255));
	}

	//save and show the output image
	return imgOutput;
}



int main() {
	//start server and reading loop
	pthread_t threads[1];
	startServer();
	startRecv(threads);

        //mounts the drive
        system("echo 'ubuntu' | sudo -S mount /dev/sda1");

        printf("Initial exposure settings for GearVision input:\n");
        system("v4l2-ctl -d /dev/video0 -C exposure_auto");
        system("v4l2-ctl -d /dev/video0 -C exposure_absolute");
        printf("Initial exposure settings for BoilerVision input:\n");
        system("v4l2-ctl -d /dev/video1 -C exposure_auto");
        system("v4l2-ctl -d /dev/video1 -C exposure_absolute");

        //sets exposrure to 5
        system("v4l2-ctl -d /dev/video0 -c exposure_auto=1");
        system("v4l2-ctl -d /dev/video1 -c exposure_auto=1");
        system("v4l2-ctl -d /dev/video0 -c exposure_absolute=5");
        system("v4l2-ctl -d /dev/video1 -c exposure_absolute=5");

        //print the exposures of the cameras
        printf("New exposure settings:\n");
        system("v4l2-ctl -d /dev/video0 -C exposure_absolute");
        system("v4l2-ctl -d /dev/video1 -C exposure_absolute");


	time_t start = time(0);
	char time_stamp[32];
	strftime(time_stamp, sizeof(time_stamp), "%m-%d-%Y_%T", localtime(&start));
	std::string formatted_time = time_stamp;
	std::string videoname = "test_" + formatted_time + ".avi";

	
}

*/

