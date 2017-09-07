// ImageZoom.cpp : 定义控制台应用程序的入口点。
//http://blog.csdn.net/morewindows/article/category/1291764
//第二篇 缩放图像

#include "stdafx.h"
#include <opencv2/opencv.hpp>  
using namespace std;
//隐藏控制台窗口  
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")  
//第二课：图片缩放
int main()
{
	const char *pstrImageName = "D://8.jpg";
	const char *pstrSaveImageName = "D://8缩放图.jpg";
	const char *pstrWindowsSrcTitle = "原图 ";
	const char *pstrWindowsDstTitle = "缩放图 ";

	double fScale = 0.314;      //缩放倍数  
	CvSize czSize;              //目标图像尺寸  

								//从文件中读取图像    
	IplImage *pSrcImage = cvLoadImage(pstrImageName, CV_LOAD_IMAGE_UNCHANGED);
	IplImage *pDstImage = NULL;

	//计算目标图像大小  
	czSize.width = pSrcImage->width * fScale;
	czSize.height = pSrcImage->height * fScale;

	//创建图像并缩放  
	/*
	第一个参数表示图像的大小。
	第二个参数表示图像的深度，可以为IPL_DEPTH_8U，IPL_DEPTH_16U等等。
	第三个参数表示图像的通道数。
	*/
	pDstImage = cvCreateImage(czSize, pSrcImage->depth, pSrcImage->nChannels);
	/*
		第一个参数表示输入图像。
		第二个参数表示输出图像。
		第三个参数表示插值方法，可以有以下四种：
		CV_INTER_NN - 最近邻插值,
		CV_INTER_LINEAR - 双线性插值 (缺省使用)
		CV_INTER_AREA - 使用象素关系重采样。当图像缩小时候，该方法可以避免波纹出现。当图像放大时，类似于 CV_INTER_NN 方法..
		CV_INTER_CUBIC - 立方插值.
	*/
	cvResize(pSrcImage, pDstImage, CV_INTER_AREA);

	//创建窗口  
	cvNamedWindow(pstrWindowsSrcTitle, CV_WINDOW_AUTOSIZE);
	cvNamedWindow(pstrWindowsDstTitle, CV_WINDOW_AUTOSIZE);

	//在指定窗口中显示图像  
	cvShowImage(pstrWindowsSrcTitle, pSrcImage);
	cvShowImage(pstrWindowsDstTitle, pDstImage);

	//等待按键事件  
	cvWaitKey();

	//保存图片  
	cvSaveImage(pstrSaveImageName, pDstImage);

	cvDestroyWindow(pstrWindowsSrcTitle);
	cvDestroyWindow(pstrWindowsDstTitle);
	cvReleaseImage(&pSrcImage);
	cvReleaseImage(&pDstImage);
	return 0;
}

