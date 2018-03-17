// Hog_Svm.cpp : �������̨Ӧ�ó������ڵ㡣

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
	vector<Rect> personSVM, personCasc, personListSVM;//��������ο�����  
	DWORD SVMTimeBegin, SVMTimeEnd, CascTimeBegin, CascTimeEnd;  //��ʱͳ��      

	//����1��Hog+svm    
	SVMTimeBegin = GetTickCount();
	HOGDescriptor hog;//HOG���������    
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());//����SVM������ΪĬ�ϲ���       
	hog.detectMultiScale(src, personSVM, 0, Size(2, 2), Size(0, 0), 1.05, 2);//��ͼ����ж�߶ȼ��  
	SVMTimeEnd = GetTickCount();
	cout << "HOG+SVM���˼���ʱ��\n" << (SVMTimeEnd - SVMTimeBegin) << endl;

	//���غϵ�ֱ�ӷ���List���غϵ�ѡȡ�������    
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

	//�����ο����ż�⵽�ľ��ο�   
	for (int i = 0; i<personListSVM.size(); i++)
	{
		Rect r = personListSVM[i];
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

