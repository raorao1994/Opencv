// 基于凹点匹配的重叠分割.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>

using namespace std;
using namespace cv;

Mat searchConcaveRegion(std::vector<std::vector<Point> >hull, Mat &src);
std::vector<Point2f> searchConcavePoint(Mat &src);
void Imgs(Mat thresholdimg, vector<Point> ps, Rect mr);


int main()
{
	Mat img = imread("img.jpg");
	Mat src = imread("thresholdimg.jpg");
	Mat grayImg;
	cvtColor(src, grayImg, CV_BGR2GRAY);
	Mat distImg;
	threshold(grayImg, distImg, 200, 255, THRESH_BINARY);
	vector<vector<Point>> contours;
	//寻找轮廓，省去层次结构，检索外部轮廓，轮廓拐点近似
	findContours(distImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	for (size_t i = 0; i < contours.size(); i++) {
		double area = abs(contourArea(contours[i]));
		if (area < 3500)continue;
		int PointCount = contours[i].size();
		cout << "轮廓" << i << "的数量：" << PointCount << endl;
		vector<Point> c = contours[i];
		if (PointCount > 50) {
			drawContours(src, contours, i, Scalar(0, 255, 0), 3);
			Rect mr = boundingRect(c);
			//fillPoly(grayImg, c, Scalar(255, 0, 255));
			Imgs(distImg, c, mr);

			vector<vector<Point>> _contours(1);
			_contours.push_back(c);

			Mat aotu = searchConcaveRegion(_contours, distImg);
			vector<Point2f> pt;
			pt = searchConcavePoint(aotu);
			if(pt.size()>=2)
				line(src, pt[pt.size() - 2], pt[pt.size() - 1], Scalar(255, 0, 0), 2, 8, 0);
		}
		
	}
	imshow("结果", src);
	imshow("多边形", grayImg);
	waitKey(0);
	return 0;
}

//提取边缘，并重新处理
void Imgs(Mat thresholdimg, vector<Point> ps, Rect mr)
{
	//simpleBlobDetector  fitRetangle  fitEllipse
	Mat img2(thresholdimg, mr);

	
}
/**
* @brief searchConcaveRegion 寻找凹区域
* @param hull  凸包点集
* @param src  原图像（二值图）
* @return 返回 图像凹区域
*/
Mat searchConcaveRegion(std::vector<std::vector<Point> >contours, Mat &src)
{
	if (src.empty())
		return Mat();

	//遍历每个轮廓，寻找其凸包  
	vector<vector<Point>>hull(contours.size());
	for (unsigned int i = 0; i<contours.size(); ++i)
	{
		convexHull(Mat(contours[i]), hull[i], false);
	}

	//绘制轮廓及其凸包  
	Mat drawing = Mat::zeros(src.size(), CV_8UC1);
	for (unsigned int i = 0; i < contours.size(); ++i)
	{
		drawContours(drawing, hull, i, Scalar(255,0,100), -1, 8, vector<Vec4i>(), 0, Point());
		drawContours(drawing, contours, i, Scalar(0,255,0), -1, 8, vector<Vec4i>(), 0, Point());
	}

	medianBlur(drawing, drawing, 3);

	imshow("凹区域", drawing);

	return drawing;
}

/**
* @brief searchConcavePoint
* @param src 凹区域图像
* @return 返回匹配好的凹点对（2个）
*/
std::vector<Point2f> searchConcavePoint(Mat &src)
{
	std::vector<Point2f> ConcavePoint;
	//轮廓寻找    
	std::vector<std::vector<Point> > contour;//用来存储轮廓    
	std::vector<Vec4i> hierarchys;
	findContours(src, contour, hierarchys,
		CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));  //寻找轮廓    

	//凹区域少于2要退出    
	if (contour.size()<2)
		return ConcavePoint;

	//按照轮廓面积大小排序    
	std::sort(contour.begin(), contour.end(), [](const std::vector<Point> &s1,
		const std::vector<Point> &s2) {
		double a1 = contourArea(s1);
		double a2 = contourArea(s2);
		return a1>a2;
	});

	int minDistance = 100000000;//最短距离  
	cout << "总共多少个" << contour[0].size() << endl;
	for (int i = 0; i<contour[0].size(); ++i)
		for (int j = 0; j<contour[1].size(); ++j)
		{
			//欧氏距离    
			int d = std::sqrt(std::pow((contour[0][i].x - contour[1][j].x), 2) +
				std::pow((contour[0][i].y - contour[1][j].y), 2));
			if (minDistance>d)
			{
				minDistance = d;
				ConcavePoint.push_back(contour[0][i]);
				ConcavePoint.push_back(contour[1][j]);
			}
		}
	cout << "ConcavePoint0:" << ConcavePoint[ConcavePoint.size() - 2].x << "," << ConcavePoint[ConcavePoint.size() - 2].y << endl;
	cout << "ConcavePoint1:" << ConcavePoint[ConcavePoint.size() - 1].x << "," << ConcavePoint[ConcavePoint.size() - 1].y << endl;

	return ConcavePoint;
}

