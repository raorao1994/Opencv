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
	for (size_t x = 0; x < imgSize.width; x++)
	{
		int _maxVal = 0;
		for (size_t y = 0; y < imgSize.height; ++y)
		{
			if (distImg.at<float>(y,x) > _maxVal)
			{
				_maxVal = distImg.at<float>(y, x);
			};
		}
		for (size_t y = 0; y < imgSize.height; ++y)
		{
			if (distImg.at<float>(y, x) == _maxVal)
			{
				horizontalMax[x].push_back(y);
			};
		}
	}
	//获取纵向最大值位置
	for (int y = 0; y < imgSize.height; ++y)
	{
		int _maxVal = 0;
		for (int x = 0; x < imgSize.width; ++x)
		{
			if (distImg.at<float>(y, x) > _maxVal)
			{
				_maxVal = distImg.at<float>(y, x);
			};
		}
		for (int x = 0; x < imgSize.width; ++x)
		{
			if (distImg.at<float>(y, x) == _maxVal)
			{
				verticalMax[y].push_back(x);
			};
		}
	}
	//计算出横向纵向都为最大值的点
	vector<Point> ps;
	Mat src = Mat::zeros(imgSize,CV_8UC1);
	for (int x = 0; x < imgSize.width; x++)
	{

		vector<int> y = horizontalMax[x];
		for (size_t i = 0; i < y.size(); i++)
		{
			vector<int> _x = verticalMax[y[i]];
			for (size_t j = 0; j < _x.size(); j++)
			{
				if (_x[j] == x)
				{
					circle(src, Point(x, y[i]), 8, Scalar(255), 4);
					ps.push_back(Point(x, y[i]));
				}
			}
		}
	}
	imshow("src", src);
	return ps;
}

/*
*Kenalsize 核大小，必须为奇数
*/
void FindMax1(Mat distImg, int Kenalsize)
{
	Size imgSize = distImg.size();
	int width = distImg.cols;
	int height = distImg.rows;
	int _step = distImg.step[0];
	int _step1 = distImg.step[1];
	int _step2 = distImg.step[2];
	const int channels = distImg.channels();
	int step = (Kenalsize - 1) / 2;
	
	Mat _img = Mat::zeros(Size(width,height),CV_8UC1);
	//遍历宽度
	uchar* p;
	for (int y = step; y < height - step; y++)
	{
		//遍历高度
		for (int x = step; x < width - step; x++)
		{
			//算子操作
			for (int i = 0 - step; i < Kenalsize - step; i++)
			{
				for (int j = 0 - step; j < Kenalsize - step; j++)
				{
					if (distImg.at<float>(y, x) > distImg.at<float>(y + i, x + j))
						distImg.at<float>(y + i, x + j) = 0;
				}
			}
		}
	}
	/*for (int j = 0; j<distImg.rows; j++)
	{
		for (int i = 0; i<distImg.cols; i++)
		{
			double m = distImg.at<float>(j, i);
			int y = m * 255;
			if (y > 150)
			{
				*(_img.data + j*_img.step[0] + i) = y;
			}
		}
	}*/
	imshow("_img", _img);
	imshow("distImg1", distImg);
}

//计算欧氏距离的函数  
float calcEuclideanDiatance(int x1, int y1, int x2, int y2)
{
	return sqrt(float((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1)));
}

//计算棋盘距离的函数  
int calcChessboardDistance(int x1, int y1, int x2, int y2)
{
	return cv::max(abs(x1 - x2), (y1 - y2));
}

//计算麦哈顿距离(街区距离)  
int calcBlockDistance(int x1, int y1, int x2, int y2)
{
	return abs(x1 - x2) + abs(y1 - y2);
}

//距离变换函数的实现  
void distanceTrans(Mat &srcImage, Mat &dstImage)
{
	CV_Assert(srcImage.data != nullptr);
	//CV_Assert（）若括号中的表达式值为false，则返回一个错误信息。  
	//定义灰度图像的二值图像  
	Mat grayImage, binaryImage;
	//将原图像转换为灰度图像  
	cvtColor(srcImage, grayImage, CV_BGR2GRAY);
	//将灰度图像转换为二值图像  
	threshold(grayImage, binaryImage, 100, 255, THRESH_BINARY);
	imshow("二值化图像", binaryImage);

	int rows = binaryImage.rows;
	int cols = binaryImage.cols;
	uchar *pDataOne;
	uchar *pDataTwo;
	float disPara = 0;
	float fDisMIn = 0;

	//第一遍遍历图像，使用左模板更新像素值  
	for (int i = 1; i < rows - 1; i++)
	{
		//图像的行指针的获取  
		pDataOne = binaryImage.ptr<uchar>(i);
		for (int j = 1; j < cols; j++)
		{
			//分别计算左模板掩码的相关距离  
			//PL  PL  
			//PL  P  
			//PL  
			pDataTwo = binaryImage.ptr<uchar>(i - 1);
			disPara = calcEuclideanDiatance(i, j, i - 1, j - 1);
			fDisMIn = cv::min((float)pDataOne[j], disPara + pDataTwo[j - 1]);
			disPara = calcEuclideanDiatance(i, j, i - 1, j);
			fDisMIn = cv::min(fDisMIn, disPara + pDataTwo[j]);
			pDataTwo = binaryImage.ptr<uchar>(i);
			disPara = calcEuclideanDiatance(i, j, i, j - 1);
			fDisMIn = cv::min(fDisMIn, disPara + pDataTwo[j - 1]);
			pDataTwo = binaryImage.ptr<uchar>(i + 1);
			disPara = calcEuclideanDiatance(i, j, i + 1, j - 1);
			fDisMIn = cv::min(fDisMIn, disPara + pDataTwo[j - 1]);
			pDataOne[j] = (uchar)cvRound(fDisMIn);
		}
	}

	//第二遍遍历图像，使用右模板更新像素值  
	for (int i = rows - 2; i > 0; i--)
	{
		pDataOne = binaryImage.ptr<uchar>(i);
		for (int j = cols - 1; j >= 0; j--)
		{
			//分别计算右模板掩码的相关距离  
			//pR  pR  
			//pR  p  
			//pR  
			pDataTwo = binaryImage.ptr<uchar>(i + 1);
			disPara = calcEuclideanDiatance(i, j, i + 1, j);
			fDisMIn = cv::min((float)pDataOne[j], disPara + pDataTwo[j]);
			disPara = calcEuclideanDiatance(i, j, i + 1, j + 1);

			fDisMIn = cv::min(fDisMIn, disPara + pDataTwo[j + 1]);
			pDataTwo = binaryImage.ptr<uchar>(i);
			disPara = calcEuclideanDiatance(i, j, i, j + 1);
			fDisMIn = cv::min(fDisMIn, disPara + pDataTwo[j + 1]);
			pDataTwo = binaryImage.ptr<uchar>(i - 1);
			disPara = calcEuclideanDiatance(i, j, i - 1, j + 1);
			fDisMIn = cv::min(fDisMIn, disPara + pDataTwo[j + 1]);
			pDataOne[j] = (uchar)cvRound(fDisMIn);
		}
	}
	dstImage = binaryImage.clone();
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
	FindMax1(distImg,5);
	//Mat dstImage;
	//distanceTrans(distImg, dstImage);
	//imshow("距离变换图像", dstImage);

	//vector<Point> ps = Find_Max(distImg, mask, 10, 20);
	/*for (size_t i = 0; i < ps.size(); i++)
	{
		circle(src, ps[i], 8, Scalar(255), 4);
	}*/
	//imshow("src", src);
	waitKey(0);
	return 0;
}

