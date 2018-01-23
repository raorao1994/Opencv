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
	//get video file fullpath
	char* videoFilename = "F:\\SVN\\OpenCV\\trunk\\Data\\video1.avi";
	VideoCapture cap(videoFilename);//打开默认的摄像头
	if (!cap.isOpened())
	{
		return -1;
	}
	//创建图像对象
	Mat img;
	GMM* model = new GMM(10);
	//
	int i = 0;
	while (!stop)
	{
		//cut out one image 
		cap >> img;
		imshow("原图", img);
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
			imshow("二值化图像", binaryzationImg);
		}
		cout << "第" << i << "几张图片" << endl;
		if (waitKey(10) == 27) //Esc键退出      
		{
			stop = true;
		}
		i++;
	}
    return 0;
}

