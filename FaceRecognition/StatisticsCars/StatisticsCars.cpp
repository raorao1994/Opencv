// StatisticsCars.cpp : �������̨Ӧ�ó������ڵ㡣
//ʵʱͳ����Ƶ�еĳ�����
#include "stdafx.h"
#include "stdafx.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <iostream>
#include "GMM.h"

using namespace cv;
using namespace std;

int main()
{
	bool stop = false;
	//get video file fullpath
	char* videoFilename = "F:\\SVN\\OpenCV\\trunk\\Data\\video1.avi";
	VideoCapture cap(videoFilename);//��Ĭ�ϵ�����ͷ
	if (!cap.isOpened())
	{
		return -1;
	}
	//����ͼ�����
	Mat img;
	GMM* model = new GMM(10);
	//
	int i = 0;
	while (!stop)
	{
		//cut out one image 
		cap >> img;
		imshow("ԭͼ", img);
		Mat testimg;
		img.copyTo(testimg);
		cvtColor(testimg, testimg, CV_BGR2GRAY);
		//if image number is less than 200
		if (i < model->END_FRAME)
		{
			model->Train(testimg);
		}
		//if image number is greater than 200
		else
		{
			Mat binaryzationImg= model->Apply(testimg);
			//medianBlur(binaryzationImg, binaryzationImg, 3);
			//dilate(img, img, Mat(), Point(-1, -1), 3);
			//erode(img, img, Mat(), Point(-1, -1), 6);
			//dilate(img, img, Mat(), Point(-1, -1), 3);
			imshow("��ֵ��ͼ��", binaryzationImg);
		}
		cout << "��" << i << "����ͼƬ" << endl;
		if (waitKey(10) == 27) //Esc���˳�      
		{
			stop = true;
		}
		i++;
	}
    return 0;
}

