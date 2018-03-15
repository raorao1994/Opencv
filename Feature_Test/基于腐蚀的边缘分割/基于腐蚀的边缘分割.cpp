// ���ڸ�ʴ�ı�Ե�ָ�.cpp : �������̨Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
	Mat src = imread("thresholdimg.jpg");
	Mat grayImg;
	cvtColor(src, grayImg, CV_BGR2GRAY);
	/* 1.����仯�Ķ�ֵ�� binary again�����������*/
	Mat thresholdimg;
	threshold(grayImg, thresholdimg, 200, 255, THRESH_BINARY); //ȫ����ֵ0.4-1
	imshow("��ֵ��", thresholdimg);
	vector<vector<Point>> contours;
	findContours(thresholdimg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	Mat drawing = Mat::zeros(thresholdimg.size(), CV_8UC1);
	//for (size_t i = 0; i < contours.size(); i++)
	//{

	//	double area = abs(contourArea(contours[i]));
	//	if (area < 3600)continue;
	//	//FindHull(contours[i],drawing);
	//	vector<Point> poly;
	//	//��ͼ����������ж�������
	//	approxPolyDP(Mat(contours[i]), poly, 1, true);//���ݵ㼯����ϳ������
	//	//fillPoly(drawing, Mat(poly), Scalar(255)); // ���ݵ㼯�����Ƴ������
	//	fillConvexPoly(drawing, Mat(contours[i]), Scalar(255)); //���ݵ㼯�����Ʋ����������
	//}

	/* 2.��ʴ���õ������߷� */
	Mat k1 = Mat::ones(5, 5, CV_8UC1); //�ṹԪ��ȫ��1,�ṹ�Ĵ�СӰ��ָ�Ч��
	erode(thresholdimg, thresholdimg, k1, Point(-1, -1)); //��ʴ
	erode(thresholdimg, thresholdimg, k1, Point(-1, -1)); //��ʴ
	erode(thresholdimg, thresholdimg, k1, Point(-1, -1)); //��ʴ
	erode(thresholdimg, thresholdimg, k1, Point(-1, -1)); //��ʴ
	erode(thresholdimg, thresholdimg, k1, Point(-1, -1)); //��ʴ
	imshow("��ʴ", thresholdimg);
	waitKey(0);
    return 0;
}

