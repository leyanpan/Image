#include "Types.h"
#include "myutils.h"
#include "DetectObject.h"
#include "Identical.h"
#include "LightSource.h"
#include "ELA.h"
#include "DCT.h"
#include "noise.h"

#include <windows.h>
#include <iostream>
#include <string>
using std::cout;

using cv::imread;
using cv::line;
using cv::imshow;
using cv::waitKey;
using cv::destroyAllWindows;
using cv::Mat;


void displayVector(Mat img, Image_Point center, Vector v)
{
	line(img, center.toCV(), (center + v).toCV(), CV_RGB(255, 0, 0));
	displayImage(img);
}

int main(int argc, char * argv[])
{
	Mat image = imread(argv[1]);
	displayImage(image);
	Mat blurred, edges;
	cv::GaussianBlur(image, blurred, cv::Size(3, 3), 0);
	cv::Canny(blurred, edges, 50, 150);
	/*for (int i = 5; i < 360; i += 5)
	{
		Mat image = imread("C:\\Users\\pxb07\\Desktop\\Computer\\Programming\\Ray Tracing\\" + std::to_string(i) + ".png");
		Mat image2 = imread("C:\\Users\\pxb07\\Desktop\\Computer\\Programming\\Ray Tracing\\tst\\" + std::to_string(i) + "_sphere.png");
		Mat blurred, edges, edgesblurred, edges2;
		cv::Canny(image, edges, 50, 120);
		cv::Canny(image2, edges2, 50, 120);
		displayImage(edges);
		int64 startt = ::GetTickCount();
		auto edge_IL = detect_objects(edges);
		auto edge_IL2 = detect_objects(edges2);
		for (auto edge : edge_IL)   
		{
			array2D content = nullptr;
			auto pcenter = getPatchCenter(image, edge, content);
			auto center = get_center(edge);
			auto center2 = get_center(edge_IL2[0]);
			auto ratio = len_direction(content, pcenter - center, center);
			auto v = pcenter - center;
			auto v2 = matrixAlgorithm(image, edge, center).normalize();
			auto v3 = matrixAlgorithm(image2, edge_IL2[0], center2).normalize();
			cout << v.angleFrom(v3) << endl;
			cout << v2.angleFrom(v3) << std::endl;
			Dynamic_free(content, image.size[0], image.size[1]);
		}*/
	/*if (argc >= 4)
		ELA_analysis(image, edges, std::atoi(argv[2]), std::atoi(argv[3]));
	else
		ELA_analysis(image, edges);*/
	DCT_check(image, argv[1]);
	return 0;
}