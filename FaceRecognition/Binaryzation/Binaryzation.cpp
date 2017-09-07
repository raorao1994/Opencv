// Binaryzation.cpp : �������̨Ӧ�ó������ڵ㡣
//���ĿΣ�ͼ��Ķ�ֵ��
//http://blog.csdn.net/morewindows/article/details/8239678

#include "stdafx.h"
#include <opencv2/opencv.hpp>  
using namespace std;
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")  
IplImage *g_pGrayImage = NULL;
IplImage *g_pBinaryImage = NULL;
const char *pstrWindowsBinaryTitle = "��ֵͼ";
void on_trackbar(int pos)
{
	// תΪ��ֵͼ  
	cvThreshold(g_pGrayImage, g_pBinaryImage, pos, 255, CV_THRESH_BINARY);
	// ��ʾ��ֵͼ  
	cvShowImage(pstrWindowsBinaryTitle, g_pBinaryImage);
}

int main()
{
	const char * imgpath = "D://8.jpg";
	const char *pstrWindowsSrcTitle = "ԭͼ";
	const char *pstrWindowsToolBarName = "��ֵͼ��ֵ";

	// ���ļ��м���ԭͼ  
	IplImage *pSrcImage = cvLoadImage(imgpath, CV_LOAD_IMAGE_UNCHANGED);

	// תΪ�Ҷ�ͼ  
	g_pGrayImage = cvCreateImage(cvGetSize(pSrcImage), IPL_DEPTH_8U, 1);
	cvCvtColor(pSrcImage, g_pGrayImage, CV_BGR2GRAY);

	// ������ֵͼ  
	g_pBinaryImage = cvCreateImage(cvGetSize(g_pGrayImage), IPL_DEPTH_8U, 1);

	// ��ʾԭͼ  
	cvNamedWindow(pstrWindowsSrcTitle, CV_WINDOW_AUTOSIZE);
	cvShowImage(pstrWindowsSrcTitle, pSrcImage);
	// ������ֵͼ����  
	cvNamedWindow(pstrWindowsBinaryTitle, CV_WINDOW_AUTOSIZE);

	// ������    
	int nThreshold = 0;
	/*
	��һ��������ʾ����ͼ�񣬱���Ϊ��ͨ���Ҷ�ͼ��
	�ڶ���������ʾ����ı�Եͼ��Ϊ��ͨ���ڰ�ͼ��
	������������ʾ��ֵ
	���ĸ�������ʾ���ֵ��
	�����������ʾ���㷽����
	��OpenCV��imgproc\types_c.h�п����ҵ����㷽���Ķ��塣
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

