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
string getstring(const int n)
{

	std::stringstream newstr;
	newstr << n;
	return newstr.str();

}
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
	GMM* model = new GMM(100);
	//
	int i = 0;
	int cars = 0;
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
			medianBlur(binaryzationImg, binaryzationImg, 3);
			dilate(binaryzationImg, binaryzationImg, Mat(), Point(-1, -1), 3);
			erode(binaryzationImg, binaryzationImg, Mat(), Point(-1, -1), 6);
			dilate(binaryzationImg, binaryzationImg, Mat(), Point(-1, -1), 3);
			imshow("二值化图像", binaryzationImg);

			//检测车身
			Mat findc;
			binaryzationImg.copyTo(findc);
			vector<vector<Point>> contours;
			//寻找轮廓
			cv::findContours(findc, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
			const int maxArea = 100;
			size_t s = contours.size();
			RNG rng;
			for (size_t i = 0; i < s; i++)
			{
				double area = abs(contourArea(contours[i]));
				if (area > maxArea)
				{
					Scalar sca_color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
					Rect mr = boundingRect(Mat(contours[i]));
					rectangle(img, mr, sca_color, 2, 8, 0);
					//可以对动态目标进行相应操作  
				}
			}
			//char text[50];
			//sprintf_s(text, "count：", getstring(s));
			//putText(img, text, Point(50, 50), 3, 1, Scalar(0, 255, 255), 2, 8, false);
			cars += s;
			cout <<"总数："<<cars<< "实时数量" << s << endl;
			imshow("标记原图", img);
		}
		cout << "第" << i << "张图片" << endl;
		if (waitKey(10) == 27) //Esc键退出      
		{
			stop = true;
		}
		i++;
	}
    return 0;
}

