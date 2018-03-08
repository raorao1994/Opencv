// 基于距离的图像分割.cpp : 定义控制台应用程序的入口点。

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>

using namespace std;
using namespace cv;

int main()
{
	Mat src = imread("3.jpg");
	/* 1.转为黑背景 change background */
	for (int row = 0; row < src.rows; row++) {
		for (int col = 0; col < src.cols; col++) {
			if (src.at<Vec3b>(row, col) == Vec3b(255, 255, 255)) {
				src.at<Vec3b>(row, col)[0] = 0;
				src.at<Vec3b>(row, col)[1] = 0;
				src.at<Vec3b>(row, col)[2] = 0;
			}
			else
			{
				src.at<Vec3b>(row, col)[0] = 255;
				src.at<Vec3b>(row, col)[1] = 255;
				src.at<Vec3b>(row, col)[2] = 255;
			}
		}
	}
	Mat grayImg;
	cvtColor(src, grayImg, CV_BGR2GRAY);
	/* 1.距离变化 distance transform*/
	Mat distImg;
	distanceTransform(grayImg, distImg, DIST_L1, 3, 5); //距离变换：输入二值图，输出距离图像，距离类型，掩膜大小，输出图像的类型5或CV_32F

	/* 5.对距离变换结果进行归一化到[0~1]之间，梯度效果 */
	normalize(distImg, distImg, 0, 1, NORM_MINMAX); //标准化归一化到[0~1]
	imshow("05对距离变换结果进行归一化", distImg);

	/* 6.距离变化的二值化 binary again，各对象孤立*/
	threshold(distImg, distImg, 0.4, 1, THRESH_BINARY); //全局阈值0.4-1
	imshow("06距离变化的二值化", distImg);

	/* 7.腐蚀，得到各个高峰 */
	Mat k1 = Mat::ones(3, 3, CV_8UC1); //结构元素全是1,结构的大小影响分割效果
	erode(distImg, distImg, k1, Point(-1, -1)); //腐蚀
	imshow("07腐蚀", distImg);

	/* 8.寻找轮廓 find contours*/
	Mat dist_8u;
	distImg.convertTo(dist_8u, CV_8U);
	vector<vector<Point>> contours;
	findContours(dist_8u, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0)); //寻找轮廓，省去层次结构，检索外部轮廓，轮廓拐点近似
																					  /* 9.画轮廓标记并填充 create marker image */
	Mat markers = Mat::zeros(src.size(), CV_32SC1); //在黑背景上画轮廓标记
	for (size_t i = 0; i < contours.size(); i++) {
		drawContours(markers, contours, static_cast<int>(i), Scalar::all(static_cast<int>(i) + 1), -1); //绘制轮廓并填充：输入，轮廓点，轮廓索引号，颜色（转为int，背景色就是从0开始的，+1避免从0开始），thickness（正值代表线宽，负值代表填充）
	}
	circle(markers, Point(9, 9), 5, Scalar(255, 255, 255), -1); //左上角画圆：原图，圆心，半径，线颜色，thickness（正值代表圆线宽，负值代表填充圆）
	imshow("08寻找轮廓", markers * 1000);

	/* 10.执行分水岭，分割对象 perform watershed */
	watershed(src, markers);  //分水岭变换,分割对象：输入锐化原图8位3通道，输入高峰轮廓标记
	Mat mark = Mat::zeros(markers.size(), CV_8UC1);
	markers.convertTo(mark, CV_8UC1);
	bitwise_not(mark, mark, Mat()); //位取反，变黑背景
	imshow("09执行分水岭", mark);  //各对象灰度等级不一样

							  /* 11.填充随机颜色到每个分割区域并显示最终结果 fill with color and display final result*/
	vector<Vec3b> colors;
	for (size_t i = 0; i < contours.size(); i++) {  //生成随机颜色
		int r = theRNG().uniform(0, 255);
		int g = theRNG().uniform(0, 255);
		int b = theRNG().uniform(0, 255);
		colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
	}
	Mat dst = Mat::zeros(markers.size(), CV_8UC3);
	for (int row = 0; row < markers.rows; row++) {
		for (int col = 0; col < markers.cols; col++) {
			int index = markers.at<int>(row, col); //取得分水岭分割对象的不同灰度等级的值

			if (index > 0 && index <= static_cast<int>(contours.size())) {
				dst.at<Vec3b>(row, col) = colors[index - 1];  //每一个非0灰度等级着一种色
			}
			else {
				dst.at<Vec3b>(row, col) = Vec3b(0, 0, 0); //非对象着黑色
			}
		}
	}
	imshow("10结果", dst);

	waitKey(0);
	return 0;
}

