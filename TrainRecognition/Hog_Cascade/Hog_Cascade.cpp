// Hog_Cascade.cpp : �������̨Ӧ�ó������ڵ㡣
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
	vector<Rect> personSVM, personCasc, personListCacs;//��������ο�����  
	DWORD SVMTimeBegin, SVMTimeEnd, CascTimeBegin, CascTimeEnd;  //��ʱͳ��      

	//����2.Hog+cascade    
	CascTimeBegin = GetTickCount();
	char* facePath = "D:\\opencv\\build\\data\\haarcascades\\haarcascade_frontalface_alt.xml";
	CascadeClassifier cascade;
	//3.1����ʶ���
	cascade.load(facePath);
	//CascadeClassifier *cascade = new CascadeClassifier;
	cascade.load(facePath);
	cascade.detectMultiScale(src, personCasc);
	CascTimeEnd = GetTickCount();
	cout << "HOG+Cascade���˼���ʱ��\n" << (CascTimeEnd - CascTimeBegin) << endl;

	//���غϵ�ֱ�ӷ���List���غϵ�ѡȡ�������    
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

	//�����ο����ż�⵽�ľ��ο�   
	for (int i = 0; i<personListCacs.size(); i++)
	{
		Rect r = personListCacs[i];
		r.x += cvRound(r.width*0.1);
		r.width = cvRound(r.width*0.8);
		r.y += cvRound(r.height*0.07);
		r.height = cvRound(r.height*0.8);
		rectangle(src, r, Scalar(0, 0, 255), 2, 8, 0);
	}
	imshow("HOG����+SVM/Cascade���˼��", src);
	waitKey();
	return 0;
}

