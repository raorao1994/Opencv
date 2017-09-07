// ConsoleApplication1.cpp : 定义控制台应用程序的入口点。
//第一课

#include "stdafx.h"
#include <opencv2\opencv.hpp>

int first()
{
	const char *pstrImageName = "D://8.jpg";
	const char *pstrWindowsTitle = "OpenCV第一个程序";

	//从文件中读取图像  
	IplImage *pImage = cvLoadImage(pstrImageName, CV_LOAD_IMAGE_UNCHANGED);

	//创建窗口  
	cvNamedWindow(pstrWindowsTitle, CV_WINDOW_AUTOSIZE);

	//在指定窗口中显示图像  
	cvShowImage(pstrWindowsTitle, pImage);

	//等待按键事件  
	cvWaitKey();
	//清除制定窗口
	cvDestroyWindow(pstrWindowsTitle);
	//清空pImage对象内存
	cvReleaseImage(&pImage);
	return 0;
}


//第一课：配置opencv。打开图像
int main()
{
	first();
	//cv::Mat相当于cv.Mat意思是cv命名空间下的Mat类
	//可以在头部加入using namespace cv;省去添加cv命名空间
	cv::Mat img = cv::imread("D://8.jpg");//替换成你的图片路径

	cv::imshow("test", img);

	cv::waitKey();

	return 0;
}

