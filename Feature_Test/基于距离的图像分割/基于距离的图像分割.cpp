// ���ھ����ͼ��ָ�.cpp : �������̨Ӧ�ó������ڵ㡣
//http://www.bubuko.com/infodetail-2274333.html
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>

using namespace std;
using namespace cv;

vector<Point> FindMax(Mat distImg)
{
	Size imgSize = distImg.size();
	vector<vector<int>> horizontalMax(imgSize.width);
	vector<vector<int>> verticalMax(imgSize.height);
	//��ȡ�������ֵλ��
	for (int x = 0; x < imgSize.width; ++x)
	{
		int _maxVal = 0;
		int _index = 0;
		uchar* p = distImg.ptr<uchar>(x);
		for (int y = 0; y < imgSize.height; ++y)
		{
			if (p[y] > _maxVal) 
			{ 
				_maxVal = p[y];
				_index = y;
			};
		}
		horizontalMax[x].push_back(_index);
	}
	//��ȡ�������ֵλ��
	for (int y = 0; y < imgSize.height; ++y)
	{
		int _maxVal = 0;
		uchar* p = distImg.ptr<uchar>(y);
		for (int x = 0; x < imgSize.width; ++x)
		{
			if (p[x] >= _maxVal)
			{
				_maxVal = p[x];
				verticalMax[y].push_back(x);
			};
		}
	}
	//�������������Ϊ���ֵ�ĵ�
	vector<Point> ps;
	for (size_t x = 0; x < imgSize.width; x++)
	{
		vector<int> yList = horizontalMax[x];
		for (size_t i = 0; i < yList.size(); i++)
		{
			int y = yList[i];
			vector<int> xList= verticalMax[y];
			for (size_t j = 0; j < xList.size(); j++)
			{
				if (xList[j] == x)
					ps.push_back(Point(x,y));
			}
		}
	}
	return ps;
}

int main()
{
	Mat src = imread("16.jpg");
	Mat grayImg;
	cvtColor(src, grayImg, CV_BGR2GRAY);
	Mat thresholdimg;
	threshold(grayImg, thresholdimg, 200, 255, THRESH_BINARY);
	Mat k = Mat::ones(5, 5, CV_8UC1); //�ṹԪ��ȫ��1,�ṹ�Ĵ�СӰ��ָ�Ч��
	dilate(thresholdimg, thresholdimg, k);
	Mat distImg;
	distanceTransform(thresholdimg, distImg, DIST_L1, 3, 5);
	imshow("distImg", distImg);
	Mat mask;
	vector<Point> ps = FindMax(distImg);
	//vector<Point> ps = Find_Max(distImg, mask, 10, 20);
	for (size_t i = 0; i < ps.size(); i++)
	{
		circle(src, ps[i], 8, Scalar(255), 4);
	}
	imshow("src", src);
	waitKey(0);
	return 0;
}

