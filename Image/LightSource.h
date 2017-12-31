#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include "Types.h"
#include "myutils.h"
#include <numeric>
#include <cmath>
using std::vector;
using cv::Mat;
using std::max;
using std::sqrt;

struct MVector
{
	int x;
	int y;
	int o;
#define SEQUENCE(__x, __y, __o)\
if( __x >= __y && __y >= 0)\
{\
	x = __x;\
	y = __y;\
	o = __o;\
}
	void init(int _x, int _y)
	{
		SEQUENCE(_x, _y, 0)
		else SEQUENCE(_y, _x, 1)
		else SEQUENCE(_y, -_x, 2)
		else SEQUENCE(-_x, _y, 3)
		else SEQUENCE(-_x, -_y, 4)
		else SEQUENCE(-_y, -_x, 5)
		else SEQUENCE(-_y, _x, 6)
		else SEQUENCE(_x, -_y, 7)
	}

	MVector(Vector _v)
	{
		init(_v.x, _v.y);
	}

#define REVERSE(__o, __x, __y)\
case __o:\
	return origin + Vector(__x, __y);

	Image_Point reverse(Image_Point origin, int _x, int _y)
	{
		switch (o)
		{
			REVERSE(0, _x, _y)
			REVERSE(1, _y, _x)
			REVERSE(2, -_y, _x)
			REVERSE(3, -_x, _y)
			REVERSE(4, -_x, -_y)
			REVERSE(5, -_y, -_x)
			REVERSE(6, _y, -_x)
			REVERSE(7, _x, -_y)
		}
		return Image_Point();
	}
};



auto get_center(IL list)
{
	auto s = std::accumulate(list.begin(), list.end(), Image_Point(0, 0));
	return Image_Point(static_cast<int>(s.get_x() / list.size()), static_cast<int>(s.get_y() / list.size()));
}


auto fade_in(Image_Point p, Image_Point center)
{
	double length = p.length_to(center);
	return Image_Point(p.get_x() + static_cast<int>((center.get_x() - p.get_x()) * 3 / length), p.get_y() + static_cast<int>((center.get_y() - p.get_y()) * 3 / length));
}

auto get_norm(IL edge, int index)
{
	auto v = Vector(0, 0);
	for (int i = index - 5; i < index + 6; i++)
	{
		if (i == index)
			continue;
		auto t = (edge[(i + edge.size()) % edge.size()] - edge[index]).normalize();
		v = v + t;
	}
	return v.normalize();
}

#define BRIGHTNESS(r, g, b) (r * 0.299 + g * 0.587 + b * 0.114)

void plotLine(Image_Point p1, Image_Point p2, array2D target)
{
	auto v = p2 - p1;
	MVector mv(v);
	int D = 2 * mv.y - mv.x;
	int y = 0;
	for (int x = 0; x < mv.x; x++)
	{
		Image_Point pos = mv.reverse(p1, x, y);
		target[pos.get_x()][pos.get_y()] = 255;
		if (D > 0)
		{
			y++;
			D -= 2 * mv.x;
		}
		D += 2 * mv.y;
	}
}

auto vec3d(double ratio, Vector & v)
{
	if (ratio < 0.7)
		v.z = v.length() * sqrt(1 - (ratio - 0.12) * (ratio - 0.12)) / (ratio - 0.12);
	else if (ratio < 0.9)
		v.z = v.length() * ((ratio - 0.7) / 0.2 * sqrt(2));
	else
		v.z = -v.length() * ((ratio - 0.9) / 0.1 * sqrt(2));
}

auto getPatchCenter(Mat image, IL edge, array2D & full_edge)
{
	Mat greyImage;
	cv::cvtColor(image, greyImage, cv::COLOR_RGB2GRAY);
	Dynamic_alloc(full_edge, image.size[0], image.size[1]);
	double avg_brightness = 0;
	double max_brightness = -1;
	int n = 0;
	for (auto i = (edge.begin() + 1); i < edge.end(); i++) 
		plotLine(*(i - 1), *i, full_edge);
	plotLine(*(edge.end() - 1), *edge.begin(), full_edge);
	for (int i = 0; i < greyImage.size[0]; i++)
	{
		bool should_fill = false;
		auto ptr = greyImage.ptr<uchar>(i);
		for (int j = 0; j < greyImage.size[1]; j++)
		{
			if (full_edge[i][j] == 255 && (!(j != 0 && full_edge[i][j - 1] == 255)))
				should_fill = !should_fill;
			if (should_fill)
			{
				full_edge[i][j] = 255;
				auto b = static_cast<double>(*(ptr + j));
				n++;
				avg_brightness += (b - avg_brightness) / n;
				max_brightness = max(b, max_brightness);
			}
		}
	}
	auto threshhold = static_cast<uchar>((avg_brightness + max_brightness * 4) / 5);
	double midx = 0, midy = 0;
	n = 0;
	for (int i = 0; i < greyImage.size[0]; i++)
	{
		auto ptr = greyImage.ptr<uchar>(i);
		for (int j = 0; j < greyImage.size[1]; j++)
		{
			if (full_edge[i][j] != 255)
				continue;
			auto b = *(ptr + j);
			if (b > threshhold)
			{
				n++;
				midx += static_cast<double>(i - midx) / n;
				midy += static_cast<double>(j - midy) / n;
			}
		}
	}
	return Image_Point(static_cast<int>(midx), static_cast<int>(midy));
}

auto matrixAlgorithm(Mat img, IL edge, Image_Point center)
{
	Mat M, b, brightness;
	M = Mat::zeros(cv::Size(edge.size(), 3), CV_32F);
	b = Mat::zeros(cv::Size(edge.size(), 1), CV_32F);
	cvtColor(img, brightness, CV_BGR2GRAY);
	FOR_(i, edge.size())
	{
		auto bp = fade_in(edge[i], center);
		auto norm = get_norm(edge, i);
		M.ptr<float>(0)[i] = -norm.get_x();
		M.ptr<float>(1)[i] = -norm.get_y();
		M.ptr<float>(2)[i] = 1;
		b.ptr<float>(0)[i] = (float)(*(get(brightness, bp)));
	}
	Mat t1, t2;
	cv::mulTransposed(M, t1, false);
	t2 = t1.inv();

	Mat mv = Mat::zeros(cv::Size(40, 3), CV_32F);
	mv = t2 * M;
	mv *= b.t();
	return Vector(mv.ptr<float>()[0], mv.ptr<float>()[1]);
}

double len_direction(array2D content, Vector v, Image_Point center)
{
	MVector mv(v);
	int D = 2 * mv.y - mv.x;
	int y = 0;
	for (int x = 0; ; x++)
	{
		Image_Point pos = mv.reverse(center, x, y);
		if (content[pos.get_x()][pos.get_y()] != 255)
			return ((double)v.get_x()) / (pos.get_x() - center.get_x());
		if (D > 0)
		{
			y++;
			D -= 2 * mv.x;
		}
		D += 2 * mv.y;
	}
}


#endif