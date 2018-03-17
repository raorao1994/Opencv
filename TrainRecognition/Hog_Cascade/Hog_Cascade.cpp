// Hog_Cascade.cpp : 定义控制台应用程序的入口点。
//

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
using namespace cv::ml;


int main()
{
	Mat src = imread("E:\\SVN\\OpenCV\\trunk\\person.jpg", 1);
	vector<Rect> personSVM, personCasc, personListCacs;//检测结果矩形框向量  
	DWORD SVMTimeBegin, SVMTimeEnd, CascTimeBegin, CascTimeEnd;  //耗时统计      

	//方法2.Hog+cascade    
	CascTimeBegin = GetTickCount();
	char* facePath = "D:\\opencv\\build\\data\\haarcascades\\haarcascade_frontalface_alt.xml";
	CascadeClassifier cascade;
	//3.1加载识别库
	cascade.load(facePath);
	//CascadeClassifier *cascade = new CascadeClassifier;
	cascade.load(facePath);
	cascade.detectMultiScale(src, personCasc);
	CascTimeEnd = GetTickCount();
	cout << "HOG+Cascade行人检测耗时：\n" << (CascTimeEnd - CascTimeBegin) << endl;

	//不重合的直接放入List，重合的选取最外侧结果    
	for (int i = 0; i < personCasc.size(); i++)
	{
		Rect r = personCasc[i];
		int j = 0;
		for (; j < personCasc.size(); j++)
			if (j != i && (r & personCasc[j]) == r)
				break;
		if (j == personCasc.size())
			personListCacs.push_back(r);
	}

	//画矩形框，缩放检测到的矩形框   
	for (int i = 0; i<personListCacs.size(); i++)
	{
		Rect r = personListCacs[i];
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

