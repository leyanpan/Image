#ifndef DETECTOBJECT_H
#define DETECTOBJECT_H

#include "Types.h"
#include <vector>
#include <algorithm>

using std::vector;
using cv::Mat;
using std::max;
using std::min;

const int MAX_LENGTH = 10;
const int MIN_DIST = 2;

vector<Image_Point> search_edge(Mat edges, Image_Point start)
{
	vector<Image_Point> trail{ start };
	bool dead = false;
	Image_Point lp, cp = start;
	int ll = -1;
	while (!trail.empty()) {
		int cl = -1;
		auto np = Image_Point();
		auto max_i = min(edges.size[0], cp.get_x() + MAX_LENGTH + 1);
		auto max_j = min(edges.size[1], cp.get_y() + MAX_LENGTH + 1);
		for (int i = max(0, cp.get_x() - MAX_LENGTH); i < max_i; ++i)
		{
			uchar * row = edges.ptr<uchar>(i);
			for (int j = max(0, cp.get_y() - MAX_LENGTH); j < max_j; ++j)
			{
				auto tp = Image_Point(i, j);
				if (i == cp.get_x() && j == cp.get_y())
					continue;
				if (i == start.get_x() && j == start.get_y() && trail.size() >= 4)
					return trail;
				auto tmp = (*(trail.end() - 1));
				if (*(row + edges.channels() * j) == 255 && (!lp || (lp.pointDistance(tp) > ll && lp.pointDistance(tp) > cp.pointDistance(tp))) && (!dead || (tmp.pointDistance(tp) >= MIN_DIST && tmp.pointDistance(cp) >= cp.pointDistance(tp))) && cp.pointDistance(tp) > cl)
				{
					np = tp;
					cl = cp.pointDistance(tp);
				}
			}
		}
		if (np)
		{
			if (dead)
			{
				trail.pop_back();
				dead = false;
			}
			trail.push_back(np);
			lp = cp;
			ll = cp.pointDistance(np);
			cp = np;
		}
		else
		{
			if (dead)
			{
				return trail;
			}
			dead = true;
			cp = lp;
			if (trail.size() >= 2)
				lp = *(trail.end() - 2);
			else
				return vector<Image_Point>();
			ll = lp.pointDistance(cp);
		}
	}
}

bool on_trail(Image_Point p, vector<Image_Point> trail)
{
	for (auto p2 : trail)
		if (p.pointDistance(p2) < 3 * MIN_DIST)
			return true;
	return false;
}

void delete_edge(Mat edge_mat, IL edges)
{
	for (auto i = edges.begin(); i < edges.end() - 1; i++)
	{
		Image_Point p1 = *i;
		Image_Point p2 = *(i + 1);
		for (int j = max(min(p1.get_x(), p2.get_x()) - 3,0); j <= max(p1.get_x(), p2.get_x()) + 3; j++)
		{
			auto p = edge_mat.ptr<uchar>(j);
			for (int k = max(min(p1.get_y(), p2.get_y()) - 3, 0); k <= max(p1.get_y(), p2.get_y()) + 3; k++)
			{
				p[k] = 0;
			}
		}
	}
}

auto detect_objects(Mat edges_arg)
{
	Mat edges;
	edges_arg.copyTo(edges);
	vector<IL> result;
	for (int i = 0; i < edges.size[0]; ++i)
	{
		uchar * row = edges.ptr<uchar>(i);
		for (int j = 0; j < edges.size[1]; ++j)
		{
			if (*(row + edges.channels() * j) == 255)
			{
				auto res = search_edge(edges, Image_Point(i, j));
				if (!res.empty())
				{
					result.push_back(res);
					delete_edge(edges, res);
				}
			}
		}
	}
	return result;
}
#endif
