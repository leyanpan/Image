#ifndef NOISE_H
#define NOISE_H

#include <cmath>
#include <iostream>

#include "Types.h"
#include "myutils.h"

using namespace std;
using cv::Mat;
using cv::GaussianBlur;
using cv::medianBlur;
using cv::bilateralFilter;

using std::abs;
using std::sqrt;
using std::cout;
using std::endl;

int compareNoise(Mat img)
{
	Mat Gauss, Med, Bil, edges;
	array2D denoised = nullptr, edges_array = nullptr;
	cv::Canny(img, edges, 50, 120);
	cv::GaussianBlur(img, Gauss, cv::Size(5, 5), 0);
	cv::medianBlur(img, Med, 5);
	cv::bilateralFilter(img, Bil, 5, 50, 50);
	Dynamic_alloc(denoised, img.size[0], img.size[1] * img.channels());
	Dynamic_alloc(edges_array, img.size[0], img.size[1]);
	FOR_(i, img.size[0])
	{
		auto gp = Gauss.ptr<uchar>(i);
		auto mp = Med.ptr<uchar>(i);
		auto bp = Bil.ptr<uchar>(i);
		FOR_(j, img.size[1] * img.channels())
		{
			denoised[i][j] = (uchar)(((int)gp[j] + mp[j] + 2 * bp[j] + 2) / 4);
		}
	}
	for(int i = 4; i < img.size[0] - 2; i++)
	{
		auto edgep = edges.ptr<uchar>(i);
		for(int j = 2; j < img.size[1] - 2;j++)
		{
			if (edgep[j] == 255)
			{
				for (int k = i - 4; k <= i + 2; k++)
					for (int l = j - 2; l <= j + 2; l++)
						edges_array[k][l]++;
			}
		}
	}
	Gauss.release();
	Med.release();
	Bil.release();
	edges.release();
	Mat noise(img.size[0], img.size[1], CV_8UC1);
	int cnt = 0;
	int last_diff = 1;
	FOR_(i, img.size[0])
	{
		auto p = img.ptr<uchar>(i);
		auto noisep = noise.ptr<uchar>(i);
		FOR_(j, img.size[1] - 2)
		{
			int diff = 0;
			int colorcnt = 0;
			FOR_(k, img.channels())
			{
				if (denoised[i][j * img.channels() + k] < 10 || denoised[i][j * img.channels() + k] > 246)
					continue;
				diff += abs(p[j * img.channels() + k] - denoised[i][j * img.channels() + k]);
				colorcnt++;
			}
			if (colorcnt == 0)
				continue;
			diff = diff / colorcnt * 3;
			if (edges_array[i][j])
			{
				diff = last_diff;
			}
			noisep[j] = (diff * 10 > 255 ? 255 : diff * 10);
			last_diff = diff;
		}
	}
	Dynamic_free(denoised, img.size[0], img.size[1] * img.channels());
	Dynamic_free(edges_array, img.size[0], img.size[1]);

	cv::namedWindow("noise", CV_WINDOW_NORMAL);
	cv::imshow("noise", noise);
	cv::waitKey(0);
	cv::destroyAllWindows();
	return 0;
}

#endif
