// 形状匹配.cpp : 定义控制台应用程序的入口点。
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

using namespace std;
using namespace cv;

int main()
{
	//1.查找模版图像的轮廓  
	Mat templateImg = imread("11.png", CV_LOAD_IMAGE_COLOR);
	Mat copyImg1 = templateImg.clone();
	cvtColor(templateImg, templateImg, CV_BGR2GRAY);
	threshold(templateImg, templateImg, 100, 255, CV_THRESH_BINARY);//确保黑中找白  
	imshow("temp", templateImg);
	vector<vector<Point>> contours1;
	findContours(templateImg, contours1, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//最外层轮廓  
	drawContours(copyImg1, contours1, -1, Scalar(100, 255, 100), 4, 8);
	//2.查找待测试图像的轮廓  
	Mat testImg = imread("22.png", CV_LOAD_IMAGE_COLOR);
	Mat copyImg2 = testImg.clone();
	cvtColor(testImg, testImg, CV_BGR2GRAY);
	threshold(testImg, testImg, 100, 255, CV_THRESH_BINARY);//确保黑中找白  
	imshow("thresh2", testImg);
	vector<vector<Point>> contours2;
	findContours(testImg, contours2, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//最外层轮廓  
																			 //3.形状匹配---比较两个形状或轮廓间的相似度  
	for (int i = 0; i < contours2.size(); i++)//遍历待测试图像的轮廓  
	{
		//返回此轮廓与模版轮廓之间的相似度,a0越小越相似  
		double a0 = matchShapes(contours1[0], contours2[i], CV_CONTOURS_MATCH_I1, 0);
		cout << "模版轮廓与待测试图像轮廓" << i << "的相似度:" << a0 << endl;//输出两个轮廓间的相似度  
		if (a0<0.1)//如果此轮廓与模版轮廓的相似度小于0.1  
		{
			drawContours(copyImg2, contours2, i, Scalar(100, 255, 100), 2, 8);//则在待测试图像上画出此轮廓  
		}
		imshow("copyImg2", copyImg2);
		if (waitKey(0) == 27)//等待按键进行下一个轮廓，ESC则退出  
		{
			cout << "ESC退出" << endl;
			break;
		}
	}
	waitKey(0);
	return 0;
}

