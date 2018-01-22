// StatisticsCars.cpp : 定义控制台应用程序的入口点。
//实时统计视频中的车流量
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
	VideoCapture cap(videoFilename);//打开默认的摄像头
	if (!cap.isOpened())
	{
		return -1;
	}
	//创建图像对象
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

