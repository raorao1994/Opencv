// FaceRecognition.cpp : 定义控制台应用程序的入口点。
//http://blog.csdn.net/morewindows/article/details/8426318
//第十二篇 人脸检测
#include "stdafx.h"
// Haar特征检测 - 人脸识别   
#include <opencv2/opencv.hpp>
#include <cstdio>  
#include <cstdlib>  
#include <Windows.h>  
using namespace std;


int main()
{
	// 加载Haar特征检测分类器  
	// haarcascade_frontalface_alt.xml系OpenCV自带的分类器 下面是我机器上的文件路径  
	//const char *pstrCascadeFileName = "e://opencv2.3.1//data//haarcascades//haarcascade_frontalface_alt.xml";
	const char *pstrCascadeFileName = "D://opencv//build//data//haarcascades//haarcascade_frontalface_alt.xml";
	CvHaarClassifierCascade *pHaarCascade = NULL;
	//加载分类器文件
	pHaarCascade = (CvHaarClassifierCascade*)cvLoad(pstrCascadeFileName);

	// 载入图像  
	const char *pstrImageName = "d://8.jpg";
	//加载图像
	IplImage *pSrcImage = cvLoadImage(pstrImageName, CV_LOAD_IMAGE_UNCHANGED);
	//图像灰度处理
	IplImage *pGrayImage = cvCreateImage(cvGetSize(pSrcImage), IPL_DEPTH_8U, 1);
	cvCvtColor(pSrcImage, pGrayImage, CV_BGR2GRAY);

	// 人脸识别与标记  
	if (pHaarCascade != NULL)
	{
		//颜色列表
		CvScalar FaceCirclecolors[] =
		{
			{ 0, 0, 255 } ,
			{ 0, 128, 255 } ,
			{ 0, 255, 255 } ,
			{ 0, 255, 0 } ,
			{ 255, 128, 0  } ,
			{ 255, 255, 0  } ,
			{ 255, 0, 0 } ,
			{ 255, 0, 255 } 
		};
		//Storage：存储
		CvMemStorage *pcvMStorage = cvCreateMemStorage(0);
		//清空数据
		cvClearMemStorage(pcvMStorage);
		// 识别  
		DWORD dwTimeBegin, dwTimeEnd;
		dwTimeBegin = GetTickCount();
		CvSeq *pcvSeqFaces = cvHaarDetectObjects(pGrayImage, pHaarCascade, pcvMStorage);
		dwTimeEnd = GetTickCount();

		printf("人脸个数: %d   识别用时: %d ms\n", pcvSeqFaces->total, dwTimeEnd - dwTimeBegin);

		// 标记  
		for (int i = 0; i <pcvSeqFaces->total; i++)
		{
			CvRect* r = (CvRect*)cvGetSeqElem(pcvSeqFaces, i);
			//画圆
			//CvPoint center;
			//int radius;
			//center.x = cvRound((r->x + r->width * 0.5));
			//center.y = cvRound((r->y + r->height * 0.5));
			//radius = cvRound((r->width + r->height) * 0.25);
			//cvCircle(pSrcImage, center, radius, FaceCirclecolors[i % 8], 2);
			CvRect rr=cvRect(r->x,r->y,r->width,r->height);
			cvRectangleR(pSrcImage, rr, FaceCirclecolors[i % 8],2);
		}
		//释放内存资源
		cvReleaseMemStorage(&pcvMStorage);
	}

	const char *pstrWindowsTitle = "人脸识别";
	cvNamedWindow(pstrWindowsTitle, CV_WINDOW_AUTOSIZE);
	cvShowImage(pstrWindowsTitle, pSrcImage);

	cvWaitKey(0);

	cvDestroyWindow(pstrWindowsTitle);
	cvReleaseImage(&pSrcImage);
	cvReleaseImage(&pGrayImage);
	return 0;
}
