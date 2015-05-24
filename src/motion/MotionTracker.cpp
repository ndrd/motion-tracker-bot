#include "MotionTracker.hpp"

namespace Position 
{
	IplImage* img_input1 = 0;
	IplImage* img_input2 = 0;
	
	Point p1;
	Point p2;
	Point p3;
	Point p4;	

	int startDraw = 0;
	bool configured = false;

	void firstLine_on_mouse(int evt, int x, int y, int flag, void* param)
	{
		if ( evt == CV_EVENT_LBUTTONDOWN)
		{
			if (!startDraw)
			{
				p1 = Point(x,y);
				startDraw = 1;
			}
			else 
			{
				p2 = Point(x,y);
				startDraw = 0;
			}
		}

		if (evt == CV_EVENT_LBUTTONDOWN && startDraw)
		{
			img_input2 = cvCloneImage(img_input1);
			cvLine(img_input2, p1, p2, CV_RGB(255,0,255));
			cvShowImage("Object counting", img_input2);
			cvReleaseImage(&img_input2);
			startDraw = 0;
		}
	}

	void secondLine_on_mouse(int evt, int x, int y, int flag, void* param)
	{
		if ( evt == CV_EVENT_LBUTTONDOWN)
		{
			if (!startDraw)
			{
				p3 = Point(x,y);
				startDraw = 1;
			}
			else 
			{
				p4 = Point(x,y);
				startDraw = 0;
			}
		}

		if (evt == CV_EVENT_LBUTTONDOWN && startDraw)
		{
			img_input2 = cvCloneImage(img_input1);
			cvLine(img_input2, p1, p2, CV_RGB(255,0,255));
			cvShowImage("Object counting", img_input2);
			cvReleaseImage(&img_input2);
		}
	}
}


MotionTracker::MotionTracker(): configured(false), objectFromStartToEnd(0), objectFromEndToStart(0)
{
  std::cout << "MotionTracker()" << std::endl;
}

void MotionTracker::init()
{
	loadConfig();
}

void MotionTracker::setImageBlob(const cv::Mat &i)
{
  img_input = i;

}

void MotionTracker::setTracks(const cvb::CvTracks &t)
{
  tracks = t;
}

ObjectPosition MotionTracker::getObjectPosition(const CvPoint2D64f centroid)
{
	ObjectPosition position = STOP;

	if (orientation == HORIZONTAL)
	{
		if ( centroid.x  < Position::p1.x ) 
		{
			position = START;
		}
		if ( centroid.x > Position::p2.x )
		{
			position =  END;
		}
	}

	if (orientation == VERTICAL)
	{
		if ( centroid.y  < Position::p1.y ) 
		{
			position = START;
		}
		if ( centroid.y > Position::p2.y )
		{
			position =  END;
		}
	}

	return position;
}

void MotionTracker::detect()
{
	if ( img_input.empty() )
		return;

  	img_w = img_input.size().width;
  	img_h = img_input.size().height;

  	if ( !Position::configured )
  		loadConfig();
}

void MotionTracker::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("config/MotionTracker.xml", 0, CV_STORAGE_WRITE);

  cvWriteInt(fs, "debug", debug);  
  cvWriteInt(fs, "configured", Position::configured);
  cvWriteInt(fs, "l1p1x", Position::p1.x);
  cvWriteInt(fs, "l1p1y", Position::p1.y);
  cvWriteInt(fs, "l1p2x", Position::p2.x);
  cvWriteInt(fs, "l1p2y", Position::p2.y);
  
  cvReleaseFileStorage(&fs);
}

void MotionTracker::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("config/MotionTracker.xml", 0, CV_STORAGE_READ);

  debug = cvReadIntByName(fs, 0, "debug", true);
  Position::configured = cvReadIntByName(fs, 0, "configured", 1);
  Position
  
  cvReleaseFileStorage(&fs);
}