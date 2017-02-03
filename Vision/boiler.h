#pragma once
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <math.h>
using namespace cv;
using namespace std;

/**
* boiler class.
* 
* Autogenerated Pipeline from GRIP
*
* Make sure to set sources before running process()
*/
class boiler {
	private:
		Mat source0;
		Mat cvResizeOutput;
		Mat hslThresholdOutput;
		vector<vector<Point> > findContoursOutput;
		void cvResize(Mat &, Size &, double , double , int , Mat &);
		void hslThreshold(Mat &, double [], double [], double [], Mat &);
		void findContours(Mat &, bool , vector<vector<Point> > &);

	public:
		boiler();
		void Process();
		void setsource0(Mat &source0);
		Mat* getcvResizeOutput();
		Mat* gethslThresholdOutput();
		vector<vector<Point> >* getfindContoursOutput();
};

