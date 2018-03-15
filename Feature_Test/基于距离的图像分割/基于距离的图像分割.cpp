// 基于距离的图像分割.cpp : 定义控制台应用程序的入口点。
//http://www.bubuko.com/infodetail-2274333.html
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>

using namespace std;
using namespace cv;

vector<Point> FindMax2(Mat distImg)
{
	Size imgSize = distImg.size();
	vector<vector<int>> horizontalMax(imgSize.width);
	vector<vector<int>> verticalMax(imgSize.height);
	//获取横向最大值位置
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
	//获取纵向最大值位置
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
	//计算出横向纵向都为最大值的点
	vector<Point> ps;
	for (size_t x = 0; x < imgSize.width; x++)
	{
		vector<int> yList = horizontalMax[x];
		for (size_t i = 0; i < yList.size(); i++)
		{
			int y = yList[i];
			vector<int> xList = verticalMax[y];
			for (size_t j = 0; j < xList.size(); j++)
			{
				if (xList[j] == x)
					ps.push_back(Point(x, y));
			}
		}
	}
	return ps;
}

/*
*Kenalsize 核大小，必须为奇数
*/
void FindMax1(Mat distImg, int Kenalsize)
{
	Size imgSize = distImg.size();
	int width = imgSize.width;
	int height = imgSize.height;
	int step = (Kenalsize - 1) / 2;
	//遍历宽度
	for (int y = step; y < height - step; y++)
	{
		//遍历高度
		for (int x = step; x < width - step; x++)
		{
			int _max = 0;
			for (int i = 0 - step; i < Kenalsize - step; i++)
			{
				uchar* p = distImg.ptr<uchar>(y + i);
				for (int j = 0 - step; j < Kenalsize - step; j++)
				{
					if (p[x + j] > _max)
						_max = p[x + j];
				}
			}
			for (int i = 0 - step; i < Kenalsize - step; i++)
			{
				uchar* p = distImg.ptr<uchar>(y + i);
				for (int j = 0 - step; j < Kenalsize - step; j++)
				{
					if (p[x + j] < _max)
						p[x + j] = 0;
				}
			}
		}
	}
	imshow("distImg1", distImg);
}

int main()
{
	Mat src = imread("thresholdimg.jpg");
	Mat grayImg;
	cvtColor(src, grayImg, CV_BGR2GRAY);
	Mat thresholdimg;
	threshold(grayImg, thresholdimg, 200, 255, THRESH_BINARY);
	Mat k = Mat::ones(5, 5, CV_8UC1); //结构元素全是1,结构的大小影响分割效果
	dilate(thresholdimg, thresholdimg, k);
	Mat distImg;
	distanceTransform(thresholdimg, distImg, DIST_L1, 3, 5);
	normalize(distImg, distImg, 0, 1, NORM_MINMAX); //标准化归一化到[0~1]
	imshow("distImg", distImg);
	Mat mask;
	FindMax1(distImg,3);
	//Find_Max(distImg, mask, 10, 20);

	//vector<Point> ps = FindMax(distImg);
	//vector<Point> ps = Find_Max(distImg, mask, 10, 20);
	/*for (size_t i = 0; i < ps.size(); i++)
	{
		circle(src, ps[i], 8, Scalar(255), 4);
	}*/
	//imshow("src", src);
	waitKey(0);
	return 0;
}

