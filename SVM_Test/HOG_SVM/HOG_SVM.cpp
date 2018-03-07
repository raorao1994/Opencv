// HOG_SVM.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>  
#include <opencv2/opencv.hpp>  

using namespace cv;
using namespace std;

int main()
{
	Mat image = imread("1.jpg");
	// 1. 定义HOG对象 
	//HOG检测器，用来计算HOG描述子的
	HOGDescriptor hog(Size(64, 128), Size(16, 16), Size(8, 8), Size(8, 8), 9);
	// 2. 设置SVM分类器
	// 采用已经训练好的行人检测分类器
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());   
	// 3. 在测试图像上检测行人区域
	vector<cv::Rect> regions;
	hog.detectMultiScale(image, regions, 0, Size(8, 8), Size(32, 32), 1.05, 1);
	// 显示  
	for (size_t i = 0; i < regions.size(); i++)
	{
		rectangle(image, regions[i], Scalar(0, 0, 255), 2);
	}
	imshow("HOG行人检测", image);
	waitKey();

	return 0;
}

