// 连接边缘分割.cpp : 定义控制台应用程序的入口点。

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>

using namespace std;
using namespace cv;
void Imgs(Mat thresholdimg, vector<Point> ps, Rect mr);
void cacute(Mat im, vector<Point> ps);
double Angle(Point cen, Point first, Point second);
int main()
{
	Mat img = imread("img.jpg");
	Mat src = imread("thresholdimg.jpg");
	Mat grayImg;
	cvtColor(src, grayImg, CV_BGR2GRAY);
	Mat distImg;
	threshold(grayImg, distImg, 200, 255, THRESH_BINARY);
	vector<vector<Point>> contours;
	//寻找轮廓，省去层次结构，检索外部轮廓，轮廓拐点近似
	findContours(distImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); 
	//drawContours(src, contours, -1, Scalar(255, 0, 255),5);

	for (size_t i = 0; i < contours.size(); i++) { 
		double area = abs(contourArea(contours[i]));
		if (area < 3500)continue;
		int PointCount = contours[i].size();
		cout << "轮廓"<<i<<"的数量：" << PointCount << endl;
		vector<Point> c = contours[i];
		if (PointCount > 50) {
			drawContours(src, contours, i, Scalar(0, 255, 0), 3);
			Rect mr = boundingRect(c);
			//fillPoly(grayImg, c, Scalar(255, 0, 255));
			Imgs(distImg, c, mr);
			cacute(src, c);
			//for (int i = 0; i < c.size(); i++)
			//{
			//	circle(src, c[i], 1, Scalar(0, 0, 255), 1);//第五个参数我们调高点，让线更粗
			//}
		}
	}


	imshow("结果", src);
	imshow("多边形", grayImg);
	waitKey(0);
    return 0;
}
//提取边缘，并重新处理
void Imgs(Mat thresholdimg, vector<Point> ps, Rect mr)
{
	//simpleBlobDetector  fitRetangle  fitEllipse
	Mat img2(thresholdimg, mr);
	
	/*vector<KeyPoint> keypoints;
	SimpleBlobDetector::Params params;
	params.filterByArea = true;
	params.minArea = 3000;
	params.maxArea = 30000;

	Ptr<SimpleBlobDetector> blobDetector = SimpleBlobDetector::create(params);
	blobDetector->detect(img2, keypoints);
	drawKeypoints(img2, keypoints, img2, Scalar(0, 255, 0));
	imshow("result", img2);
	waitKey(3000);*/
}

void cacute(Mat im,vector<Point> ps)
{
	int count = ps.size();
	for (int i = 1; i < count-1; i++)
	{
		Point cen = ps[i];
		Point first = ps[i-1];
		Point second = ps[i + 1];
		double angle = Angle(cen, first, second);
		if (angle<=110) {
			circle(im, cen, 5, Scalar(0, 255, 255), 2);
			cout << "第"<<i<<"点的角度为："<< angle << endl;
		}
	}
}


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
	double angleAMB = acos(cosM) * 180 / M_PI;
	return angleAMB;
}


