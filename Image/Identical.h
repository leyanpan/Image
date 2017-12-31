#ifndef IDENTICAL_H
#define IDENTICAL_H

#include "Types.h"
#include "myutils.h"
#include <string.h>
#include <numeric>
#include <unordered_map>
#include <iostream>
using cv::Mat;
using std::unordered_map;
int SQUARE_LENGTH;
int SQUARE_A;
bool identical_square(Mat img, Image_Point pos1, Image_Point pos2)
{
	static uchar ** p = nullptr;     
	if (p == nullptr)
	{
		p = new uchar*[img.size[0]];
		for (int i = 0; i < img.size[0]; i++)
			p[i] = img.ptr<uchar>(i);
	}
	for (int i = 0; i < SQUARE_A; ++i)
	{
		if (memcmp(p[pos1.get_x() + i] + img.channels() * pos1.get_y(), p[pos2.get_x() + i] + img.channels() * pos2.get_y(), SQUARE_A * sizeof(uchar)) != 0)
			return false;
	}
	return true;
}

int check_identical(Mat img)
{
	SQUARE_LENGTH = min(10, (img.size[0] + img.size[1]) / 400);
	SQUARE_A = (2 * SQUARE_LENGTH) + 1;
	if (img.size[0] < 2 * SQUARE_A || img.size[1] < 2 * SQUARE_A)
		return -1;
	array2D sum;
	Dynamic_alloc(sum, img.size[0], img.size[1]);
	for (int i = 0; i < img.size[0]; ++i)
	{
		auto p = img.ptr<uchar>(i);
		int start_sum = std::accumulate(p, (uchar*)(p + (SQUARE_A * img.channels())), 0);
		sum[i][0] = start_sum;
		for (int j = 1; j < img.size[1] - SQUARE_A; ++j)
		{
			sum[i][j] = sum[i][j - 1] - std::accumulate(p + (j - 1) * img.channels(), p + j * img.channels(), 0) + std::accumulate(p + (j + SQUARE_A - 1) * img.channels(), p + (j + SQUARE_A) * img.channels(), 0);
		}
	}
	array2D sum2 = nullptr;
	Dynamic_alloc(sum2, img.size[0], img.size[1]);
	for (int i = 0; i < img.size[1] - SQUARE_A; ++i)
	{
		int start_sum = 0;
		for (int j = 0; j < SQUARE_A; j++)
		{
			start_sum += sum[j][i];
		}
		sum2[0][i] = start_sum;
		for (int j = 1; j < img.size[0] - SQUARE_A; ++j)
		{
			sum2[j][i] = sum2[j - 1][i] - sum[j - 1][i] + sum[j + SQUARE_A - 1][i];
		}
	}
	Dynamic_free(sum, img.size[0], img.size[1]);
	unordered_map<int, IL> map;
	int cnt = 0;
	int maxi = img.size[0] * img.size[1] / 200;
	for (int i = 0; i < img.size[0] - SQUARE_A; i++)
	{
		for (int j = 0; j < img.size[1] - SQUARE_A; j++)
		{
			if (sum2[i][j] == 255 * SQUARE_A * SQUARE_A * 3)
			{
				//cnt++;
				continue;
			}
			if (sum2[i][j] > (253 * SQUARE_A * SQUARE_A * 3))
				continue;
			if (map.find(sum2[i][j]) == map.end())
			{
				(map[sum2[i][j]]).push_back(Image_Point(i, j));
				continue;
			}

			bool hasIdentical = false;
			for (auto it = map[sum2[i][j]].begin(); it < map[sum2[i][j]].end(); it++)
			{
				if (identical_square(img, *it, Image_Point(i, j)))
				{
					if (++cnt > maxi)
						return cnt;
					 
					break;
				}
			}
			if (!hasIdentical)
				map[sum2[i][j]].push_back(Image_Point(i, j));
		}
	}
	Dynamic_free(sum2, img.size[0], img.size[1]);
	return cnt;
}

#endif