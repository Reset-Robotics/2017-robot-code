#include "boiler.h"
/**
* Initializes a boiler.
*/
boiler::boiler() {
}
/**
* Runs an iteration of the Pipeline and updates outputs.
*
* Sources need to be set before calling this method. 
*
*/
void boiler::Process(){
	//Step CV_resize0:
	//input
	Mat cvResizeSrc = source0;
	Size cvResizeDsize(0, 0);
	double cvResizeFx = 0.5;
	double cvResizeFy = 0.5;
	int cvResizeInterpolation = INTER_LINEAR;
	cvResize(cvResizeSrc, cvResizeDsize, cvResizeFx, cvResizeFy, cvResizeInterpolation, this->cvResizeOutput);
	//Step HSL_Threshold0:
	//input
	Mat hslThresholdInput = cvResizeOutput;
	double hslThresholdHue[] = {79.31654676258994, 102.07130730050932};
	double hslThresholdSaturation[] = {149.05575539568343, 255.0};
	double hslThresholdLuminance[] = {45.86330935251798, 255.0};
	hslThreshold(hslThresholdInput, hslThresholdHue, hslThresholdSaturation, hslThresholdLuminance, this->hslThresholdOutput);
	//Step Find_Contours0:
	//input
	Mat findContoursInput = hslThresholdOutput;
	bool findContoursExternalOnly = false;
	findContours(findContoursInput, findContoursExternalOnly, this->findContoursOutput);
}

/**
 * This method is a generated setter for source0.
 * @param source the Mat to set
 */
void boiler::setsource0(Mat &source0){
	source0.copyTo(this->source0);
}
/**
 * This method is a generated getter for the output of a CV_resize.
 * @return Mat output from CV_resize.
 */
Mat* boiler::getcvResizeOutput(){
	return &(this->cvResizeOutput);
}
/**
 * This method is a generated getter for the output of a HSL_Threshold.
 * @return Mat output from HSL_Threshold.
 */
Mat* boiler::gethslThresholdOutput(){
	return &(this->hslThresholdOutput);
}
/**
 * This method is a generated getter for the output of a Find_Contours.
 * @return ContoursReport output from Find_Contours.
 */
vector<vector<Point> >* boiler::getfindContoursOutput(){
	return &(this->findContoursOutput);
}
	/**
	 * Resizes an Image.
	 * @param src The image to resize.
	 * @param dSize size to set the image.
	 * @param fx scale factor along X axis.
	 * @param fy scale factor along Y axis.
	 * @param interpolation type of interpolation to use.
	 * @param dst output image.
	 */
	void boiler::cvResize(Mat &src, Size &dSize, double fx, double fy, int interpolation, Mat &dst) {
		resize(src, dst, dSize, fx, fy, interpolation);
	}
	/**
	 * Segment an image based on hue, saturation, and luminance ranges.
	 *
	 * @param input The image on which to perform the HSL threshold.
	 * @param hue The min and max hue.
	 * @param sat The min and max saturation.
	 * @param lum The min and max luminance.
	 * @param output The image in which to store the output.
	 */
	//void hslThreshold(Mat *input, double hue[], double sat[], double lum[], Mat *out) {
	void boiler::hslThreshold(Mat &input, double hue[], double sat[], double lum[], Mat &out) {
		cvtColor(input, out, COLOR_BGR2HLS);
		inRange(out, Scalar(hue[0], lum[0], sat[0]), Scalar(hue[1], lum[1], sat[1]), out);
	}
	/**
	 * Finds contours in an image.
	 *
	 * @param input The image to find contours in.
	 * @param externalOnly if only external contours are to be found.
	 * @param contours vector of contours to put contours in.
	 */
	void boiler::findContours(Mat &input, bool externalOnly, vector<vector<Point> > &contours) {
		vector<Vec4i> hierarchy;
		contours.clear();
		int mode = externalOnly ? RETR_EXTERNAL : RETR_LIST;
		int method = CHAIN_APPROX_SIMPLE;
		findContours(input, contours, hierarchy, mode, method);
	}

