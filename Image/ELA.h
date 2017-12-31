#ifndef ELA_H
#define ELA_H

#include "Types.h"
#include "myutils.h"
#include <vector>
#include <cassert>
#include <cmath>

using cv::Mat;
using cv::imwrite;
using cv::imread;

using std::vector;
using std::abs;

#define TEMP_IMAGE_NAME "ELA_analysis_temp.jpg"

void ELA_analysis(Mat img, Mat edges, int min_display = 0, int max_display = 0)
{
	//Compression rates to find the best result of ELA
	const int compression_rates[] = { 10 };
	//Vector as argument into the imwrite function
	displayImage(edges);
	vector<int> arg_vector;
	for (int cr : compression_rates)
	{
		arg_vector = { CV_IMWRITE_JPEG_QUALITY, cr };
		imwrite(TEMP_IMAGE_NAME, img, arg_vector);
		auto compressed = imread(TEMP_IMAGE_NAME);
		assert(!compressed.empty());
		// Calculating the difference between compressed and original jpg
		Mat ELA = Mat::zeros(cv::Size(img.cols, img.rows), CV_8UC1);

		FOR_(i, img.rows)
		{
			auto ip = img.ptr<uchar>(i), cp = compressed.ptr<uchar>(i), Ep = ELA.ptr<uchar>(i);
			FOR_(j, img.cols)
			{
				FOR_(k, 3)
				{
					Ep[j] += (uchar)(abs((int)ip[j * img.channels() + k] - cp[j * img.channels() + k]) * 5);
				}
			}
		}

		displayImage(ELA);
		// Getting fair ELA values based on the frequency of edges in the area in blocks of 16 * 16
		Mat realELA = Mat::zeros(cv::Size(img.cols / 8, img.rows / 8), CV_32FC1);
		FOR_(br, img.rows / 8) // Iterating through the blocks in the rows
		{
			FOR_(bc, img.cols / 8) // In the columns
			{
				int ELA_sum = 0, Edge_sum = 0;
				for (int x = br * 8; x < br * 8 + 8; x++)
				{
					auto Ep = ELA.ptr<uchar>(x), ep = edges.ptr<uchar>(x);
					for(int y = bc * 8; y < bc * 8 + 8; y++)
					{
						ELA_sum += Ep[y];
						if (ep[y])
							Edge_sum++;
					}
				}
				realELA.ptr<float>(br)[bc] = ELA_sum / (Edge_sum + 10);
				std::cout << realELA.ptr<float>(br)[bc] << std::endl;
				if (min_display && realELA.ptr<float>(br)[bc] < min_display && realELA.ptr<float>(br)[bc] != 0)
				{
					cv::rectangle(img, cv::Point(bc * 8, br * 8), cv::Point(bc * 8 + 7, br * 8 + 7), CV_RGB(100, 100, 255));
				}
				if (max_display && realELA.ptr<float>(br)[bc] > max_display)
				{
					cv::rectangle(img, cv::Point(bc * 8, br * 8), cv::Point(bc * 8 + 7, br * 8 + 7), CV_RGB(255, 100, 100));
				}
			}
		}
		if (min_display && max_display)
		{
			displayImage(img);
		}
	}
}

#endif