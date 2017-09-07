// LineAndCircleDetection.cpp : �������̨Ӧ�ó������ڵ㡣
//http://blog.csdn.net/morewindows/article/details/8266985
//����ƪ �߶μ����Բ���
#include "stdafx.h"
#include <opencv2/opencv.hpp>  
using namespace std;
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")  
int main()
{
	const char *pstrWindowsSrcTitle = "ԭͼ";
	const char *pstrWindowsLineName = "�߶μ��";

	// ���ļ��м���ԭͼ  
	IplImage *pSrcImage = cvLoadImage("D://9.JPG", CV_LOAD_IMAGE_UNCHANGED);
	// �Ҷ�ͼ  
	IplImage *pGrayImage = cvCreateImage(cvGetSize(pSrcImage), IPL_DEPTH_8U, 1);
	cvCvtColor(pSrcImage, pGrayImage, CV_BGR2GRAY);
	// ��Եͼ  
	IplImage *pCannyImage = cvCreateImage(cvGetSize(pSrcImage), IPL_DEPTH_8U, 1);
	cvCanny(pGrayImage, pCannyImage, 30, 90);
	//cvSmooth(pCannyImage, pCannyImage);  

	// �߶μ��(ֻ����Զ�ֵͼ��)  
	CvMemStorage *pcvMStorage = cvCreateMemStorage();
	double fRho = 1;
	double fTheta = CV_PI / 180;
	int nMaxLineNumber = 50;   //�������ֱ��  
	double fMinLineLen = 50;   //��С�߶γ���  
	double fMinLineGap = 10;   //��С�߶μ��  
	CvSeq *pcvSeqLines = cvHoughLines2(pCannyImage, pcvMStorage, CV_HOUGH_PROBABILISTIC, fRho, fTheta, nMaxLineNumber, fMinLineLen, fMinLineGap);

	// �����߶�  
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

