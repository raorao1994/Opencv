// ���ڰ���ƥ����ص��ָ�.cpp : �������̨Ӧ�ó������ڵ㡣
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
	//Ѱ��������ʡȥ��νṹ�������ⲿ�����������յ����
	findContours(distImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	for (size_t i = 0; i < contours.size(); i++) {
		double area = abs(contourArea(contours[i]));
		if (area < 3500)continue;
		int PointCount = contours[i].size();
		cout << "����" << i << "��������" << PointCount << endl;
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
	imshow("���", src);
	imshow("�����", grayImg);
	waitKey(0);
	return 0;
}

//��ȡ��Ե�������´���
void Imgs(Mat thresholdimg, vector<Point> ps, Rect mr)
{
	//simpleBlobDetector  fitRetangle  fitEllipse
	Mat img2(thresholdimg, mr);

	
}
/**
* @brief searchConcaveRegion Ѱ�Ұ�����
* @param hull  ͹���㼯
* @param src  ԭͼ�񣨶�ֵͼ��
* @return ���� ͼ������
*/
Mat searchConcaveRegion(std::vector<std::vector<Point> >contours, Mat &src)
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

/**
* @brief searchConcavePoint
* @param src ������ͼ��
* @return ����ƥ��õİ���ԣ�2����
*/
std::vector<Point2f> searchConcavePoint(Mat &src)
{
	std::vector<Point2f> ConcavePoint;
	//����Ѱ��    
	std::vector<std::vector<Point> > contour;//�����洢����    
	std::vector<Vec4i> hierarchys;
	findContours(src, contour, hierarchys,
		CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));  //Ѱ������    

	//����������2Ҫ�˳�    
	if (contour.size()<2)
		return ConcavePoint;

	//�������������С����    
	std::sort(contour.begin(), contour.end(), [](const std::vector<Point> &s1,
		const std::vector<Point> &s2) {
		double a1 = contourArea(s1);
		double a2 = contourArea(s2);
		return a1>a2;
	});

	int minDistance = 100000000;//��̾���  
	cout << "�ܹ����ٸ�" << contour[0].size() << endl;
	for (int i = 0; i<contour[0].size(); ++i)
		for (int j = 0; j<contour[1].size(); ++j)
		{
			//ŷ�Ͼ���    
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

