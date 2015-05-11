#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/opencv_modules.hpp"
#include "opencv2/core/core.hpp"

#include <iostream>

using namespace std;
using namespace cv;


class cars
{
	public:

	Mat image_in;
	Mat image_result;
	Mat storage;

	CascadeClassifier cascade;
	CascadeClassifier check_cascade;

	int instances;

	void get_image(Mat src)
	{
		if (!src.data) {
			cout << "Not input image" << endl;
		} else {
			image_in = src.clone();
			storage = src.clone();
			image_result = src.clone();
		}
	}

	void cascade_load(string cascade_path)
	{
		cascade.load(cascade_path);

		if (!cascade.load(cascade_path)) {
			cout << endl << "Could not load classifier cascade" << endl;
		} else {
			cout << "cascade: " << cascade_path << " loaded" << endl;
		}
	}

	void check_cascade_load(string check_cascade_path)
	{
		check_cascade.load(check_cascade_path);

		if (!check_cascade.load(check_cascade_path)) {
			cout << endl << "Could not load the training cascade";
		} else {
			cout << "check_cascade: " << check_cascade_path << " loaded";
		}
	}


	void display_input()
	{
		namedWindow("display_input");
		imshow("display_input", image_in);
	}

	void display_output()
	{

		if (!image_result.empty()) {
			namedWindow("display_output");
			imshow("display_output", image_result);
			waitKey(0);
		}
	}

	void set_instances() 
	{
		instances = 0;
	}

	void detect_cars()
	{
		int i = 0;
		Rect max_rectangle;
		Mat img =  storage.clone();
		Mat temp;
		Mat rezise_image_to_region_of_interest;
		vector<Rect> nested_cars;
		Point center;
		
		const static Scalar colors [] = {
			CV_RGB(0,0,255), CV_RGB(0,255,0),
			CV_RGB(255,0,0), CV_RGB(255,255,0),
			CV_RGB(255,0,255), CV_RGB(0,255,255),
			CV_RGB(255,255,255), CV_RGB(128,0,0),
			CV_RGB(0,128,0), CV_RGB(0,0,128),
			CV_RGB(128,128,128),CV_RGB(0,0,0)
		};

		Scalar color  = colors[i%8];

		if (img.empty()) {
			cout << endl << "An error ocurred with the detector";
		} 

		int center_x;
		int center_y;
		vector<Rect> cars;

		Mat grayscale;

		cvtColor(img, grayscale, CV_BGR2GRAY);

		Mat resize_image(cvRound(img.rows), cvRound(img.cols), CV_8UC1);

		resize(grayscale, resize_image, resize_image.size(), 0, 0, INTER_LINEAR);
		equalizeHist(resize_image, resize_image);

		cascade.detectMultiScale( resize_image, cars, 1.1, 2, 0, Size(10,10));

		for ( vector<Rect>::const_iterator main = cars.begin(); main != cars.end(); main++, i++)
		{
			if (main[i].area() > max_rectangle.area())
				max_rectangle = main[i];
		}

		int x0 = cvRound(max_rectangle.x);
		int y0 = cvRound(max_rectangle.y);
		int x1 = cvRound((max_rectangle.x + max_rectangle.width - 1));
		int y1 = cvRound((max_rectangle.y + max_rectangle.height - 1));

		rezise_image_to_region_of_interest = resize_image(max_rectangle);
		check_cascade.detectMultiScale(rezise_image_to_region_of_interest, nested_cars, 1.1, 2, 0, Size(30,30));

		for (vector<Rect>::const_iterator sub = nested_cars.begin(); sub != nested_cars.end(); sub++)
		{
			center.x = cvRound((max_rectangle.x + sub->x + sub->width * 0.5));
			center_x = center.x;
		
			center.y = cvRound((max_rectangle.y + sub->y + sub->height * 0.5));
			center_y = center.y;

			if (center_x > (x0 + 15) && center_x < (x1-15) &&
				center_y > (y0 + 15) && center_y < (y1-15))
			{
				rectangle(image_result, cvPoint(x0, y0),
							cvPoint(x1,y1),
							color, 3, 8, 0);

				Rect region_of_interest = Rect(x0, y0, x1-x0, y1-y0);
				temp = storage(region_of_interest);
				temp = Scalar(255,255,255);

				instances += 1;
			}


		}

		if ( image_result.empty())
		{
			cout << endl << "Result storage not sucessfull" << endl;
		}

		return;
	}
};


int main(int argc, const char* argv[])
{
	double t = 0;
	t = (double) getTickCount();
	VideoCapture cap(argv[1	]);

	if (!cap.isOpened()) {
		cout << "Cannot open the video file" << endl;
		return -1;
	}

	string check_cascade = argv[2];
	
	cars car_detector;
	car_detector.set_instances();
	car_detector.check_cascade_load(check_cascade);

	if (argc > 3) {
		for (int i = 3; i < argc; i++)
		{
			string cascade = argv[i];
			car_detector.cascade_load(cascade);
		}
	} else {
		cout << endl << "Provide atleast one cascade xml file" << endl;
	}

	while (1)
	{
		Mat image;
		bool success = cap.read(image);

		if (!success) {
			cout << "Cannot read the frame from the video file " << endl;
			break;
		}

		Mat img;
		resize(image, img, Size(300, 150),0,0, INTER_LINEAR);
		car_detector.get_image(image);
		car_detector.detect_cars();


		if (car_detector.instances != 0) {
			cout << endl << car_detector.instances << " cars detected in " <<  ((double)getTickCount() - t) / getTickFrequency() << " ms" << endl;
			car_detector.set_instances();
		} else {
			cout << endl << " cars not found " << endl;
		}


	}


	return 0;
}

