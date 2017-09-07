// ContourDetection.cpp : �������̨Ӧ�ó������ڵ㡣
//ѧϰ��ַ��http://blog.csdn.net/morewindows/article/details/8253174
//����ƪ �������
#include "stdafx.h"
#include <opencv2/opencv.hpp>  
using namespace std;
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")  

int ContourDetection1()
{
	const char *pstrWindowsSrcTitle = "ԭͼ";
	const char *pstrWindowsOutLineTitle = "����ͼ";

	const int IMAGE_WIDTH = 400;
	const int IMAGE_HEIGHT = 200;

	// ����ͼ��  
	IplImage *pSrcImage = cvCreateImage(cvSize(IMAGE_WIDTH, IMAGE_HEIGHT), IPL_DEPTH_8U, 3);
	// ���ɰ�ɫ  
	cvRectangle(pSrcImage, cvPoint(0, 0), cvPoint(pSrcImage->width, pSrcImage->height), CV_RGB(255, 255, 255), CV_FILLED);
	// ��Բ  
	CvPoint ptCircleCenter = cvPoint(IMAGE_WIDTH / 4, IMAGE_HEIGHT / 2);
	int nRadius = 80;
	cvCircle(pSrcImage, ptCircleCenter, nRadius, CV_RGB(255, 255, 0), CV_FILLED);
	ptCircleCenter = cvPoint(IMAGE_WIDTH / 4, IMAGE_HEIGHT / 2);
	nRadius = 30;
	cvCircle(pSrcImage, ptCircleCenter, nRadius, CV_RGB(255, 255, 255), CV_FILLED);
	// ������  
	CvPoint ptLeftTop = cvPoint(IMAGE_WIDTH / 2 + 20, 20);
	CvPoint ptRightBottom = cvPoint(IMAGE_WIDTH - 20, IMAGE_HEIGHT - 20);
	cvRectangle(pSrcImage, ptLeftTop, ptRightBottom, CV_RGB(0, 255, 255), CV_FILLED);
	ptLeftTop = cvPoint(IMAGE_WIDTH / 2 + 60, 40);
	ptRightBottom = cvPoint(IMAGE_WIDTH - 60, IMAGE_HEIGHT - 40);
	cvRectangle(pSrcImage, ptLeftTop, ptRightBottom, CV_RGB(255, 255, 255), CV_FILLED);
	// ��ʾԭͼ  
	cvNamedWindow(pstrWindowsSrcTitle, CV_WINDOW_AUTOSIZE);
	cvShowImage(pstrWindowsSrcTitle, pSrcImage);


	// תΪ�Ҷ�ͼ  
	IplImage *pGrayImage = cvCreateImage(cvGetSize(pSrcImage), IPL_DEPTH_8U, 1);
	cvCvtColor(pSrcImage, pGrayImage, CV_BGR2GRAY);
	// תΪ��ֵͼ  
	IplImage *pBinaryImage = cvCreateImage(cvGetSize(pGrayImage), IPL_DEPTH_8U, 1);
	cvThreshold(pGrayImage, pBinaryImage, 250, 255, CV_THRESH_BINARY);


	// �������������ؼ�⵽�������ĸ���  
	CvMemStorage *pcvMStorage = cvCreateMemStorage();
	CvSeq *pcvSeq = NULL;
	cvFindContours(pBinaryImage, pcvMStorage, &pcvSeq, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));

	// ������ͼ  
	IplImage *pOutlineImage = cvCreateImage(cvGetSize(pSrcImage), IPL_DEPTH_8U, 3);
	int nLevels = 5;
	// ���ɰ�ɫ  
	cvRectangle(pOutlineImage, cvPoint(0, 0), cvPoint(pOutlineImage->width, pOutlineImage->height), CV_RGB(255, 255, 255), CV_FILLED);
	cvDrawContours(pOutlineImage, pcvSeq, CV_RGB(255, 0, 0), CV_RGB(0, 255, 0), nLevels, 2);
	// ��ʾ����ͼ  
	cvNamedWindow(pstrWindowsOutLineTitle, CV_WINDOW_AUTOSIZE);
	cvShowImage(pstrWindowsOutLineTitle, pOutlineImage);


	cvWaitKey(0);

	cvReleaseMemStorage(&pcvMStorage);

	cvDestroyWindow(pstrWindowsSrcTitle);
	cvDestroyWindow(pstrWindowsOutLineTitle);
	cvReleaseImage(&pSrcImage);
	cvReleaseImage(&pGrayImage);
	cvReleaseImage(&pBinaryImage);
	cvReleaseImage(&pOutlineImage);
	return 0;
}


