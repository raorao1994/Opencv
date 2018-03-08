// 连接边缘分割.cpp : 定义控制台应用程序的入口点。

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>

using namespace std;
using namespace cv;

int main()
{
	Mat src = imread("3.jpg");
	Mat grayImg;
	cvtColor(src, grayImg, CV_BGR2GRAY);
	Mat distImg;
	threshold(grayImg, distImg, 200, 255, THRESH_BINARY);
	vector<vector<Point>> contours;
	//寻找轮廓，省去层次结构，检索外部轮廓，轮廓拐点近似
	findContours(distImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); 
	drawContours(src, contours, -1, Scalar(255, 0, 255),5);
	for (size_t i = 0; i < contours.size(); i++) {
		int PointCount = contours[i].size();
		cout << "轮廓"<<i<<"的数量：" << PointCount << endl;
		vector<Point> c = contours[i];
		if (PointCount > 50) {
			fillPoly(grayImg, c, Scalar(255, 0, 255));
		}
	}
	imshow("结果", src);
	imshow("多边形", grayImg);
	waitKey(0);
    return 0;
}

