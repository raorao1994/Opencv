// ���ӱ�Ե�ָ�.cpp : �������̨Ӧ�ó������ڵ㡣

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>

using namespace std;
using namespace cv;
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
	//drawContours(src, contours, -1, Scalar(255, 0, 255),5);

	for (size_t i = 0; i < contours.size(); i++) { 
		double area = abs(contourArea(contours[i]));
		if (area < 3500)continue;
		int PointCount = contours[i].size();
		cout << "����"<<i<<"��������" << PointCount << endl;
		vector<Point> c = contours[i];
		if (PointCount > 50) {
			drawContours(src, contours, i, Scalar(0, 255, 0), 3);
			Rect mr = boundingRect(c);
			//fillPoly(grayImg, c, Scalar(255, 0, 255));
			Imgs(distImg, c, mr);

			//for (int i = 0; i < c.size(); i++)
			//{
			//	circle(src, c[i], 1, Scalar(0, 0, 255), 1);//������������ǵ��ߵ㣬���߸���
			//}
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
	
	vector<KeyPoint> keypoints;
	SimpleBlobDetector::Params params;
	params.filterByArea = true;
	params.minArea = 3000;
	params.maxArea = 30000;

	Ptr<SimpleBlobDetector> blobDetector = SimpleBlobDetector::create(params);
	blobDetector->detect(img2, keypoints);
	drawKeypoints(img2, keypoints, img2, Scalar(0, 255, 0));
	imshow("result", img2);

	
	

	waitKey(3000);
}
