// ���ӱ�Ե�ָ�.cpp : �������̨Ӧ�ó������ڵ㡣

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
	//Ѱ��������ʡȥ��νṹ�������ⲿ�����������յ����
	findContours(distImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); 
	drawContours(src, contours, -1, Scalar(255, 0, 255),5);
	for (size_t i = 0; i < contours.size(); i++) {
		int PointCount = contours[i].size();
		cout << "����"<<i<<"��������" << PointCount << endl;
		vector<Point> c = contours[i];
		if (PointCount > 50) {
			fillPoly(grayImg, c, Scalar(255, 0, 255));
		}
	}
	imshow("���", src);
	imshow("�����", grayImg);
	waitKey(0);
    return 0;
}

