#define _CRT_SECURE_NO_WARNINGS // disable strcat and whatnot compiler errors

#include <stdlib.h>

// Linux only libraries
#ifdef __linux__
#include "i2c/I2C.h"
#include <unistd.h> // sleep
#elif _WIN32
// Windows only libraries
#include <windows.h> //sleep
#endif

/*
Cross platform solution!
A file locking is a class that has process lifetime. This means that if a process holding a file lock ends or crashes, 
the operating system will automatically unlock it. This feature is very useful in some situations where we want to assure 
automatic unlocking even when the process crashes and avoid leaving blocked resources in the system. A file lock is 
constructed using the name of the file as an argument:
*/
//#include <boost/interprocess/sync/file_lock.hpp>
#include <fstream>
// Cross platform checking if directory exists
#include <sys/types.h>
#include <sys/stat.h>
#include <string>

// https://github.com/easylogging/easyloggingpp/blob/master/README.md
#include "easylogging++.h" // logging library
#include <signal.h> // force failure

#include <stdio.h> // lumberjack->info
#include <iostream> // try catch
#include <exception> // try catch
#include <vector> // Circle and blob detection
#include <time.h> //gettimeofday

// OpenCV
#include <opencv2/opencv.hpp> //Blur
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

// The address of the RoboDuino
#define SLAVE_ADDRESS 0x29;

// Enable this to disable verbose logging which is being treated as debug level
#define ELPP_STL_LOGGING // Enables lumberjack->info like logging
#define ELPP_DISABLE_PERFORMANCE_TRACKING
#define ELPP_FEATURE_CRASH_LOG // Enables crash logging override to log manager.

INITIALIZE_EASYLOGGINGPP


using namespace cv;
using namespace std;

/*
To compile:
Linux
Requires -std=c++11 or -std=gnu++11 enabled for logging.
g++ ~/Desktop/RobotVision.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_calib3d -lopencv_contrib -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_legacy -lopencv_ml -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_video -lopencv_videostab -std=c++11 -o RobotVision

Windows
Use Visual Studio 2013 or 2015.  Debug mode not available with VS 2015 for OpenCV2.4.9

Schedule the jobs with Cron
@reboot /bin/RobotVision "/home/ubuntu/Desktop/Webcam/lightIncluded.jpg" > /dev/null 2>&1 # JOB_ID_1
*/

// Only do i2c stuff in Linux
#ifdef __linux__
/*
void readFromI2c()
{
char buf[10] = { 0 };
float data;
char channel;

for (int i = 0; i<4; i++)
{
// Using I2C Read
if (read(file, buf, 2) != 2) {
// ERROR HANDLING: i2c transaction failed
lumberjack->info("Failed to read from the i2c bus.");
buffer = g_strerror(errno);
lumberjack->info(buffer);
lumberjack->info("");
}
else {
data = (float)((buf[0] & 0b00001111) << 8) + buf[1];
data = data / 4096 * 5;
channel = ((buf[0] & 0b00110000) >> 4);
lumberjack->info("Channel %02d Data:  %04f", channel, data);
}
}
}
*/

void writeToI2c(FILE* file, int status, uint8_t value = 255 / 2)
{
	char buf[10] = { 0 };

	switch (status)
	{
	case 0:
		buf[0] = 0x00;
		buf[1] = value;
		break;
	case 1:
		// Data update for goal
		buf[0] = 0x01;
		buf[1] = value;
		break;
	case 2:
		// Data update for LowBar
		buf[0] = 0x02;
		buf[1] = value;
		break;
	}

	if (fwrite(buf, sizeof(char), sizeof(buf), file) != sizeof(buf))
	{
		/* ERROR HANDLING: i2c transaction failed */
		LOG(FATAL) << "Failed to write to the i2c bus.";
	}
}

void writeToI2c(int fd, int status, uint8_t value = 255 / 2)
{
	char buf[10] = { 0 };

	switch (status)
	{
	case 0:
		buf[0] = 0x00;
		buf[1] = value;
		break;
	case 1:
		// Data update for goal
		buf[0] = 0x01;
		buf[1] = value;
		break;
	case 2:
		// Data update for LowBar
		buf[0] = 0x02;
		buf[1] = value;
		break;
	}

	if (write(fd, buf, 2) != 2)
	{
		/* ERROR HANDLING: i2c transaction failed */
		LOG(FATAL) << "Failed to write to the i2c bus.";
	}
}
#endif

// Cross platform sleep function takes milliseconds as input
void goToSleep(int sleepMs)
{
#ifdef __linux__
	usleep(sleepMs * 1000); // usleep takes sleep time in us (1 millionth of a second)
#elif _WIN32
	Sleep(sleepMs);
#endif
}

