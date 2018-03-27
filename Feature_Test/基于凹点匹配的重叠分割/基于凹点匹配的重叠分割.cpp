// 基于凹点匹配的重叠分割.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>

using namespace std;
using namespace cv;

int  convexArea=1000;//凸面边缘面积阀值
int AmaxArea = 3600;//计算分割下的边缘面积阈值
int Pixthreshold = 6;//像素点距离阈值
Mat src;
vector<Point> searchConcavePoint(Mat &src);
Mat searchConcaveRegion(vector<Point>contours, Mat &src);
vector<vector<Point>> Overlapping_Edge_Segmentation(Mat thresholdimg, vector<Point> ps, Rect mr);
vector<vector<Point>> RegionSort(vector<vector<Point>> contours);
int main()
{
	Mat img = imread("img.jpg");
	src = imread("thresholdimg1.jpg");
	//GaussianBlur(src, src, Size(3, 3),CV_8U);
	Mat grayImg;
	cvtColor(src, grayImg, CV_BGR2GRAY);
	Mat distImg;
	threshold(grayImg, distImg, 200, 255, THRESH_BINARY);
	//膨胀操作

	vector<vector<Point>> contours;
	//寻找轮廓，省去层次结构，检索外部轮廓，轮廓拐点近似
	findContours(distImg, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);//RETR_LIST RETR_EXTERNAL
	for (size_t i = 0; i < contours.size(); i++) {
		double area = abs(contourArea(contours[i]));
		if (area < 3500)continue;
		vector<Point> c = contours[i];
		//drawContours(src, contours, i, Scalar(0, 255, 0), 3);
		Rect mr = boundingRect(c);
		vector<vector<Point>> cc= Overlapping_Edge_Segmentation(distImg, c, mr);
		//drawContours(src, cc, -1, Scalar(0, 255, 0), 3);
	}
	imshow("结果", src);
	waitKey(0);
	return 0;
}


