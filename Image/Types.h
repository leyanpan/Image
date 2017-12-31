#ifndef TYPES_H
#define TYPES_H

#include <cv.hpp>
#include <vector>
#include <cmath>

using cv::Point;
using std::sqrt;
using cv::Mat;

class Vector;

class Image_Point
{
private:
	int x;
	int y;
public:
	Image_Point(int _x, int _y) : x(_x), y(_y) {}
	Image_Point():x(-1), y(-1){}
	bool operator == (Image_Point other);
	auto operator + (Image_Point other);
	auto operator + (Vector other);
	double length_to(Image_Point other);
	auto operator - (Image_Point other);
	operator bool();
	auto pointDistance(Image_Point other);
	auto toCV();
	int get_x();
	int get_y();
};

class Vector
{
private:
	double x;
	double y;
public:
	double z = 0;
	friend class Image_Point;
	friend class MVector;
	Vector(double _x, double _y) : x(_x), y(_y){}
	Vector(int _x, int _y) : x(static_cast<double>(_x)), y(static_cast<double>(_y)){}
	Vector() : x(-1), y(-1) {}
	bool operator == (Vector other);
	auto operator + (Vector other);
	auto operator * (int n);
	double length();
	auto normalize();
	auto floor();
	double get_x();
	double get_y();
	double angleFrom(Vector v2);
};

typedef std::vector<Image_Point> IL;

bool Image_Point::operator == (Image_Point other)
{
	if (x == -1)
		return false;
	return x == other.x && y == other.y;
}

auto Image_Point::operator- (Image_Point other)
{
	return Vector(x - other.x, y - other.y);
}

inline Image_Point::operator bool()
{
	return x != -1;
}

auto Image_Point::operator+ (Image_Point other)
{
	return Image_Point(x + other.x, y + other.y);
}

auto Image_Point::operator+ (Vector other)
{
	return Image_Point(x + other.x, y + other.y);
}

double Image_Point::length_to(Image_Point other)
{
	return sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
}

auto Image_Point::pointDistance(Image_Point other)
{
	return abs(x - other.x) + abs(y - other.y);
}

auto Image_Point::toCV()
{
	return Point(y, x);
}

inline int Image_Point::get_x()
{
	return x;
}

inline int Image_Point::get_y()
{
	return y;
}

auto Vector::floor()
{
	return Vector(std::floor(x), std::floor(y));
}

inline double Vector::get_x()
{
	return x;
}


inline double Vector::get_y()
{
	return y;
}

inline double Vector::angleFrom(Vector v2)
{
	return std::acos((x * v2.x + y * v2.y) / (length() * v2.length())) / 3.1415926535897932 * 180;
}
double Vector::length()
{
	return sqrt(x * x + y * y);
}

auto Vector::normalize()
{
	double l = length();
	return Vector(x / l, y / l);
}

auto Vector::operator + (Vector other)
{
	return Vector(x + other.x, y + other.y);
}

auto Vector::operator * (int n)
{
	return Vector(x * n, y * n);
}

bool Vector::operator == (Vector other)
{
	if (x == -1)
		return false;
	return x == other.x && y == other.y;
}

uchar* get(Mat mat, Image_Point pos)
{
	auto p = mat.ptr<uchar>(pos.get_x());
	return (p + (pos.get_y() * mat.channels()));
}
#endif // !TYPES_H


