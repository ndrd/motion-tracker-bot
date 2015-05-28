#include "MotionTracker.hpp"
#include <string>
#include <iostream>

namespace Position 
{
	Mat img1;
	Mat img2;

	IplImage* img_input1 = 0;
	IplImage* img_input2 = 0;
	
	//line start
	Point p1;
	Point p2;

	//line end
	Point p3;
	Point p4;

	//line half
	Point p5;
	Point p6;	

	bool startDraw = false;
	bool l1Defined =  false;
	bool l2Defined = false;
	int linesDrawed = 0;
	bool configured = false;
	string videoFilename =  "";

	void firstLine_on_mouse(int evt, int x, int y, int flag, void* param)
	{
		if (evt == CV_EVENT_LBUTTONDOWN)
		{
			if (!startDraw)
			{
				cout << "starting " << x << " , " << y << endl; 
				p1.x = x;
				p1.y = y;
				startDraw = true;
			}
			else 
			{
				p2.x = x;
				p2.y = y;
				startDraw = false;
				l1Defined = true;

			}
		}

		if (evt == CV_EVENT_LBUTTONDOWN && startDraw)
		{
			cout << "end " << x << " , " << y << endl; 
			line(img1, p1, p2, CV_RGB(255,0,0));
		}
	}

	void secondLine_on_mouse(int evt, int x, int y, int flag, void* param)
	{
		if (evt == CV_EVENT_LBUTTONDOWN)
		{
			if (!startDraw)
			{
				cout << "starting " << x << " , " << y << endl; 
				p3.x = x;
				p3.y = y;
				startDraw = true;
			}
			else 
			{
				p4.x = x;
				p4.y = y;
				startDraw = false;
				l2Defined = true;

			}
		}

		if (evt == CV_EVENT_LBUTTONDOWN && startDraw)
		{
			cout << "end " << x << " , " << y << endl; 
			line(img1, p3, p4, CV_RGB(0,255,0));
		}
	}

}


MotionTracker::MotionTracker(): objectFromStartToEnd(0), objectFromEndToStart(0)
{
  std::cout << "MotionTracker()" << std::endl;
}

void MotionTracker::setFPS(const double f)
{
  fps = f;
}

void MotionTracker::init(const Mat &img, string filename)
{
	img_input = img;

	if ( img_input.empty() )
		return;

	loadConfig(filename);

	Position::configured = (filename.compare(Position::videoFilename) == 0);

	if (!Position::configured)
	{
		Position::videoFilename = filename;

		int key  = 0;
		Position::img1 = img_input;

		do
		{
			cv::imshow("Draw start line", Position::img1);
			cvSetMouseCallback("Draw start line", Position::firstLine_on_mouse, NULL);
  			key = cvWaitKey(0);

  			if (Position::l1Defined) 
				break;
			

		} while(1) ;

		do
		{
			cv::imshow("Draw end line", Position::img1);
			cvSetMouseCallback("Draw end line", Position::secondLine_on_mouse, NULL);
  			key = cvWaitKey(0);

  			if (Position::l2Defined) 
				break;			

		} while(1) ;

		// half line
		Position::p5.x = (int) (Position::p1.x + Position::p3.x) / 2.0;
		Position::p5.y = (int) (Position::p1.y + Position::p3.y) / 2.0;
		Position::p6.x = (int) (Position::p2.x + Position::p4.x) / 2.0;
		Position::p6.y = (int) (Position::p2.y + Position::p4.y) / 2.0;


		cout << "Distance, between lines? (meters): " << endl;
		cin >> realDistance;

		Position::configured =  true;

		saveConfig(filename);

	}

	if (abs(Position::p1.x - Position::p2.x) > abs(Position::p1.y - Position::p2.y))
		orientation = HORIZONTAL;
	else
		orientation = VERTICAL;

	cout << "orientation " << orientation << endl;


}

void MotionTracker::setTracks(const CvTracks &t)
{
	tracks = t;
}

ObjectPosition MotionTracker::getObjectPosition(const CvPoint2D64f centroid)
{
	ObjectPosition position = STOP;

	if (orientation == HORIZONTAL)
	{
		cout << "HORIZONTAL";
		if ( centroid.x  < Position::p1.x ) 
		{
			position = START;
		}
		else if ( centroid.x >= Position::p5.x - UMBRAL || centroid.x <= Position::p6.x + UMBRAL )
		{
			cout << "HALF" << endl;
			 position = HALF;
		} 
		else
		{
			position =  END;
		}
	}

	if (orientation == VERTICAL)
	{
		cout << "vertical";
		if ( centroid.y  < Position::p1.y ) 
		{
			position = START;
		}
		else if (centroid.x >= Position::p5.x - UMBRAL || centroid.x <= Position::p6.x + UMBRAL  )
		{
			cout << "HHaLf" << endl;
			position = HALF;
		}
		else if ( centroid.y > Position::p2.y )
		{
			position =  END;
		}
	}

	return position;
}

