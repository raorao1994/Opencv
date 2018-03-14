// ��ȱ��_͹ȱ��.cpp : �������̨Ӧ�ó������ڵ㡣
//http://blog.csdn.net/jjddss/article/details/73548564
//http://blog.csdn.net/Lu597203933/article/details/14532053
#include "stdafx.h"
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

int main()
{
	Mat img = imread("thresholdimg.jpg");
	Mat img_gray;
	cvtColor(img, img_gray, CV_BGR2GRAY);
	Mat img_threshold;
	threshold(img_gray, img_threshold, 100, 255, THRESH_BINARY);
	vector<vector<Point>> contours;
	findContours(img_threshold, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	Mat drawing = Mat::zeros(img_threshold.size(), CV_8UC1);
	vector<vector<Point>>hull(contours.size());
	for (size_t i = 0; i < contours.size(); i++)
	{
		convexHull(Mat(contours[i]), hull[i], false);

		vector<Point> poly;
		approxPolyDP(Mat(contours[i]), poly, 1, true);//���ݵ㼯����ϳ������

		//drawContours(drawing, poly, i, Scalar(0, 255, 255), 2, 8);  //����

		fillConvexPoly(drawing, Mat(poly), Scalar(255));// ���ݵ㼯�����Ʋ����������

		//fillPoly(drawing, Mat(poly), Scalar(255)); // ���ݵ㼯�����Ƴ������
	}

	//drawContours(drawing, hull, -1, Scalar(255, 0, 100), -1, 8, vector<Vec4i>(), 0, Point());
	//drawContours(drawing, contours, -1, Scalar(0, 255, 0), -1, 8, vector<Vec4i>(), 0, Point());


	//drawContours(drawing, contours, -1, Scalar(255), 4);
	//drawContours(drawing, hull, -1, Scalar(255), 4);

	imshow("result", drawing);

	waitKey(0);
    return 0;
}

