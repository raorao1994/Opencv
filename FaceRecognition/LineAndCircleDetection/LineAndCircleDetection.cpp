// LineAndCircleDetection.cpp : 定义控制台应用程序的入口点。
//http://blog.csdn.net/morewindows/article/details/8266985
//第六篇 线段检测与圆检测
#include "stdafx.h"
#include <opencv2/opencv.hpp>  
using namespace std;
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")  
int main()
{
	const char *pstrWindowsSrcTitle = "原图";
	const char *pstrWindowsLineName = "线段检测";

	// 从文件中加载原图  
	IplImage *pSrcImage = cvLoadImage("D://9.JPG", CV_LOAD_IMAGE_UNCHANGED);
	// 灰度图  
	IplImage *pGrayImage = cvCreateImage(cvGetSize(pSrcImage), IPL_DEPTH_8U, 1);
	cvCvtColor(pSrcImage, pGrayImage, CV_BGR2GRAY);
	// 边缘图  
	IplImage *pCannyImage = cvCreateImage(cvGetSize(pSrcImage), IPL_DEPTH_8U, 1);
	cvCanny(pGrayImage, pCannyImage, 30, 90);
	//cvSmooth(pCannyImage, pCannyImage);  

	// 线段检测(只能针对二值图像)  
	CvMemStorage *pcvMStorage = cvCreateMemStorage();
	double fRho = 1;
	double fTheta = CV_PI / 180;
	int nMaxLineNumber = 50;   //最多检测条直线  
	double fMinLineLen = 50;   //最小线段长度  
	double fMinLineGap = 10;   //最小线段间隔  
	CvSeq *pcvSeqLines = cvHoughLines2(pCannyImage, pcvMStorage, CV_HOUGH_PROBABILISTIC, fRho, fTheta, nMaxLineNumber, fMinLineLen, fMinLineGap);

	// 绘制线段  
	IplImage *pColorImage = cvCreateImage(cvGetSize(pSrcImage), IPL_DEPTH_8U, 3);
	cvCvtColor(pCannyImage, pColorImage, CV_GRAY2BGR);
	int i;
	for (i = 0; i < pcvSeqLines->total; i++)
	{
		CvPoint* line = (CvPoint*)cvGetSeqElem(pcvSeqLines, i);
		cvLine(pColorImage, line[0], line[1], CV_RGB(255, 0, 0), 2);
	}

	cvNamedWindow(pstrWindowsSrcTitle, CV_WINDOW_AUTOSIZE);
	cvShowImage(pstrWindowsSrcTitle, pSrcImage);
	cvNamedWindow(pstrWindowsLineName, CV_WINDOW_AUTOSIZE);
	cvShowImage(pstrWindowsLineName, pColorImage);

	cvWaitKey(0);

	cvReleaseMemStorage(&pcvMStorage);
	cvDestroyWindow(pstrWindowsSrcTitle);
	cvDestroyWindow(pstrWindowsLineName);
	cvReleaseImage(&pSrcImage);
	cvReleaseImage(&pGrayImage);
	cvReleaseImage(&pCannyImage);
	cvReleaseImage(&pColorImage);
	return 0;
}

