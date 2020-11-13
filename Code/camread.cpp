/*

g++ -std=c++11 -ggdb capture_images.cpp -o camCapture.out `pkg-config --cflags --libs opencv`

*/
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace cv;

int x = 0;

int main()
{
	string imgs_directory;
	string extension;
	int im_width, im_height;

	imgs_directory = "Image/";
	extension = ".jpg";
	im_width = 640;
	im_height = 480;

	VideoCapture cap1(0);
	if (!cap1.isOpened()) 
	{
        cerr << "ERROR: Could not open camera, reconnect it..." << endl;
        return 1;
    }
	Mat img1, img_res1;
	while (1) 
	{
		cap1 >> img1; 
		resize(img1, img_res1, Size(im_width, im_height));
		if(img1.empty())
            break;
		
		imshow("Leftcamera", img_res1);
		if (waitKey(30) > 0) 
		{
			string filename1, filename2;
			char capture = waitKey(30);
			if(capture == 's')
			{
				x++;
				filename1 = imgs_directory + "left/"+ "left" + to_string(x) + extension;
				cout << filename1 << endl;
				waitKey(10);
				cout << "filename1 = " << filename1 << endl;
				cout << "Saving img pair " << x << endl;
				imwrite(filename1, img_res1);
			}
			char c = waitKey(30);
			if(c == 27)
				break;
		}
	}
	return 0;
}
