// 椭圆识别.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv2/highgui/highgui.hpp"  
#include <iostream>  

using namespace cv;
using namespace std;

int main()
{
	string filename = "E:\\TablewareRecognition\\碗\\多碗-2\\微信图片_20180416144009.jpg";
	Mat src_image = imread(filename, 0);
	if (src_image.empty())
	{
		cout << "Couldn't open image!" << filename;
		return 0;
	}

	imshow("原图", src_image);

	//轮廓  
	vector<vector<Point>> contours;

	//使用canny检测出边缘  
	Mat edge_image;
	Canny(src_image, edge_image, 30, 70);
	imshow("canny边缘", edge_image);

	//边缘追踪，没有存储边缘的组织结构  
	findContours(edge_image, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	Mat cimage = Mat::zeros(edge_image.size(), CV_8UC3);

	for (size_t i = 0; i < contours.size(); i++)
	{
		//拟合的点至少为6  
		size_t count = contours[i].size();
		if (count < 16)
			continue;
		//double area = abs(contourArea(contours[i]));
		//if (area < 3600|| area>170000)
		//	continue;
		//椭圆拟合  
		RotatedRect box = fitEllipse(contours[i]);

		//如果长宽比大于30，则排除，不做拟合  
		if (MAX(box.size.width, box.size.height) > MIN(box.size.width, box.size.height) * 30)
			continue;
		if (MAX(box.size.width, box.size.height) <150)
			continue;

		//画出追踪出的轮廓  
		drawContours(cimage, contours, (int)i, Scalar::all(255), 1, 8);

		//画出拟合的椭圆  
		ellipse(cimage, box, Scalar(0, 0, 255), 1, CV_AA);
	}
	imshow("拟合结果", cimage);

	waitKey();
	return 0;
}