/**
* @brief searchConcaveRegion 寻找凹区域
* @param hull  凸包点集
* @param src  原图像（二值图）
* @return 返回 图像凹区域
*/
Mat searchConcaveRegion(vector<Point>contours, Mat &src)
{
	if (src.empty())
		return Mat();
	vector<Point> poly;
	//对图像轮廓点进行多边形拟合
	approxPolyDP(Mat(contours), poly, 1, true);//根据点集，拟合出多边形
	//遍历每个轮廓，寻找其凸包  
	vector<vector<Point>>_contours;
	_contours.push_back(contours);
	vector<vector<Point>>hull(_contours.size());
	convexHull(Mat(contours), hull[0], false);
	//绘制轮廓及其凸包  
	Mat drawing = Mat::zeros(src.size(), CV_8UC1);
	drawContours(drawing, hull, -1, Scalar(255, 0, 100), -1, 8, vector<Vec4i>(), 0, Point());
	drawContours(drawing, _contours, -1, Scalar(0, 255, 0), -1, 8, vector<Vec4i>(), 0, Point());
	//腐蚀操作
	Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
	erode(drawing, drawing, element);
	imshow("凸面",drawing);
	//释放内存资源
	poly.capacity();
	_contours.capacity();
	element.release();
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
	vector<vector<Point>> contour;//用来存储轮廓    
	vector<Vec4i> hierarchys;
	findContours(src, contour, hierarchys,
		CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));  //寻找轮廓    
	//获取凸面边缘
	vector<Point> ConcavePoint;
	//凹区域少于2要退出    
	if (contour.size()<2)
		return ConcavePoint;
	//按照轮廓面积大小排序 
	vector<vector<Point>> _contours=RegionSort(contour);
	/*sort(_contours.begin(), _contours.end(), [](const vector<Point> &s1,
		const std::vector<Point> &s2) {
		double a1 = contourArea(s1);
		double a2 = contourArea(s2);
		return a1>a2;
	});*/
	//遍历凸面，计算最小点
	for (int a = 0; a < _contours.size(); a++)
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
	//过滤无效分割点
	vector<Point> _ConcavePoint;
	int ptSize = ConcavePoint.size();
	if (ptSize > 2)
	{
		//找到重合点
		for (int i = 0; i <ptSize; i += 2)
		{
			bool isok = true;
			int upIndex = i - 1;
			if (upIndex < 0)upIndex = ptSize - 1;
			int nextIndex = i + 2;
			if (nextIndex >= ptSize)nextIndex = 0;
			int _pixDic = abs(ConcavePoint[i].x - ConcavePoint[upIndex].x) + abs(ConcavePoint[i].y - ConcavePoint[upIndex].y);
			if (_pixDic < Pixthreshold)
			{
				_pixDic = abs(ConcavePoint[i + 1].x - ConcavePoint[nextIndex].x) + abs(ConcavePoint[i + 1].y - ConcavePoint[nextIndex].y);
				if (_pixDic < Pixthreshold) { 
					isok = false;
					continue;
				}
			}
			_pixDic = abs(ConcavePoint[i].x - ConcavePoint[nextIndex].x) + abs(ConcavePoint[i].y - ConcavePoint[nextIndex].y);
			if (_pixDic < Pixthreshold)
			{
				_pixDic = abs(ConcavePoint[i + 1].x - ConcavePoint[upIndex].x) + abs(ConcavePoint[i + 1].y - ConcavePoint[upIndex].y);
				if (_pixDic < Pixthreshold) {
					isok = false;
					continue;
				}
			}
			if (isok) {
				_ConcavePoint.push_back(ConcavePoint[i]);
				_ConcavePoint.push_back(ConcavePoint[i + 1]);
			}
		}
	}
	return _ConcavePoint;
}
//提取边缘，并重新处理
vector<vector<Point>> Overlapping_Edge_Segmentation(Mat thresholdimg, vector<Point> ps, Rect mr)
{
	//求凹面
	Mat aotu = searchConcaveRegion(ps, thresholdimg);
	//计算分割点
	vector<Point> pt = searchConcavePoint(aotu);
	int ptSize = pt.size();
	//如果不存在凹点
	if (ptSize < 2)
	{
		vector<vector<Point>> _ps;
		_ps.push_back(ps);
		return _ps;
	}
	//按循序找到边缘分割点的index索引
	vector<int> indexList;
	int MaxIndex = 0, MinIndex = ps.size();
	for (int i = 0; i < ptSize; i++)
	{
		for (int j = 0; j < ps.size(); j++)
		{
			int _xycout = abs((pt[i].x - ps[j].x)) + abs((pt[i].y - ps[j].y));
			if (_xycout <= Pixthreshold) {
				indexList.push_back(j);
				if (j > MaxIndex) MaxIndex = j;
				if (j < MinIndex) MinIndex = j;
				break;
			}
		}
	}
	//最终得到的分割结果
	vector<vector<Point>> PointList;
	//遍历分割点对应的索引，并分割边缘点
	for (int i = 0; i < indexList.size(); i += 2)
	{
		//在原图上画分割线
		line(src, pt[i], pt[i + 1], Scalar(255));

		int index0 = indexList[i];//小
		int index1 = indexList[i + 1];//大
		if (index0 > index1) {
			index0 = index1;
			index1 = indexList[i];
		}
		//1、分割点横跨0点的时候
		vector<Point> _p;
		if (index0 == MinIndex&&index1 == MaxIndex)
		{
			for (index1; index1 < ps.size() + index0; index1++)
			{
				int _index = index1;
				if (_index >= ps.size())
					_index = _index - ps.size();
				_p.push_back(ps[_index]);
				ps[index0] = _p[0];
			}
			if (AmaxArea < abs(contourArea(_p)))
				PointList.push_back(_p);
			continue;
		}
		//2、分割点不横跨0点的时候
		else
		{
			for (index0; index0 < index1; index0++)
			{
				_p.push_back(ps[index0]);
				ps[index0] = ps[index1];
			}
			if (AmaxArea < abs(contourArea(_p)))
				PointList.push_back(_p);
			continue;
		}
	}
	if (AmaxArea < abs(contourArea(ps)))
		PointList.push_back(ps);
	return PointList;
}
//区域排序
//根据顺时针方向，对凸面进行排序，并清除面积小的部分
vector<vector<Point>> RegionSort(vector<vector<Point>> contours)
{
	vector<Point3d> xindex;
	int averageY = 0;
	//过滤较小的凸面区域，并计算出他的中心点
	for (int i = 0; i < contours.size(); i++)
	{
		double area = abs(contourArea(contours[i]));
		if (area < convexArea) continue;
		long x=0,y=0;
		for (size_t j = 0; j < contours[i].size(); j++)
		{
			x += contours[i][j].x;
			y += contours[i][j].y;
		}
		x = x / contours[i].size();
		y = y / contours[i].size();
		xindex.push_back(Point3d(x,y,i));
		averageY += y;
	}
	averageY = averageY / xindex.size();
	//从小到大
	sort(xindex.begin(), xindex.end(), [](const Point3d &p1,
		const Point3d &p2) {
		bool b = false;
		if (p1.x < p2.x)b = true;
		if (p1.x == p2.x&&p1.y > p2.y)b = true;
		return b;
	});
	vector<vector<Point>> _contours(xindex.size());
	int _Maxindex = xindex.size() - 1;
	int _Minindex = 0;
	for (int i = 0; i < xindex.size(); i++)
	{
		if (xindex[i].y > averageY) 
		{
			_contours[_Maxindex] = contours[xindex[i].z];
			_Maxindex--;
			
		}
		else
		{
			_contours[_Minindex] = contours[xindex[i].z];
			_Minindex++;
		}
	}
	return _contours;
}