// For I2C we convert the X value to be on a scale of 0 - 255.
int convertX_Scale(int oldRangeMax, int cvX_Value)
{
	// Thanks to http://stackoverflow.com/questions/5294955/how-to-scale-down-a-range-of-numbers-with-a-known-min-and-max-value
	int
		i2cX_Value = 0,
		newRangeMin = 0,
		newRangeMax = 255,
		oldRangeMin = 0;

	i2cX_Value = (((newRangeMax - newRangeMin)*(cvX_Value - oldRangeMin)) / (oldRangeMax - oldRangeMin)) + newRangeMin;
	return i2cX_Value;
}

// comparison function object for use with sorting
bool compareContourAreas(vector<Point> contour1, vector<Point> contour2) {
	double i = fabs(contourArea(Mat(contour1)));
	double j = fabs(contourArea(Mat(contour2)));
	return (i < j);
}

void crashHandler(int sig) 
{
	LOG(FATAL) << "Crashed!";

	el::Helpers::logCrashReason(sig, false, el::Level::Fatal, "default");
	// FOLLOWING LINE IS ABSOLUTELY NEEDED AT THE END IN ORDER TO ABORT APPLICATION
	el::Helpers::crashAbort(sig);
}


/*
// There can be only one!  Check if one process instance is already present and shutdown if so.  Uses boost libraries with a simple lock file.
void highlander()
{

#ifdef __linux__
	char* filePath = "/tmp/robotvision.lock";
#elif _WIN32
	char* filePath = "C:\robotvision.lock";
#endif

	// create an empty file
	std::ofstream outfile(filePath);
	outfile.close();

	boost::interprocess::file_lock m_flock = boost::interprocess::file_lock(filePath);
	const auto couldLock = m_flock.try_lock(); 
	if (!couldLock) 
	{ 
		// Rather than throw an exception just exit since this is desired to quit
		LOG(DEBUG) << "Unable to lock file!";
		exit(0);
	}
}
*/