IplImage *g_pGrayImage = NULL;
const char *pstrWindowsBinaryTitle = "��ֵͼ";
const char *pstrWindowsOutLineTitle = "����ͼ";
CvSeq *g_pcvSeq = NULL;
void on_trackbar(int pos)
{
	// תΪ��ֵͼ  
	IplImage *pBinaryImage = cvCreateImage(cvGetSize(g_pGrayImage), IPL_DEPTH_8U, 1);
	cvThreshold(g_pGrayImage, pBinaryImage, pos, 255, CV_THRESH_BINARY);
	// ��ʾ��ֵͼ  
	cvShowImage(pstrWindowsBinaryTitle, pBinaryImage);

	CvMemStorage* cvMStorage = cvCreateMemStorage();
	// �������������ؼ�⵽�������ĸ���  
	cvFindContours(pBinaryImage, cvMStorage, &g_pcvSeq);

	IplImage *pOutlineImage = cvCreateImage(cvGetSize(g_pGrayImage), IPL_DEPTH_8U, 3);
	int _levels = 5;
	cvZero(pOutlineImage);
	cvDrawContours(pOutlineImage, g_pcvSeq, CV_RGB(255, 0, 0), CV_RGB(0, 255, 0), _levels);
	cvShowImage(pstrWindowsOutLineTitle, pOutlineImage);

	cvReleaseMemStorage(&cvMStorage);
	cvReleaseImage(&pBinaryImage);
	cvReleaseImage(&pOutlineImage);
}

int ContourDetection2()
{
	const char *pstrImageName = "D://8.JPG";
	const char *pstrWindowsSrcTitle = "ԭͼ";
	const char *pstrWindowsToolBarName = "��ֵ��";

	// ���ļ��м���ԭͼ  
	IplImage *pSrcImage = cvLoadImage(pstrImageName, CV_LOAD_IMAGE_UNCHANGED);
	// ��ʾԭͼ  
	cvNamedWindow(pstrWindowsSrcTitle, CV_WINDOW_AUTOSIZE);
	cvShowImage(pstrWindowsSrcTitle, pSrcImage);

	// תΪ�Ҷ�ͼ  
	g_pGrayImage = cvCreateImage(cvGetSize(pSrcImage), IPL_DEPTH_8U, 1);
	cvCvtColor(pSrcImage, g_pGrayImage, CV_BGR2GRAY);

	// ������ֵͼ������ͼ����  
	cvNamedWindow(pstrWindowsBinaryTitle, CV_WINDOW_AUTOSIZE);
	cvNamedWindow(pstrWindowsOutLineTitle, CV_WINDOW_AUTOSIZE);


	// ������    
	int nThreshold = 0;
	cvCreateTrackbar(pstrWindowsToolBarName, pstrWindowsBinaryTitle, &nThreshold, 254, on_trackbar);

	on_trackbar(1);

	cvWaitKey(0);

	cvDestroyWindow(pstrWindowsSrcTitle);
	cvDestroyWindow(pstrWindowsBinaryTitle);
	cvDestroyWindow(pstrWindowsOutLineTitle);
	cvReleaseImage(&pSrcImage);
	cvReleaseImage(&g_pGrayImage);
	return 0;
}


int main(int argc, char** argv)
{
	int i=ContourDetection1();
	//int i = ContourDetection2();
	return 0;
}



