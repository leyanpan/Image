#ifndef DCT_H
#define DCT_H

#include "Types.h"
#include "myutils.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using cv::Mat;
using cv::cvtColor;
using cv::split;
using cv::dct;
using cv::Rect;


using std::vector;
using std::cout;
using std::endl;
//too lazy to calculate the order of filling. Just type in manually...
const char* QOrder = "00011020110203122130403122130405142332415060514233241506071625344352617071625344352617273645546372736455463747566574756657677677";

int ReadQTables(std::string filename, array2D QTables[])
{
	std::ifstream image_in(filename, std::ios::binary);
	int cnt = 0;
	while (image_in)
	{
		unsigned char buf;
		image_in.read((char*)&buf, sizeof(char));

		if (buf == 0xff)
		{
			image_in.read((char*)&buf, sizeof(char));
			if (buf == 0xdb)
			{
				// null null null then we get the Q Tables
				image_in.read((char*)&buf, sizeof(char));
				image_in.read((char*)&buf, sizeof(char));
				image_in.read((char*)&buf, sizeof(char));

				Dynamic_alloc(QTables[cnt], 8, 8);
				for (int i = 0; i < 64; i++)
				{
					image_in.read((char*)&buf, sizeof(char));
					QTables[cnt][QOrder[2 * i] - '0'][QOrder[2 * i + 1] - '0'] = buf;
				}
				cnt++;
			}
		}
	}
	return cnt;
}

void DCT_check(Mat img, std::string filename)
{
	array2D QTables[2];
	int numq = ReadQTables(filename, QTables);

	Mat YUV;
	cvtColor(img, YUV, CV_BGR2YUV);
	vector<Mat> YUV_channel;
	split(YUV, YUV_channel);

	Mat Y, U, V;
	YUV_channel[0].convertTo(Y, CV_64FC1);
	YUV_channel[1].convertTo(U, CV_64FC1);
	YUV_channel[2].convertTo(V, CV_64FC1);


	Mat DCT_Y, DCT_U, DCT_V;
	Y.copyTo(DCT_Y);
	U.copyTo(DCT_U);
	V.copyTo(DCT_V);
	//TODO: Replace with Raw Data
	FOR_(i, img.cols / 8)
	{
		FOR_(j, img.rows / 8)
		{
			Rect Image_part(8 * i, 8 * j, 8, 8);
			dct(Y(Image_part), DCT_Y(Image_part));
			dct(U(Image_part), DCT_U(Image_part));
			dct(V(Image_part), DCT_V(Image_part));
			cout << (int)(DCT_Y(Image_part).ptr<double>(1)[1] + 0.5) / QTables[0][1][1] << " ";     
		}
	}
	displayImage(Y);
	for (int i = 0; i < numq; i++)
	{
		Dynamic_free(QTables[i], 8, 8);
	}
}

#endif