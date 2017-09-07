// TakePhoto.cpp : 定义控制台应用程序的入口点。
//拍照
#include "stdafx.h"
#include<opencv2\opencv.hpp>  
using namespace std;
using namespace cv;

#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <iostream>  
//拍照，保存图片
int takephoto() 
{
	VideoCapture cap(0);
	int i = 1;
	while (1)
	{
		Mat frame;
		char key = waitKey(100);
		cap >> frame;
		imshow("frame", frame);
		//string filename = format("D://Picture/FaceBase/%d.bmp", i);
		string filename = format("D://Picture//FaceBase//%d.jpg", i);
		cout << key << endl;
		switch (key)
		{
		case'p':
			i++;
			imwrite(filename, frame);
			imshow("photo", frame);
			waitKey(500);
			destroyWindow("photo");
			break;
		default:
			break;
		}
	}
	return 0;
}
//视频边缘提取
int video()
{
	VideoCapture cap(0);
	if (!cap.isOpened())
	{
		return -1;
	}
	Mat frame;
	Mat edges;

	bool stop = false;
	cvNamedWindow("视频", CV_WINDOW_FULLSCREEN);//CV_WINDOW_FULLSCREEN
	while (!stop)
	{
		cap >> frame;
		cvtColor(frame, edges, CV_BGR2GRAY);
		GaussianBlur(edges, edges, Size(7, 7), 1.5, 1.5);
		Canny(edges, edges, 0, 30, 3);
		//图片缩放
		//IplImage *img_v = &IplImage(edges);
		//cvResize(img_v, img_v, CV_INTER_NN);
		//resize(edges, edges, Size(), 1.5, 1.5);
		imshow("视频", edges);
		//cvShowImage("视频", img_v);
		char key = waitKey(100);
		if (key=='p')
			stop = true;
	}
	return 0;
}

int video2()
{
	//打开视频文件：其实就是建立一个VideoCapture结构  
	const char* path = "E:/";//D:/videos/PetsD2TeC2.avi
	VideoCapture capture(path);
	//检测是否正常打开:成功打开时，isOpened返回ture  
	if (!capture.isOpened())
		cout << "fail to open!" << endl;
	//获取整个帧数  
	long totalFrameNumber = capture.get(CV_CAP_PROP_FRAME_COUNT);
	cout << "整个视频共" << totalFrameNumber << "帧" << endl;


	//设置开始帧()  
	long frameToStart = 300;
	capture.set(CV_CAP_PROP_POS_FRAMES, frameToStart);
	cout << "从第" << frameToStart << "帧开始读" << endl;


	//设置结束帧  
	int frameToStop = 400;

	if (frameToStop < frameToStart)
	{
		cout << "结束帧小于开始帧，程序错误，即将退出！" << endl;
		return -1;
	}
	else
	{
		cout << "结束帧为：第" << frameToStop << "帧" << endl;
	}


	//获取帧率  
	double rate = capture.get(CV_CAP_PROP_FPS);
	cout << "帧率为:" << rate << endl;



	//定义一个用来控制读取视频循环结束的变量  
	bool stop = false;
	//承载每一帧的图像  
	Mat frame;
	//显示每一帧的窗口  
	namedWindow("Extracted frame");
	//两帧间的间隔时间:  
	//int delay = 1000/rate;  
	int delay = 1000 / rate;


	//利用while循环读取帧  
	//currentFrame是在循环体中控制读取到指定的帧后循环结束的变量  
	long currentFrame = frameToStart;


	//滤波器的核  
	int kernel_size = 3;
	Mat kernel = Mat::ones(kernel_size, kernel_size, CV_32F) / (float)(kernel_size*kernel_size);

	while (!stop)
	{
		//读取下一帧  
		if (!capture.read(frame))
		{
			cout << "读取视频失败" << endl;
			return -1;
		}

		//这里加滤波程序  
		imshow("Extracted frame", frame);
		filter2D(frame, frame, -1, kernel);

		imshow("after filter", frame);
		cout << "正在读取第" << currentFrame << "帧" << endl;
		//waitKey(int delay=0)当delay ≤ 0时会永远等待；当delay>0时会等待delay毫秒  
		//当时间结束前没有按键按下时，返回值为-1；否则返回按键  


		int c = waitKey(delay);
		//按下ESC或者到达指定的结束帧后退出读取视频  
		if ((char)c == 27 || currentFrame > frameToStop)
		{
			stop = true;
		}
		//按下按键后会停留在当前帧，等待下一次按键  
		if (c >= 0)
		{
			waitKey(0);
		}
		currentFrame++;

	}
	//关闭视频文件  
	capture.release();
	waitKey(0);
	return 0;
}

int video3()
{
	cvNamedWindow("Example1", CV_WINDOW_AUTOSIZE);
	CvCapture* capture; //初始化一个CvCapture结构的指针
	capture = cvCaptureFromCAM(0);//如果参数为1，则从摄像头中读入数据，并返回一个CvCapture的指针
	assert(capture != NULL); //断言（assert）使用，检查capture是否为空指针，为假时程序退出，并打印错误消息
	IplImage* frame;
	while (1)
	{
		frame = cvQueryFrame(capture);//用于将下一帧视频文件载入内存（实际是填充和更新CvCapture结构中），返回一个对应当前帧的指针
		if (!frame)
			break;
		cvShowImage("Example1", frame);
		char c = cvWaitKey(33);
		if (c == 27) break; //出发ESC键退出循环，读入数据停止
	}
	cvReleaseCapture(&capture);//释放内存
	cvDestroyWindow("Example1");
	return 2;
}

int main()
{
	takephoto();
	//video();
	//video2();
	//video3();
	return 0;
}

