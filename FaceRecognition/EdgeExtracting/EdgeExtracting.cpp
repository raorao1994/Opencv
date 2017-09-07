// EdgeExtracting.cpp : 定义控制台应用程序的入口点。
//http://blog.csdn.net/morewindows/article/category/1291764
#include "stdafx.h"
//第三课：图像的Canny边缘检测  
#include <opencv2/opencv.hpp>  
using namespace std;
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")  
IplImage *g_pSrcImage, *g_pCannyImg;
const char *pstrWindowsCannyTitle = "边缘检测图";
//cvCreateTrackbar的回调函数  
void on_trackbar(int threshold)
{
	/*
	当trackbar位置被改变的时，系统会调用这个回调函数，并将参数pos设置为表示trackbar位置的数值。
	*/
	//canny边缘检测  
	cvCanny(g_pSrcImage, g_pCannyImg, threshold, threshold * 3, 3);
	cvShowImage(pstrWindowsCannyTitle, g_pCannyImg);
}
int main()
{
	const char *pstrImageName = "D://8.JPG";
	const char *pstrWindowsSrcTitle = "原图";
	const char *pstrWindowsToolBar = "Threshold";

	//从文件中载入图像的灰度图CV_LOAD_IMAGE_GRAYSCALE - 灰度图  
	g_pSrcImage = cvLoadImage(pstrImageName, CV_LOAD_IMAGE_GRAYSCALE);
	g_pCannyImg = cvCreateImage(cvGetSize(g_pSrcImage), IPL_DEPTH_8U, 1);

	//创建窗口  
	cvNamedWindow(pstrWindowsSrcTitle, CV_WINDOW_AUTOSIZE);
	cvNamedWindow(pstrWindowsCannyTitle, CV_WINDOW_AUTOSIZE);

	//创建滑动条  
	int nThresholdEdge = 1;
	/*  
		第一个参数表示该trackbar的名称。
		第二个参数表示窗口名称，该trackbar将显示在这个窗口内。
		第三个参数表示创建时滑块的位置。
		第四个参数表示滑块位置的最大值，最小值固定为0。
		第五个参数表示回调函数。当滑块位置有变化时，系统会调用该回调函数。
	*/
	cvCreateTrackbar(pstrWindowsToolBar, pstrWindowsCannyTitle, &nThresholdEdge, 100, on_trackbar);
	
	//在指定窗口中显示图像  
	cvShowImage(pstrWindowsSrcTitle, g_pSrcImage);
	on_trackbar(1);

	//等待按键事件  
	cvWaitKey();

	cvDestroyWindow(pstrWindowsSrcTitle);
	cvDestroyWindow(pstrWindowsCannyTitle);
	cvReleaseImage(&g_pSrcImage);
	cvReleaseImage(&g_pCannyImg);
	return 0;
}

