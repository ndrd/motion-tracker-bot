#ifndef BLOB_TRACKING_HPP
#define BLOB_TRACKING_HPP

#include <iostream>
#include "opencv2/opencv.hpp" 
#include "opencv2/highgui/highgui.hpp"  
#include <cv.h>
#include <highgui.h>
#include "cvblob.h"

using namespace cv;
using namespace cvb;

class BlobTracking

{
	private:
		bool first_time;
		int min_area;
		int max_area;

		bool debug_track;
		bool debug_blob;
		bool show_blob_mask;
		bool show_output;

		CvTracks tracks;
		void save_config();
		void load_config();

	public:
		BlobTracking();
		~BlobTracking();

		void process_blob(const Mat &img_input, const Mat &img_mask, Mat &img_output );
		const CvTracks get_tracks(); 
};

#endif