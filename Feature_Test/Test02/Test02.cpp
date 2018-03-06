// Test02.cpp : 定义控制台应用程序的入口点。
//SURF特征识别-特征点检测

#include "stdafx.h"
#include "opencv2/core.hpp" 
#include <opencv2/imgcodecs.hpp>
#include "opencv2/highgui.hpp" 
#include "opencv2/features2d.hpp"  
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;
int main()
{
	Mat img = imread("D:/1.jpg");
	//创建surf对象
	Ptr<SURF> surf = SURF::create(800);
	//创建关键点对象
	vector<KeyPoint> key;
	//识别关键点
	surf->detect(img,key);
	Mat outImg;
	//绘制关键点
	drawKeypoints(img, key, outImg);

	imshow("结果",outImg);

	while (waitKey(0)==27)
	{
		
	}
    return 0;
}