void MotionTracker::detect(Mat &img_input, long &frame)
{
	if ( img_input.empty() )
		return;

  	img_w = img_input.size().width;
  	img_h = img_input.size().height;

  	if ( !Position::configured )
  		loadConfig(Position::videoFilename);


  for(std::map<cvb::CvID,cvb::CvTrack*>::iterator it = tracks.begin() ; it != tracks.end(); it++)
	{
		CvID id = (*it).first;
		CvTrack* track = (*it).second;

		CvPoint2D64f centroid  = track->centroid;

		if (track->inactive == 0) 
		{
			if (positions.count(id) > 0)
			{
				map<CvID, ObjectPosition>::iterator ita = positions.find(id);
				ObjectPosition previous = ita->second;

				ObjectPosition current  = getObjectPosition(centroid);

				if (current != previous)
				{
					if ( previous == START && current == END)
					{
						objectFromStartToEnd++;
					} 
					else if ( previous == END && current == START )
					{
						objectFromEndToStart++;
					}
				}
				else if (current ==  HALF)
				{
					cout << "speed" << realDistance / (track->lifetime/fps) << "m/s" << endl;

				}

			}
			else
			{
				cout << "First position" << endl;

				ObjectPosition position = getObjectPosition(centroid);
				cout << position << endl;

				if (position != STOP)
					positions.insert(pair<CvID, ObjectPosition>(id, position));
			}
		}
	}

  	/* show lines */
	line(img_input, Position::p1, Position::p2, CV_RGB(255,0,0));
	line(img_input, Position::p3, Position::p4, CV_RGB(0,255,0));
	line(img_input, Position::p5, Position::p6, CV_RGB(255,0,255));

	imshow("MotionTracker", img_input);
}

void MotionTracker::saveConfig(string name)
{
	string fn = "config/name.xml";
	cout << fn.c_str() << endl;

	CvFileStorage* fs = cvOpenFileStorage(fn.c_str(), 0, CV_STORAGE_WRITE);

	cvWriteString(fs, "videoFilename", Position::videoFilename.c_str());

	cvWriteInt(fs, "debug", debug);  
	cvWriteInt(fs, "configured", Position::configured);
	//line start
	cvWriteInt(fs, "l1p1x", Position::p1.x);
	cvWriteInt(fs, "l1p1y", Position::p1.y);
	cvWriteInt(fs, "l1p2x", Position::p2.x);
	cvWriteInt(fs, "l1p2y", Position::p2.y);
	//line end
	cvWriteInt(fs, "l2p1x", Position::p3.x);
	cvWriteInt(fs, "l2p1y", Position::p3.y);
	cvWriteInt(fs, "l2p2x", Position::p4.x);
	cvWriteInt(fs, "l2p2y", Position::p4.y);
	//half line
	cvWriteInt(fs, "l3p1x", Position::p5.x);
	cvWriteInt(fs, "l3p1y", Position::p5.y);
	cvWriteInt(fs, "l3p2x", Position::p6.x);
	cvWriteInt(fs, "l3p2y", Position::p6.y);
	cvWriteReal(fs, "distance", realDistance);


	cvReleaseFileStorage(&fs);
}

void MotionTracker::loadConfig(string name)
{
	string fn = "config/name.xml";
	cout << fn.c_str() << endl;

	CvFileStorage* fs = cvOpenFileStorage(fn.c_str(), 0, CV_STORAGE_READ);

	debug = cvReadIntByName(fs, 0, "debug", true);
	realDistance = cvReadRealByName(fs, 0, "distance", 1);
	Position::configured = cvReadIntByName(fs, 0, "configured", 1);
	Position::videoFilename = cvReadStringByName(fs, 0, "videoFilename", "");
	// read the first line
	Position::p1.x = cvReadIntByName(fs, 0,  "l1p1x", 0);
	Position::p1.y = cvReadIntByName(fs, 0,  "l1p1y", 0);
	Position::p2.x = cvReadIntByName(fs, 0,  "l1p2x", 0);
	Position::p2.y = cvReadIntByName(fs, 0,  "l1p2y", 0);
	// read the second line
	Position::p3.x = cvReadIntByName(fs, 0,  "l2p1x", 0);
	Position::p3.y = cvReadIntByName(fs, 0,  "l2p1y", 0);
	Position::p4.x = cvReadIntByName(fs, 0,  "l2p2x", 0);
	Position::p4.y = cvReadIntByName(fs, 0,  "l2p2y", 0);
	// read the half line
	Position::p5.x = cvReadIntByName(fs, 0,  "l3p1x", 0);
	Position::p5.y = cvReadIntByName(fs, 0,  "l3p1y", 0);
	Position::p6.x = cvReadIntByName(fs, 0,  "l3p2x", 0);
	Position::p6.y = cvReadIntByName(fs, 0,  "l3p2y", 0);

	cvReleaseFileStorage(&fs);
}
