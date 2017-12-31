#pragma once
class Image_Histogram
{
private:
	int * bins;
	int max_bin_num;
	float bin_width;
	int zero_bin;
public:
	Image_Histogram(float max_num, float min_num, float bin_width);
	~Image_Histogram();
};

