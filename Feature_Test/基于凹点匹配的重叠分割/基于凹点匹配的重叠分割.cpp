// ���ڰ���ƥ����ص��ָ�.cpp : �������̨Ӧ�ó������ڵ㡣
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
	//Ѱ��������ʡȥ��νṹ�������ⲿ�����������յ����
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
	imshow("���", src);
	waitKey(0);
	return 0;
}

//��ȡ��Ե�������´���
void Imgs(Mat thresholdimg, vector<Point> ps, Rect mr)
{
	//simpleBlobDetector  fitRetangle  fitEllipse
	Mat img2(thresholdimg, mr);
	//����
	Mat aotu = searchConcaveRegion(ps, thresholdimg);
	vector<Point> pt;
	//����ָ��
	pt = searchConcavePoint(aotu);
	cout << "�ָ��ʶ�������" << pt.size() << endl;
	//�����ָ��
	if (pt.size() >= 2)
	{
		for (int i = 0; i < pt.size(); i+=2)
		{
			line(src, pt[i], pt[i + 1], Scalar(255, 0, 0), 2, 8, 0);
		}
	}
	//���Ե�ָ��index����
	for (int i = 0; i < pt.size(); i++)
	{
		for (int j = 0; j < ps.size(); j++)
		{
			int xycout = (pt[i].x - ps[j].x);
		}
	}
}
/**
* @brief searchConcaveRegion Ѱ�Ұ�����
* @param hull  ͹���㼯
* @param src  ԭͼ�񣨶�ֵͼ��
* @return ���� ͼ������
*/
Mat searchConcaveRegions(std::vector<std::vector<Point> >contours, Mat &src)
{
	if (src.empty())
		return Mat();

	//����ÿ��������Ѱ����͹��  
	vector<vector<Point>>hull(contours.size());
	for (unsigned int i = 0; i<contours.size(); ++i)
	{
		convexHull(Mat(contours[i]), hull[i], false);
	}

	//������������͹��  
	Mat drawing = Mat::zeros(src.size(), CV_8UC1);
	for (unsigned int i = 0; i < contours.size(); ++i)
	{
		drawContours(drawing, hull, i, Scalar(255,0,100), -1, 8, vector<Vec4i>(), 0, Point());
		drawContours(drawing, contours, i, Scalar(0,255,0), -1, 8, vector<Vec4i>(), 0, Point());
	}

	medianBlur(drawing, drawing, 3);

	imshow("������", drawing);

	return drawing;
}
Mat searchConcaveRegion(vector<Point>contours, Mat &src)
{
	if (src.empty())
		return Mat();

	//����ÿ��������Ѱ����͹��  
	vector<vector<Point>>_contours;
	_contours.push_back(contours);
	vector<vector<Point>>hull(_contours.size());
	convexHull(Mat(contours), hull[0], false);
	//������������͹��  
	Mat drawing = Mat::zeros(src.size(), CV_8UC1);
	drawContours(drawing, hull, -1, Scalar(255, 0, 100), -1, 8, vector<Vec4i>(), 0, Point());
	drawContours(drawing, _contours, -1, Scalar(0, 255, 0), -1, 8, vector<Vec4i>(), 0, Point());
	//��ֵ�˲�
	//medianBlur(drawing, drawing, 3);
	//imshow("������", drawing);
	return drawing;
}
/**
* @brief searchConcavePoint
* @param src ������ͼ��
* @return ����ƥ��õİ���ԣ�2����
*/
vector<Point> searchConcavePoint(Mat &src)
{
	//����Ѱ��    
	vector<vector<Point> > contour;//�����洢����    
	vector<Vec4i> hierarchys;
	findContours(src, contour, hierarchys,
		CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));  //Ѱ������    

	vector<Point> ConcavePoint;
	//����������2Ҫ�˳�    
	if (contour.size()<2)
		return ConcavePoint;

	vector<vector<Point> > _contours;
	for (int i = 0; i < contour.size(); i++)
	{
		double area = abs(contourArea(contour[i]));
		if (area < 3000)continue;
		_contours.push_back(contour[i]);
	}
	//�������������С����    
	std::sort(_contours.begin(), _contours.end(), [](const std::vector<Point> &s1,
	const std::vector<Point> &s2) {
	double a1 = contourArea(s1);
	double a2 = contourArea(s2);
	return a1>a2;
	});
	//����͹�棬������С��
	for (int a = 0; a < _contours.size(); a+=2)
	{
		int minDistance = 100000000;//��̾���  
		vector<Point> c1 = _contours[a];
		int b = a + 1;
		if (b == _contours.size())b = 0;
		vector<Point> c2 = _contours[b];
		Point p1, p2;
		//������͹�������С��
		for (int i = 0; i < c1.size(); ++i)
		{
			for (int j = 0; j<c2.size(); ++j)
			{
				//ŷ�Ͼ���    
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

