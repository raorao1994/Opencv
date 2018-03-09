// 基于凹点匹配的重叠分割.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>

using namespace std;
using namespace cv;

Mat src;
Mat searchConcaveRegions(vector<vector<Point> >hull, Mat &src);
vector<Point> searchConcavePoint(Mat &src);
Mat searchConcaveRegion(vector<Point>contours, Mat &src);
void Imgs(Mat thresholdimg, vector<Point> ps, Rect mr);

int main()
{
	Mat img = imread("img.jpg");
	src = imread("thresholdimg.jpg");
	//GaussianBlur(src, src, Size(3, 3),CV_8U);
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
		vector<Point> c = contours[i];
		if (PointCount > 50) {
			drawContours(src, contours, i, Scalar(0, 255, 0), 3);
			Rect mr = boundingRect(c);
			Imgs(distImg, c, mr);
		}
	}
	imshow("结果", src);
	waitKey(0);
	return 0;
}

//提取边缘，并重新处理
void Imgs(Mat thresholdimg, vector<Point> ps, Rect mr)
{
	//simpleBlobDetector  fitRetangle  fitEllipse
	Mat img2(thresholdimg, mr);
	//求凹面
	Mat aotu = searchConcaveRegion(ps, thresholdimg);
	vector<Point> pt;
	//计算分割点
	pt = searchConcavePoint(aotu);
	cout << "分割点识别的数量" << pt.size() << endl;
	//画出分割点
	if (pt.size() >= 2)
	{
		for (int i = 0; i < pt.size(); i+=2)
		{
			line(src, pt[i], pt[i + 1], Scalar(255, 0, 0), 2, 8, 0);
		}
	}
	//求边缘分割点index索引
	for (int i = 0; i < pt.size(); i++)
	{
		for (int j = 0; j < ps.size(); j++)
		{
			int xycout = (pt[i].x - ps[j].x);
		}
	}
}
/**
* @brief searchConcaveRegion 寻找凹区域
* @param hull  凸包点集
* @param src  原图像（二值图）
* @return 返回 图像凹区域
*/
Mat searchConcaveRegions(std::vector<std::vector<Point> >contours, Mat &src)
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
Mat searchConcaveRegion(vector<Point>contours, Mat &src)
{
	if (src.empty())
		return Mat();

	//遍历每个轮廓，寻找其凸包  
	vector<vector<Point>>_contours;
	_contours.push_back(contours);
	vector<vector<Point>>hull(_contours.size());
	convexHull(Mat(contours), hull[0], false);
	//绘制轮廓及其凸包  
	Mat drawing = Mat::zeros(src.size(), CV_8UC1);
	drawContours(drawing, hull, -1, Scalar(255, 0, 100), -1, 8, vector<Vec4i>(), 0, Point());
	drawContours(drawing, _contours, -1, Scalar(0, 255, 0), -1, 8, vector<Vec4i>(), 0, Point());
	//中值滤波
	//medianBlur(drawing, drawing, 3);
	//imshow("凹区域", drawing);
	return drawing;
}
/**
* @brief searchConcavePoint
* @param src 凹区域图像
* @return 返回匹配好的凹点对（2个）
*/
vector<Point> searchConcavePoint(Mat &src)
{
	//轮廓寻找    
	vector<vector<Point> > contour;//用来存储轮廓    
	vector<Vec4i> hierarchys;
	findContours(src, contour, hierarchys,
		CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));  //寻找轮廓    

	vector<Point> ConcavePoint;
	//凹区域少于2要退出    
	if (contour.size()<2)
		return ConcavePoint;

	vector<vector<Point> > _contours;
	for (int i = 0; i < contour.size(); i++)
	{
		double area = abs(contourArea(contour[i]));
		if (area < 3000)continue;
		_contours.push_back(contour[i]);
	}
	//按照轮廓面积大小排序    
	std::sort(_contours.begin(), _contours.end(), [](const std::vector<Point> &s1,
	const std::vector<Point> &s2) {
	double a1 = contourArea(s1);
	double a2 = contourArea(s2);
	return a1>a2;
	});
	//遍历凸面，计算最小点
	for (int a = 0; a < _contours.size(); a+=2)
	{
		int minDistance = 100000000;//最短距离  
		vector<Point> c1 = _contours[a];
		int b = a + 1;
		if (b == _contours.size())b = 0;
		vector<Point> c2 = _contours[b];
		Point p1, p2;
		//计算两凸面距离最小点
		for (int i = 0; i < c1.size(); ++i)
		{
			for (int j = 0; j<c2.size(); ++j)
			{
				//欧氏距离    
				int d = std::sqrt(std::pow((c1[i].x - c2[j].x), 2) +
					std::pow((c1[i].y - c2[j].y), 2));
				if (minDistance>d)
				{
					minDistance = d;
					p1 = c1[i];
					p2 = c2[j];
				}
			}
		}
		ConcavePoint.push_back(p1);
		ConcavePoint.push_back(p2);
	}
	return ConcavePoint;
}

