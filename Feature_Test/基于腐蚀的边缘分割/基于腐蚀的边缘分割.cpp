// 基于腐蚀的边缘分割.cpp : 定义控制台应用程序的入口点。
#include "stdafx.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
	Mat src = imread("thresholdimg.jpg");
	Mat grayImg;
	cvtColor(src, grayImg, CV_BGR2GRAY);
	/* 1.距离变化的二值化 binary again，各对象孤立*/
	Mat thresholdimg;
	threshold(grayImg, thresholdimg, 200, 255, THRESH_BINARY); //全局阈值0.4-1
	imshow("二值化", thresholdimg);
	vector<vector<Point>> contours;
	findContours(thresholdimg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	Mat drawing = Mat::zeros(thresholdimg.size(), CV_8UC1);
	//for (size_t i = 0; i < contours.size(); i++)
	//{

	//	double area = abs(contourArea(contours[i]));
	//	if (area < 3600)continue;
	//	//FindHull(contours[i],drawing);
	//	vector<Point> poly;
	//	//对图像轮廓点进行多边形拟合
	//	approxPolyDP(Mat(contours[i]), poly, 1, true);//根据点集，拟合出多边形
	//	//fillPoly(drawing, Mat(poly), Scalar(255)); // 根据点集，绘制出多边形
	//	fillConvexPoly(drawing, Mat(contours[i]), Scalar(255)); //根据点集，绘制并填充出多边形
	//}

	/* 2.腐蚀，得到各个高峰 */
	Mat k1 = Mat::ones(5, 5, CV_8UC1); //结构元素全是1,结构的大小影响分割效果
	erode(thresholdimg, thresholdimg, k1, Point(-1, -1)); //腐蚀
	erode(thresholdimg, thresholdimg, k1, Point(-1, -1)); //腐蚀
	erode(thresholdimg, thresholdimg, k1, Point(-1, -1)); //腐蚀
	erode(thresholdimg, thresholdimg, k1, Point(-1, -1)); //腐蚀
	erode(thresholdimg, thresholdimg, k1, Point(-1, -1)); //腐蚀
	imshow("腐蚀", thresholdimg);
	waitKey(0);
    return 0;
}

