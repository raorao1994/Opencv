// 凹缺陷_凸缺陷.cpp : 定义控制台应用程序的入口点。
//http://blog.csdn.net/jjddss/article/details/73548564
//http://blog.csdn.net/Lu597203933/article/details/14532053  图像和轮廓的匹配(hu矩)
//http://blog.csdn.net/keith_bb/article/details/70197104 图像的矩moments
//http://blog.sina.com.cn/s/blog_662c785901011i7z.html  好文章
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

/*
*Angle 角度计算
*cen 中间点
*first 起点
*second结点
*起点->中间点->结点
*/
double Angle(Point cen, Point first, Point second)
{

	const double M_PI = 3.1415926535897;
	double ma_x = first.x - cen.x;
	double ma_y = first.y - cen.y;
	double mb_x = second.x - cen.x;
	double mb_y = second.y - cen.y;
	double v1 = (ma_x * mb_x) + (ma_y * mb_y);
	double ma_val = sqrt(ma_x * ma_x + ma_y * ma_y);
	double mb_val = sqrt(mb_x * mb_x + mb_y * mb_y);
	double cosM = v1 / (ma_val * mb_val);
	double acosV = acos(cosM);
	double angleAMB = acosV * 180.0 / M_PI;

	//计算是否为大于180度的角
	if (cen.x == first.x)
	{
		//cen和first两点构成的直线垂直于y轴
		if (second.x > cen.x)
			return (360 - angleAMB);
		else
			return angleAMB;
	}
	else
	{
		double k = (cen.y - first.y) / (cen.x - first.x);
		double b = first.y - first.x*k;
		double _y = second.x*k + b;
		if(second.y>=_y)
			return angleAMB;
		else
			return (360 - angleAMB);
	}
}
//寻找凸点
void FindHull(vector<Point> contour,Mat drawing)
{
	vector<Point> hull;
	convexHull(Mat(contour), hull, false);
	vector<Point> poly;
	//对图像轮廓点进行多边形拟合
	approxPolyDP(Mat(hull), poly, 1, true);//根据点集，拟合出多边形
	fillPoly(drawing, Mat(poly), Scalar(255)); // 根据点集，绘制出多边形
}
//寻找矩
void FindMoments(vector<Point> contour,Mat drawing)
{
	//计算图像矩
	Moments mu;
	mu = moments(contour, false);
	//计算图像的质心
	Point mc;
	//Point2f mc;
	mc = Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00);
	circle(drawing, mc, 5, Scalar(255), 2);
}
//角点监测
void cornerShiTomasi(Mat img)
{
	//输入参数：
	//	InputArray _image：输入的图像，InputArray类型一般为Mat；
	//	int maxCorners：可检测的角点的最大值
	//	double qualityLevel：角点特征值的门限，高于(该门限*特征值)最大值才认为是角点。
	//	double minDistance：角点间最小间距
	//	InputArray _mask：掩码，用于设置不处理的区域
	//	int blockSize：处理的窗口大小
	//	bool useHarrisDetector：是否使用harris角点检测
	//	double harrisK：harris角点检测参数
	//	输出参数：
	//	OutputArray _corners：输出的角点，OutputArray类型一般为Mat；

	int minDis = 2;
	vector<Point2f> corner;
	goodFeaturesToTrack(img, corner, 270, 0.4, minDis, Mat(), 5, false, 0);//0.04
	for (int i = 0; i<corner.size(); i++) {
		circle(img, Point(corner.at(i).x, corner.at(i).y), 5, Scalar(255));
	}
	cout << "关键点点数" << corner.size() << endl;
}
//特征点
vector<Point> FindKeyPoint(vector<Point> pList,Mat img)
{
	vector<Point> _KeyPointList;
	for (size_t i = 0; i < pList.size(); i++)
	{
		Point cen = pList[i];
		Point first;
		Point second;
		if(i<=0)
			first = pList[pList.size() - 1];
		else
			first = pList[i - 1];
		if ((i+1) >= pList.size())
			second = pList[0];
		else
			second = pList[i + 1];
		//判断节点是否重合
		/*if ((second.x == cen.x && cen.y == second.y) || (first.x == cen.x && cen.y == first.y))
		{
			_KeyPointList.push_back(cen);
			circle(img, cen, 5, Scalar(255));
			continue;
		}*/
		double angle = Angle(cen, first, second);
		if (angle > 200)
		{
			_KeyPointList.push_back(cen);
			circle(img, cen, 5, Scalar(255));
		}
		line(img, cen, Point(cen.x+10,cen.y+10), Scalar(255));
	}
	return _KeyPointList;
}

int main()
{
	Mat img = imread("thresholdimg.jpg");
	Mat img_gray;
	cvtColor(img, img_gray, CV_BGR2GRAY);
	//先进行canny
	Mat edges;
	Canny(img_gray, edges, 200, 250);
	Mat img_threshold;
	threshold(img_gray, img_threshold, 100, 255, THRESH_BINARY);
	vector<vector<Point>> contours;
	findContours(img_threshold, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	Mat drawing = Mat::zeros(img_threshold.size(), CV_8UC1);
	
	for (size_t i = 0; i < contours.size(); i++)
	{
		double area = abs(contourArea(contours[i]));
		if (area < 3600)continue;
		//FindHull(contours[i],drawing);
		vector<Point> poly;
		//对图像轮廓点进行多边形拟合
		approxPolyDP(Mat(contours[i]), poly, 1, true);//根据点集，拟合出多边形
		vector<Point> keyPoint=FindKeyPoint(poly, drawing);
		fillPoly(drawing, Mat(poly), Scalar(255)); // 根据点集，绘制出多边形
		vector<vector<Point>> _a; _a.push_back(poly);
		drawContours(drawing, _a, -1, Scalar(255));
		//获取轮廓上的关键点
		//cvFindDominantPoints()
		//cornerShiTomasi(drawing);
	}
	imshow("result", drawing);
	waitKey(0);
    return 0;
}

