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
	char* videoFilename = "F:\\SVN\\OpenCV\\trunk\\Data\\video1.avi";
	VideoCapture cap(videoFilename);//��Ĭ�ϵ�����ͷ
	if (!cap.isOpened())
	{
		return -1;
	}
	//����ͼ�����
	Mat img;
	GMM* model = new GMM(200);
	//
	int i = 0;
	while (!stop)
	{
		cap >> img;
		if (i < model->END_FRAME)
		{
			model->Train(img);
		}
		else
		{
			model->Apply(img);
		}
	}
    return 0;
}

