// ��ȱ��_͹ȱ��.cpp : �������̨Ӧ�ó������ڵ㡣
//http://blog.csdn.net/jjddss/article/details/73548564
//http://blog.csdn.net/Lu597203933/article/details/14532053  ͼ���������ƥ��(hu��)
//http://blog.csdn.net/keith_bb/article/details/70197104 ͼ��ľ�moments
//http://blog.sina.com.cn/s/blog_662c785901011i7z.html  ������
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

/*
*Angle �Ƕȼ���
*cen �м��
*first ���
*second���
*���->�м��->���
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

	//�����Ƿ�Ϊ����180�ȵĽ�
	if (cen.x == first.x)
	{
		//cen��first���㹹�ɵ�ֱ�ߴ�ֱ��y��
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
//Ѱ��͹��
void FindHull(vector<Point> contour,Mat drawing)
{
	vector<Point> hull;
	convexHull(Mat(contour), hull, false);
	vector<Point> poly;
	//��ͼ����������ж�������
	approxPolyDP(Mat(hull), poly, 1, true);//���ݵ㼯����ϳ������
	fillPoly(drawing, Mat(poly), Scalar(255)); // ���ݵ㼯�����Ƴ������
}
//Ѱ�Ҿ�
void FindMoments(vector<Point> contour,Mat drawing)
{
	//����ͼ���
	Moments mu;
	mu = moments(contour, false);
	//����ͼ�������
	Point mc;
	//Point2f mc;
	mc = Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00);
	circle(drawing, mc, 5, Scalar(255), 2);
}
//�ǵ���
void cornerShiTomasi(Mat img)
{
	//���������
	//	InputArray _image�������ͼ��InputArray����һ��ΪMat��
	//	int maxCorners���ɼ��Ľǵ�����ֵ
	//	double qualityLevel���ǵ�����ֵ�����ޣ�����(������*����ֵ)���ֵ����Ϊ�ǽǵ㡣
	//	double minDistance���ǵ����С���
	//	InputArray _mask�����룬�������ò����������
	//	int blockSize������Ĵ��ڴ�С
	//	bool useHarrisDetector���Ƿ�ʹ��harris�ǵ���
	//	double harrisK��harris�ǵ������
	//	���������
	//	OutputArray _corners������Ľǵ㣬OutputArray����һ��ΪMat��

	int minDis = 2;
	vector<Point2f> corner;
	goodFeaturesToTrack(img, corner, 270, 0.4, minDis, Mat(), 5, false, 0);//0.04
	for (int i = 0; i<corner.size(); i++) {
		circle(img, Point(corner.at(i).x, corner.at(i).y), 5, Scalar(255));
	}
	cout << "�ؼ������" << corner.size() << endl;
}
//������
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
		//�жϽڵ��Ƿ��غ�
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
	//�Ƚ���canny
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
		//��ͼ����������ж�������
		approxPolyDP(Mat(contours[i]), poly, 1, true);//���ݵ㼯����ϳ������
		vector<Point> keyPoint=FindKeyPoint(poly, drawing);
		fillPoly(drawing, Mat(poly), Scalar(255)); // ���ݵ㼯�����Ƴ������
		vector<vector<Point>> _a; _a.push_back(poly);
		drawContours(drawing, _a, -1, Scalar(255));
		//��ȡ�����ϵĹؼ���
		//cvFindDominantPoints()
		//cornerShiTomasi(drawing);
	}
	imshow("result", drawing);
	waitKey(0);
    return 0;
}