void identifyLowBar(el::Logger* lumberjack, time_t startTimeStamp, time_t endTimeStamp, Mat origImage, bool isDebugMode, bool isVerboseMode, bool isWritingToWindow, bool* isLowBarSpotted, int* centerLowBarX = 0)
{
	//################# Begin Low Bar Identification #############################
	time(&startTimeStamp);
	// Random number generator for disco disco disco disco!
	RNG lowBarRng(time(NULL));
	Mat blackHueImage, blackHueImage2Gray, blackHueImageEdges;
	vector<vector<Point> > lowBarContours;
	vector<Vec4i> lowBarHierarchy;
	// The treshold was not calibrated.
	int lowBarThresh = 50;
	// Discard contours that are less than this value.
	double minLowBarContourArea = 50000.0;

	// Slightly blur in order to avoid false positives
	GaussianBlur(origImage, blackHueImage, Size(9, 9), 2, 2);

	cvtColor(blackHueImage, blackHueImage, COLOR_BGR2HLS);
	inRange(blackHueImage, Scalar(0, 0, 0), Scalar(180, 100, 255), blackHueImage);
	// Detect edges using canny
	Canny(blackHueImage, blackHueImageEdges, lowBarThresh, lowBarThresh * 2, 3);
	// Find contours
	findContours(blackHueImageEdges, lowBarContours, lowBarHierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	if (isDebugMode && isVerboseMode)
	{
		for (int i = 0; i < lowBarContours.size(); i++)
		{
			vector<Point> contour = lowBarContours[i];
			double area = contourArea(contour);
			lumberjack->verbose(0, "Contour Low Bar: #%v Area: %v", i + 1, area);
		}
	}

	// Sort
	sort(lowBarContours.begin(), lowBarContours.end(), compareContourAreas);

	/// Get the moments (useful for calculations like centoid... center of mass)
	vector<Moments> lowBarMoments(lowBarContours.size());
	for (int i = 0; i < lowBarContours.size(); i++)
	{
		lowBarMoments[i] = moments(lowBarContours[i], false);
	}
	///  Get the mass centers:
	vector<Point2f> lowBarMassCenter(lowBarContours.size());
	for (int i = 0; i < lowBarContours.size(); i++)
	{
		lowBarMassCenter[i] = Point2f(lowBarMoments[i].m10 / lowBarMoments[i].m00, lowBarMoments[i].m01 / lowBarMoments[i].m00);
	}

	/// Draw contours
	Mat lowBarImageContours = Mat::zeros(origImage.size(), CV_8UC3);

	for (int i = 0; i < lowBarContours.size(); i++)
	{
		vector<Point> contour = lowBarContours[i];
		double area = contourArea(contour);
		if (area > minLowBarContourArea)
		{
			*isLowBarSpotted = true;

			*centerLowBarX = lowBarMassCenter[i].x;

			Scalar color = Scalar(lowBarRng.uniform(0, 255), lowBarRng.uniform(0, 255), lowBarRng.uniform(0, 255));
			if (isWritingToWindow)
			{
				drawContours(lowBarImageContours, lowBarContours, i, color, 2, 8, lowBarHierarchy, 0, Point());

				// Draw the center of mass
				circle(lowBarImageContours, lowBarMassCenter[i], 4, color, -1, 8, 0);
			}

			if (isDebugMode)
			{
				lumberjack->info("Largest Contour Area Low Bar: %v", area);
				lumberjack->info("Largest contour Low Bar (X,Y): (%v,%v)", lowBarMassCenter[i].x, lowBarMassCenter[i].y);
				lumberjack->info("Converted X Scale Low Bar: %v", convertX_Scale(origImage.cols, lowBarMassCenter[i].x));
			}
		}
	}

	time(&endTimeStamp);

	if (isDebugMode && isVerboseMode)
	{
		lumberjack->verbose(0, "Time to calculate Low Bar Identification: %v", difftime(startTimeStamp, endTimeStamp));
	}
	//################# End Low Bar Identification #############################
}

bool isJpegFile(const string& fullFilePath)
{
	bool isJpegFile = false;
	size_t found = fullFilePath.find_last_of(".");
	if (fullFilePath.substr(found + 1).compare("jpg") == 0)
	{
		isJpegFile = true;
	}

	return isJpegFile;
}

bool isVideoFile(const string& fullFilePath)
{
	bool isVideoFile = false;
#ifdef __linux__
	string videoContainer = "mp4";
#elif _WIN32
	string videoContainer = "avi";
#endif
	size_t found = fullFilePath.find_last_of(".");
	if (fullFilePath.substr(found + 1).compare(videoContainer) == 0)
	{
		isVideoFile = true;
	}

	return isVideoFile;
}

int main(int argc, char** argv)
{
	el::Helpers::setCrashHandler(crashHandler);

#ifdef __linux__
	// Linux needs to set whitebalance settings
	system("/bin/robotVisionCameraSettingsOverride.sh");
#endif
	// Linux writes to the SD card before it mounts.  Sleep for a little bit.
	//goToSleep(1000 * 30);

	//############################ Begin Logging Setup #################################################
	// Check if the logging directory exists (SD Cart mounted).  If not redirect logging to the temp directory to avoid creating the directory on Linux (Windows will fail to write, Linux will create a directory)
	struct stat info;
	bool isLoggingDirAvailable = false;

#ifdef __linux__
	char pathname[] = "/media/ubuntu/RobotVision/";
#elif _WIN32
	char pathname[] = "E:\\";
#endif
	if (stat(pathname, &info) != 0)
		printf("%s inaccessible.  Will log to the temp directory.\n", pathname);
	else if (info.st_mode & S_IFDIR)  // S_ISDIR() doesn't exist on my windows 
		//printf("%s is a directory\n", pathname);
		isLoggingDirAvailable = true;
	else
		printf("%s is no directory\n", pathname);

	// Load configuration from file
	if (isLoggingDirAvailable)
	{
#ifdef __linux__
		// Linux specific logging config location
		string loggingConfigPath = "/etc/robovision/robovisionLinux.conf";
		el::Configurations loggingConfig(loggingConfigPath);
#elif _WIN32
		// Windows specific logging config location
		string loggingConfigPath = "C:\\robovision\\robovisionWindows.conf";
		el::Configurations loggingConfig(loggingConfigPath);
#endif
		// Reconfigure single logger
		el::Loggers::reconfigureLogger("default", loggingConfig);
		// Actually reconfigure all loggers instead
		el::Loggers::reconfigureAllLoggers(loggingConfig);
		// Now all the loggers will use configuration from file
	}
	// Override the config file based on logging dir availability
	else
	{
#ifdef __linux__
		string altLoggingConfigPath = "/etc/robovision/robovisionAltLinux.conf";
		el::Configurations loggingConfig(altLoggingConfigPath);
#elif _WIN32
		string altLoggingConfigPath = "C:\\robovision\\robovisionAltWindows.conf";
		el::Configurations loggingConfig(altLoggingConfigPath);
#endif
		// Reconfigure single logger
		el::Loggers::reconfigureLogger("default", loggingConfig);
		// Actually reconfigure all loggers instead
		el::Loggers::reconfigureAllLoggers(loggingConfig);
		// Now all the loggers will use configuration from file
	}

	el::Logger* lumberjack = el::Loggers::getLogger("default");
	//############################ End Logging Setup #################################################

	bool 
		isDebugMode = true,
		isVerboseMode = false,
		isWritingToWindow = false;  // Show windows and draw things, unecessary if for the RoboRIO alone.
	int 
		secondsCalculated = 0,
		videoCaptureWidth = 0,
		videoCaptureHeight = 0,
		mediaMode = 0; // 0 for camera, 1 for photo, 2 for video

	// Class for video capturing from video files or cameras.
	VideoCapture videoCapture;
	// Class for video writing from the video capture.
	VideoWriter videoWriter;
	string
#ifdef __linux__
		videoWriterPath = "/media/ubuntu/RobotVision/videoCapture.mp4";
#elif _WIN32
		videoWriterPath = "E:\\videoCapture.avi";
#endif

	time_t
		startTimeStamp, endTimeStamp, fileTimeStamp;
	struct 
		tm * timeInfo;

	if (argc == 1 || (argc == 2 && strlen(argv[1]) == 1 && isdigit(argv[1][0])))
	{
		// Open without param specified or if specified and is digit try to open with specified camera number
		videoCapture.open(argc == 2 ? argv[1][0] - '0' : 0);

		if (!videoCapture.isOpened())
		{
			lumberjack->fatal("Could not initialize capturing...");
			return -1;
		}
		else
		{
			lumberjack->info("Camera initialized for capture.");
		}

		videoCaptureWidth = videoCapture.get(CV_CAP_PROP_FRAME_WIDTH);
		videoCaptureHeight = videoCapture.get(CV_CAP_PROP_FRAME_HEIGHT);

		// -1 opens up a selector of available system codecs.  H264 encoder is not readily available on Windows.
#ifdef __linux__
		videoWriter.open(videoWriterPath, CV_FOURCC('X', '2', '6', '4'), 10, Size(videoCaptureWidth, videoCaptureHeight), true);
#elif _WIN32
		videoWriter.open(videoWriterPath, CV_FOURCC('M', 'J', 'P', 'G'), 10, Size(videoCaptureWidth, videoCaptureHeight), true);
#endif

		mediaMode = 0;
	}
	else if (argc >= 2 && isJpegFile(argv[1]))
	{
		// run this routine if the file is a .jpg
		mediaMode = 1;
	}
	else if (argc >= 2 && isVideoFile(argv[1]))
	{
		// run this routine if the file is a .avi or .mp4
		videoCapture.open(argv[1]);
		if (!videoCapture.isOpened())
		{
			lumberjack->fatal("Could not initialize video...");
			return -1;
		}
		else
		{
			lumberjack->info("Video file initialized.");
		}

		videoCaptureWidth = videoCapture.get(CV_CAP_PROP_FRAME_WIDTH);
		videoCaptureHeight = videoCapture.get(CV_CAP_PROP_FRAME_HEIGHT);

		mediaMode = 2;
	}
	else
	{
		lumberjack->fatal("Invalid file provided.");
	}

	try
	{
		/*
		#ifdef __linux__
		// A little help from here http://elinux.org/Interfacing_with_I2C_Devices and https://www.kernel.org/doc/Documentation/i2c/dev-interface
		// Opening the bus
		FILE* file = NULL;
		char filename[] = "/dev/i2c-1";
		if ((file = fopen(filename, "w")) < 0)
		{
		// ERROR HANDLING: you can check errno to see what went wrong
		lumberjack->fatal("Failed to open the i2c bus");
		el::Loggers::flushAll();
		exit(1);
		}

		// Initiation of communication with the slave
		int addr = SLAVE_ADDRESS;          // The I2C address of the slave device
		if (ioctl(fileno(file), I2C_SLAVE, addr) < 0) {
		lumberjack->fatal("Failed to acquire bus access and/or talk to slave.");
		// ERROR HANDLING; you can check errno to see what went wrong
		el::Loggers::flushAll();
		exit(1);
		}
		#endif
		*/
#ifdef __linux__
		char filename[] = "/dev/i2c-1\0";
		int fd;
		if ((fd = open(filename, O_RDWR)) < 0) {
			// ERROR HANDLING: you can check errno to see what went wrong
			lumberjack->fatal("Failed to open the i2c bus");
			el::Loggers::flushAll();
			exit(1);
		}

		int addr = SLAVE_ADDRESS;          // The I2C address of the slave device
		if (ioctl(fd, I2C_SLAVE, addr) < 0)
		{
			lumberjack->fatal("Failed to acquire bus access and/or talk to slave");
			// ERROR HANDLING; you can check errno to see what went wrong
			el::Loggers::flushAll();
			exit(1);
		}
#endif
		if (isWritingToWindow)
		{
			namedWindow("RoboVision", CV_WINDOW_AUTOSIZE);
		}

		while (true)
		{
			Mat origImage;
			bool bReadSuccess = false;

			// Reset variables each time
			bool isGoalSpotted = false, isLowBarSpotted = false;

			switch (mediaMode)
			{
			case 0:
				// camera
				char 
					jpgFileName[256], timeStampInt[64];
				memset(jpgFileName, 0, sizeof(jpgFileName));
				memset(timeStampInt, 0, sizeof(timeStampInt));
				
				// Linux codecs are not allowing the video to be written due to codecs missing.  Write jpg files.
				strcpy(jpgFileName, pathname);
				time(&fileTimeStamp);
				timeInfo = localtime(&fileTimeStamp);
				//_itoa(static_cast<int>(fileTimeStamp), timeStampInt, 10);
				sprintf(timeStampInt, "%d", fileTimeStamp);
				strcat(jpgFileName, timeStampInt);
				strcat(jpgFileName, ".jpg");

				bReadSuccess = videoCapture.read(origImage);
				if (origImage.empty() || !bReadSuccess)
				{
					lumberjack->fatal("Camera returned empty image...");
					exit(0);
				}
#ifdef _WIN32
				videoWriter.write(origImage);
#endif
				imwrite(jpgFileName, origImage);

				break;
			case 1:
				// photo
				origImage = imread(argv[1], 1);
				isWritingToWindow = true;
				break;
			case 2:
				//video
				bReadSuccess = videoCapture.read(origImage);
				if (origImage.empty() || !bReadSuccess)
				{
					lumberjack->fatal("Camera returned empty image...");
					exit(0);
				}
				isWritingToWindow = true;
				break;
			}

			VLOG_N_TIMES(1, 0) << "Original Image height: " << origImage.rows << ", width: " << origImage.cols;

			// Abort abort!
			if (origImage.data == NULL)
			{
				lumberjack->fatal("No image data");
				return -1;
			}

			//################# Begin Goal Marker Identification #############################
			time(&startTimeStamp);
			// Add blur to the image to clear up noise.  Preserves edges.
			Mat blurredImage;
			medianBlur(origImage, blurredImage, 3);

			//########### EXPERIMENTAL
			// Since MORPH_X : 2,3,4,5 and 6 - select Morphology operation - opening
			int operation = 2;
			// choose kernel size
			int morph_size = 3;
			// select kernel type
			int morph_elem = 0;
			Mat element = getStructuringElement(morph_elem, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));

			morphologyEx(blurredImage, blurredImage, operation, element);
			//########### END

			// Convert input image to HSV
			Mat hsvImage;
			cvtColor(blurredImage, hsvImage, COLOR_BGR2HSV);

			// Threshold the HSV image, keep only the green pixels
			Mat lowerGreenHueRange;
			Mat upperGreenHueRange;
			// Use Photoshop, Gimp, or gColor2 (or similar) to identify the HSV values.
			// Taken from photo with green light right next to the camera (< 1")
			// Original
			//inRange(hsvImage, Scalar(165 / 2, 25, 80), Scalar(180 / 2, 255, 255), lowerGreenHueRange);
			//inRange(hsvImage, Scalar(95 / 2, 1, 80), Scalar(185 / 2, 255, 255), lowerGreenHueRange);
			inRange(hsvImage, Scalar(65 / 2, 0, 230), Scalar(185 / 2, 70, 255), lowerGreenHueRange);
			// Taken from photo with green light held one to two inches from the camera side
			// Original
			//inRange(hsvImage, Scalar(150 / 2, 10, 70), Scalar(170 / 2, 255, 255), upperGreenHueRange);
			inRange(hsvImage, Scalar(145 / 2, 8, 70), Scalar(170 / 2, 255, 255), upperGreenHueRange);

			// Combine the above two images
			Mat greenHueImage;
			addWeighted(lowerGreenHueRange, 1.0, upperGreenHueRange, 1.0, 0.0, greenHueImage);

			// Slightly blur the result, in order to avoid false positives
			GaussianBlur(greenHueImage, greenHueImage, Size(9, 9), 2, 2);
			time(&endTimeStamp);
			secondsCalculated = difftime(startTimeStamp, endTimeStamp);
			if (isDebugMode && isVerboseMode)
			{
				lumberjack->verbose(0, "Time to calculate Goal Marker Identification: %v", secondsCalculated);
			}
			
			//################# End Goal Marker Identification #############################

			//################# Begin Find Goal Marker Contours Lower Green Hue #############################
			time(&startTimeStamp);
			// Random number generator for disco disco disco disco!
			RNG goalMarkerLowerRng(time(NULL));
			// The treshold calibrated with the test program FindContours.exe.
			int goalMarkerLowerThresh = 180; // 180
			// Discard contours that are less than this value.
			double minGoalContourArea = 350.0;
			vector<vector<Point> > goalMarkerLowerContours, goalMarkerLowerContoursFiltered;
			vector<Vec4i> goalMarkerLowerHierarchy;
			Mat greenHueLowerImageEdges;

			// Detect edges using canny
			Canny(lowerGreenHueRange, greenHueLowerImageEdges, goalMarkerLowerThresh, goalMarkerLowerThresh * 2, 3);
			// Find contours
			findContours(greenHueLowerImageEdges, goalMarkerLowerContours, goalMarkerLowerHierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

			if (isDebugMode && isVerboseMode)
			{
				for (int i = 0; i < goalMarkerLowerContours.size(); i++)
				{
					vector<Point> contour = goalMarkerLowerContours[i];
					double area = contourArea(contour);
					lumberjack->verbose(0, "Contour: #%v Area: %v", i + 1, area);
				}
			}

			// Sort to place largest at top
			sort(goalMarkerLowerContours.begin(), goalMarkerLowerContours.end(), compareContourAreas);

			/// Get the moments (useful for calculations like centoid... center of mass)
			vector<Moments> goalMarkerLowerMoment(goalMarkerLowerContours.size());
			for (int i = 0; i < goalMarkerLowerContours.size(); i++)
			{
				goalMarkerLowerMoment[i] = moments(goalMarkerLowerContours[i], false);
			}

			///  Get the mass centers:
			vector<Point2f> goalMarkerLowerMassCenter(goalMarkerLowerContours.size());
			for (int i = 0; i < goalMarkerLowerContours.size(); i++)
			{
				goalMarkerLowerMassCenter[i] = Point2f(goalMarkerLowerMoment[i].m10 / goalMarkerLowerMoment[i].m00, goalMarkerLowerMoment[i].m01 / goalMarkerLowerMoment[i].m00);
			}

			/// Draw contours
			Mat greenHueLowerImageContours = Mat::zeros(greenHueImage.size(), CV_8UC3);

			// Set high to compare against next low found.
			int furthestLeftGoalX_Lower = origImage.cols;

			for (int i = 0; i < goalMarkerLowerContours.size(); i++)
			{
				vector<Point> contour = goalMarkerLowerContours[i];
				double area = contourArea(contour);
				if (area > minGoalContourArea)
				{
					isGoalSpotted = true;

					// Get the furthest left goal out of those found
					if (goalMarkerLowerMassCenter[i].x < furthestLeftGoalX_Lower)
					{
						if (isDebugMode)
						{
							lumberjack->info("Goal found further left: %v", goalMarkerLowerMassCenter[i].x);
						}

						furthestLeftGoalX_Lower = goalMarkerLowerMassCenter[i].x;
					}

					if (isDebugMode && isVerboseMode)
					{
						vector<Point> contour = goalMarkerLowerContours[i];
						double area = contourArea(contour);
						lumberjack->verbose(0, "Largest contour area: %v", area);
					}

					Scalar color = Scalar(goalMarkerLowerRng.uniform(0, 255), goalMarkerLowerRng.uniform(0, 255), goalMarkerLowerRng.uniform(0, 255));
					if (isWritingToWindow)
					{
						drawContours(greenHueLowerImageContours, goalMarkerLowerContours, i, color, 2, 8, goalMarkerLowerHierarchy, 0, Point());

						// Draw the center of mass
						circle(greenHueLowerImageContours, goalMarkerLowerMassCenter[i], 4, color, -1, 8, 0);
					}

					if (isDebugMode)
					{
						lumberjack->info("Largest contour (X,Y): (%v,%v)", goalMarkerLowerMassCenter[i].x, goalMarkerLowerMassCenter[i].y);
						lumberjack->info("Converted X Scale: %v", convertX_Scale(origImage.cols, goalMarkerLowerMassCenter[i].x));
					}
				}
			}

			time(&endTimeStamp);
			secondsCalculated = difftime(startTimeStamp, endTimeStamp);
			if (isDebugMode && isVerboseMode)
			{
				lumberjack->verbose(0, "Time to calculate Find Lower Goal Marker Contours: %v", secondsCalculated);
			}
			//################# End Find Goal Marker Contours Lower Green Hue #############################

			//################# Begin Find Goal Marker Contours Upper Green Hue #############################
			time(&startTimeStamp);
			// Random number generator for disco disco disco disco!
			RNG goalMarkerUpperRng(time(NULL));
			// The treshold calibrated with the test program FindContours.exe.
			int goalMarkerUpperThresh = 180; // 180
			vector<vector<Point> > goalMarkerUpperContours, goalMarkerUpperContoursFiltered;
			vector<Vec4i> goalMarkerUpperHierarchy;
			Mat greenHueUpperImageEdges;

			// Detect edges using canny
			Canny(upperGreenHueRange, greenHueUpperImageEdges, goalMarkerUpperThresh, goalMarkerUpperThresh * 2, 3);
			// Find contours
			findContours(greenHueUpperImageEdges, goalMarkerUpperContours, goalMarkerUpperHierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

			if (isDebugMode && isVerboseMode)
			{
				for (int i = 0; i < goalMarkerUpperContours.size(); i++)
				{
					vector<Point> contour = goalMarkerUpperContours[i];
					double area = contourArea(contour);
					lumberjack->verbose(0, "Contour: #%v Area: %v", i + 1, area);
				}
			}

			// Sort to place largest at top
			sort(goalMarkerUpperContours.begin(), goalMarkerUpperContours.end(), compareContourAreas);

			/// Get the moments (useful for calculations like centoid... center of mass)
			vector<Moments> goalMarkerUpperMoment(goalMarkerUpperContours.size());
			for (int i = 0; i < goalMarkerUpperContours.size(); i++)
			{
				goalMarkerUpperMoment[i] = moments(goalMarkerUpperContours[i], false);
			}

			///  Get the mass centers:
			vector<Point2f> goalMarkerUpperMassCenter(goalMarkerUpperContours.size());
			for (int i = 0; i < goalMarkerUpperContours.size(); i++)
			{
				goalMarkerUpperMassCenter[i] = Point2f(goalMarkerUpperMoment[i].m10 / goalMarkerUpperMoment[i].m00, goalMarkerUpperMoment[i].m01 / goalMarkerUpperMoment[i].m00);
			}

			/// Draw contours
			Mat greenHueUpperImageContours = Mat::zeros(greenHueImage.size(), CV_8UC3);

			// Set high to compare against next low found.
			int furthestLeftGoalX_Upper = origImage.cols;

			for (int i = 0; i < goalMarkerUpperContours.size(); i++)
			{
				vector<Point> contour = goalMarkerUpperContours[i];
				double area = contourArea(contour);
				if (area > minGoalContourArea)
				{
					isGoalSpotted = true;

					// Get the furthest left goal out of those found
					if (goalMarkerUpperMassCenter[i].x < furthestLeftGoalX_Upper)
					{
						if (isDebugMode)
						{
							lumberjack->info("Goal found further left: %v", goalMarkerUpperMassCenter[i].x);
						}

						furthestLeftGoalX_Upper = goalMarkerUpperMassCenter[i].x;
					}

					if (isDebugMode && isVerboseMode)
					{
						vector<Point> contour = goalMarkerUpperContours[i];
						double area = contourArea(contour);
						lumberjack->verbose(0, "Largest contour area: %v", area);
					}

					Scalar color = Scalar(goalMarkerUpperRng.uniform(0, 255), goalMarkerUpperRng.uniform(0, 255), goalMarkerUpperRng.uniform(0, 255));
					if (isWritingToWindow)
					{
						drawContours(greenHueUpperImageContours, goalMarkerUpperContours, i, color, 2, 8, goalMarkerUpperHierarchy, 0, Point());

						// Draw the center of mass
						circle(greenHueUpperImageContours, goalMarkerUpperMassCenter[i], 4, color, -1, 8, 0);
					}

					if (isDebugMode)
					{
						lumberjack->info("Largest contour (X,Y): (%v,%v)", goalMarkerUpperMassCenter[i].x, goalMarkerUpperMassCenter[i].y);
						lumberjack->info("Converted X Scale: %v", convertX_Scale(origImage.cols, goalMarkerUpperMassCenter[i].x));
					}
				}
			}

			time(&endTimeStamp);
			secondsCalculated = difftime(startTimeStamp, endTimeStamp);
			if (isDebugMode && isVerboseMode)
			{
				lumberjack->verbose(0, "Time to calculate Find Upper Goal Marker Contours: %v", secondsCalculated);
			}
			//################# End Find Goal Marker Contours Upper Green Hue #############################


			////################# Begin Boulder Identification #############################
			//time(&startTimeStamp);
			//// Create new image and desaturate to remove any green reflecting from the ball
			//Mat boringGreyBoulderImage;
			//Mat greyHueImage;
			//// 63-116	6-62	75-149
			//inRange(hsvImage, Scalar(0, 0, 0), Scalar(179, 50, 100), greyHueImage);
			////inRange(hsvImage, Scalar(63 / 2, 6, 75), Scalar(116 / 2, 62, 149), greyHueImage);

			//// Desaturate the image
			//cvtColor(blurredImage, boringGreyBoulderImage, COLOR_BGR2GRAY);

			//// Remove data from the image that we don't care about.
			////adaptiveThreshold(boringGreyBoulderImage, boringGreyBoulderImage, 255, ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 11, 2);

			///*
			//for (int i = 1; i < 31; i = i+2)
			//{
			//// Blurring the image doesn't remove false circles as expected so this fixes that.
			//GaussianBlur(boringGreyBoulderImage, boringGreyBoulderImage, Size(i, i), 0, 0);
			//}
			//*/

			//// Detect blobs
			//SimpleBlobDetector blobDetector(params);

			//Mat blobbyBob;

			////Storage for blobs
			//vector<KeyPoint> keypoints;
			//blobDetector.detect(boringGreyBoulderImage, keypoints);
			//drawKeypoints(boringGreyBoulderImage, keypoints, blobbyBob, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

			//// Use the Hough transform to detect circles in the combined threshold image
			//
			//vector<Vec3f> circles;
			////boringGreyBoulderImage.rows / 8 = 91
			//HoughCircles(boringGreyBoulderImage, circles, CV_HOUGH_GRADIENT, 1, 200, 100, 20, 0, 0);

			//if (circles.size() > 0)
			//{
			//	for (size_t current_circle = 0; current_circle < circles.size(); ++current_circle)
			//	{
			//		Point center(round(circles[current_circle][0]), round(circles[current_circle][1]));
			//		int radius = round(circles[current_circle][2]);

			//		circle(boringGreyBoulderImage, center, radius, Scalar(0, 255, 0), 5);
			//	}
			//}
			/*time(&endTimeStamp);
			secondsCalculated = difftime(startTimeStamp, endTimeStamp);
			if (isDebugMode)
			{
			lumberjack->info("Time to calculate Boulder Identification: %v", secondsCalculated);
			}*/
			////################# End Boulder Identification #############################

			int centerLowBarX = 0;
			identifyLowBar(lumberjack, startTimeStamp, endTimeStamp, origImage, isDebugMode, isVerboseMode, isWritingToWindow, &isLowBarSpotted, &centerLowBarX);

#ifdef __linux__
			if (isLowBarSpotted)
			{
				//writeToI2c(file, 2, convertX_Scale(origImage.cols, centerLowBarX));
				LOG_EVERY_N(100, INFO) << "Periodic 100 - Center Low BarX: " << centerLowBarX << " Converted X: Scale: " << convertX_Scale(origImage.cols, centerLowBarX);
				// Status updated to low bar
				writeToI2c(fd, 0, 2);
				goToSleep(30);
				writeToI2c(fd, 2, convertX_Scale(origImage.cols, centerLowBarX));
			}

			if (isGoalSpotted)
			{
				//writeToI2c(file, 1, convertX_Scale(origImage.cols, furthestLeftGoalX));
				// Status updated to goal
				writeToI2c(fd, 0, 1);
				goToSleep(30);
				writeToI2c(fd, 1, convertX_Scale(origImage.cols, furthestLeftGoalX));
				LOG_EVERY_N(100, INFO) << "Periodic 100 - Center GoalX: " << centerLowBarX << " Converted X: Scale: " << convertX_Scale(origImage.cols, furthestLeftGoalX);
			}

			if (!isLowBarSpotted && !isGoalSpotted)
			{
				// Clear low bar and goal status
				writeToI2c(fd, 0, 0);
			}
#endif
			// Force logs to flush every cycle
			//lumberjack->flush();

			if (isWritingToWindow)
			{
				// Visualize Robot Vision :)
				int interpolation = INTER_LINEAR;

				// specify fx and fy and let the function compute the destination image size.
				//resize(lowerGreenHueRange, lowerGreenHueRange, Size(), 0.5, 0.5, interpolation);
				//resize(upperGreenHueRange, upperGreenHueRange, Size(), 0.5, 0.5, interpolation);
				//resize(greenHueImage, greenHueImage, Size(), 0.5, 0.5, interpolation);
				//resize(blobbyBob, blobbyBob, Size(), 0.5, 0.5, interpolation);
				//resize(blackHueImage, blackHueImage, Size(), 0.5, 0.5, interpolation);

				imshow("RoboVisionOriginal", origImage);
				imshow("RobotVisionLower", lowerGreenHueRange);
				imshow("RobotVisionUpper", upperGreenHueRange);
				imshow("RoboVision", greenHueImage);
				//imshow("RobotVisionLowBar", lowBarImageContours);
				imshow("FindUpperContours", greenHueUpperImageContours);
				imshow("FindLowerContours", greenHueLowerImageContours);

				if (isDebugMode && isVerboseMode)
				{
					// Extremely verbose...
					/// Calculate the area with the moments 00 and compare with the result of the OpenCV function
					//lumberjack->verbose(0, "Info: Goal Area and Goal Contour Length");
					//for (int i = 0; i < goalMarkerContours.size(); i++)
					//lumberjack->verbose(0, " * Goal Contour[%v] - Area (M_00) = %v - Area OpenCV: %v - Length: %v ", i, goalMarkerMoment[i].m00, contourArea(goalMarkerContours[i]), arcLength(goalMarkerContours[i], true));
				}

				if (mediaMode == 1) // photo
				{
					break;
				}
				else
				{
					if (cvWaitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
					{
						lumberjack->info("esc key is pressed by user");
						break;
					}
				}
			}

			if (isDebugMode)
			{
				// Extremely verbose...
				/// Calculate the area with the moments 00 and compare with the result of the OpenCV function
				/*lumberjack->verbose(0, "Info: Goal Area and Goal Contour Length");
				for (int i = 0; i < goalMarkerContours.size(); i++)
				lumberjack->verbose(0, " * Goal Contour[%v] - Area (M_00) = %v - Area OpenCV: %v - Length: %v ", i, goalMarkerMoment[i].m00, contourArea(goalMarkerContours[i]), arcLength(goalMarkerContours[i], true));*/
			}
		}

		if (mediaMode == 1)
		{
			cvWaitKey(0);
		}

	}
	catch (exception& e)
	{
		lumberjack->error("Exception: %v", e.what());
	}

	return 0;
}
