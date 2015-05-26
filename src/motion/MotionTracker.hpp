/*
* MotionTracker.hpp
* Utilities to detect, and track objects based
* on the background substraction of a scene, render 
* other elements as blobs, and taking his positions 
* in other blobs
*/

#include <iostream>
#include <string>
#include "opencv2/opencv.hpp" 
#include "cvblob.h"

using namespace cv;
using namespace cvb;
using namespace std;

enum Orientation 
{
	NONE 		= 0,	
	HORIZONTAL 	= 1,
	VERTICAL 	= 2
};

enum ObjectPosition
{
	STOP 	= 3,
	START 	= 0,
	HALF 	= 1,
	END		= 2
};

class MotionTracker
{
	private:
		bool debug;
		int img_w;
		int img_h;
		Mat img_input;
		CvTracks tracks;
		map <CvID, vector<CvPoint2D64f> > points;
		Orientation orientation;
		map <CvID, ObjectPosition> positions;
		long objectFromStartToEnd;
		long objectFromEndToStart;

	public:
		MotionTracker();
		
		void init (const Mat &img, string filename);
		void setTracks( const CvTracks &tracks );
		void detect(Mat &img, long &frame, const long &fps);

	private:
		ObjectPosition getObjectPosition(const CvPoint2D64f centroid);

		void saveConfig(const string name);
		void loadConfig(const string name);

};