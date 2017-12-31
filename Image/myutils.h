#ifndef MYUTILS_H
#define MYUTILS_H

#define FOR_(__arg, __end) for(int __arg = 0; __arg < __end; __arg++)
#include "Types.h"

typedef uchar** array2D;

void Dynamic_alloc(array2D & target, int x, int y)
{
	target = new uchar* [x];
	for (int i = 0; i < x; i++)
	{
		target[i] = new uchar[y] {0};
	}
}

void Dynamic_free(array2D target, int x, int y)
{
	for (int i = 0; i < x; i++)
	{
		delete target[i];
	}
	delete target;
}


void displayImage(Mat img)
{
	cv::namedWindow("Image", CV_WINDOW_NORMAL);
	cv::imshow("Image", img);
	cv::waitKey(0);
	cv::destroyAllWindows();
}
#endif // !MYUTILS_H
