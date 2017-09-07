// Binaryzation.cpp : 定义控制台应用程序的入口点。
//第四课：图像的二值化
//http://blog.csdn.net/morewindows/article/details/8239678

#include "stdafx.h"
#include <opencv2/opencv.hpp>  
using namespace std;
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")  
IplImage *g_pGrayImage = NULL;
IplImage *g_pBinaryImage = NULL;
const char *pstrWindowsBinaryTitle = "二值图";
void on_trackbar(int pos)
{
	// 转为二值图  
	cvThreshold(g_pGrayImage, g_pBinaryImage, pos, 255, CV_THRESH_BINARY);
	// 显示二值图  
	cvShowImage(pstrWindowsBinaryTitle, g_pBinaryImage);
}

int main()
{
	const char * imgpath = "D://8.jpg";
	const char *pstrWindowsSrcTitle = "原图";
	const char *pstrWindowsToolBarName = "二值图阈值";

	// 从文件中加载原图  
	IplImage *pSrcImage = cvLoadImage(imgpath, CV_LOAD_IMAGE_UNCHANGED);

	// 转为灰度图  
	g_pGrayImage = cvCreateImage(cvGetSize(pSrcImage), IPL_DEPTH_8U, 1);
	cvCvtColor(pSrcImage, g_pGrayImage, CV_BGR2GRAY);

	// 创建二值图  
	g_pBinaryImage = cvCreateImage(cvGetSize(g_pGrayImage), IPL_DEPTH_8U, 1);

	// 显示原图  
	cvNamedWindow(pstrWindowsSrcTitle, CV_WINDOW_AUTOSIZE);
	cvShowImage(pstrWindowsSrcTitle, pSrcImage);
	// 创建二值图窗口  
	cvNamedWindow(pstrWindowsBinaryTitle, CV_WINDOW_AUTOSIZE);

	// 滑动条    
	int nThreshold = 0;
	/*
	第一个参数表示输入图像，必须为单通道灰度图。
	第二个参数表示输出的边缘图像，为单通道黑白图。
	第三个参数表示阈值
	第四个参数表示最大值。
	第五个参数表示运算方法。
	在OpenCV的imgproc\types_c.h中可以找到运算方法的定义。
	Threshold types 
	enum
	{
		CV_THRESH_BINARY = 0,  /* value = value > threshold ? max_value : 0      
		CV_THRESH_BINARY_INV = 1,  /* value = value > threshold ? 0 : max_value       
		CV_THRESH_TRUNC = 2,  /* value = value > threshold ? threshold : value  
		CV_THRESH_TOZERO = 3,  /* value = value > threshold ? value : 0          
		CV_THRESH_TOZERO_INV = 4,  /* value = value > threshold ? 0 : value          
		CV_THRESH_MASK = 7,
		CV_THRESH_OTSU = 8  use Otsu algorithm to choose the optimal threshold value; combine the flag with one of the above CV_THRESH_* values 
	};
	*/
	cvCreateTrackbar(pstrWindowsToolBarName, pstrWindowsBinaryTitle, &nThreshold, 254, on_trackbar);

	on_trackbar(1);

	cvWaitKey(0);

	cvDestroyWindow(pstrWindowsSrcTitle);
	cvDestroyWindow(pstrWindowsBinaryTitle);
	cvReleaseImage(&pSrcImage);
	cvReleaseImage(&g_pGrayImage);
	cvReleaseImage(&g_pBinaryImage);
	return 0;
}

