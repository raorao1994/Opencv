// Hog_Svm.cpp : 定义控制台应用程序的入口点。

#include "stdafx.h"
#include <iostream> 
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>    
#include <opencv2/objdetect/objdetect.hpp>    
#include <opencv2/ml/ml.hpp>    
#include <Windows.h>  

using namespace std;
using namespace cv;

int main()
{
	Mat src = imread("E:\\SVN\\OpenCV\\trunk\\person.jpg", 1);
	vector<Rect> personSVM, personCasc, personListSVM;//检测结果矩形框向量  
	DWORD SVMTimeBegin, SVMTimeEnd, CascTimeBegin, CascTimeEnd;  //耗时统计      

	//方法1，Hog+svm    
	SVMTimeBegin = GetTickCount();
	HOGDescriptor hog;//HOG特征检测器    
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());//设置SVM分类器为默认参数       
	hog.detectMultiScale(src, personSVM, 0, Size(2, 2), Size(0, 0), 1.05, 2);//对图像进行多尺度检测  
	SVMTimeEnd = GetTickCount();
	cout << "HOG+SVM行人检测耗时：\n" << (SVMTimeEnd - SVMTimeBegin) << endl;

	//不重合的直接放入List，重合的选取最外侧结果    
	for (int i = 0; i < personSVM.size(); i++)
	{
		Rect r = personSVM[i];
		int j = 0;
		for (; j < personSVM.size(); j++)
			if (j != i && (r & personSVM[j]) == r)
				break;
		if (j == personSVM.size())
			personListSVM.push_back(r);
	}

	//画矩形框，缩放检测到的矩形框   
	for (int i = 0; i<personListSVM.size(); i++)
	{
		Rect r = personListSVM[i];
		r.x += cvRound(r.width*0.1);
		r.width = cvRound(r.width*0.8);
		r.y += cvRound(r.height*0.07);
		r.height = cvRound(r.height*0.8);
		rectangle(src, r, Scalar(0, 0, 255), 2, 8, 0);

	}
	imshow("HOG特征+SVM/Cascade行人检测", src);
	waitKey();
	return 0;
}

