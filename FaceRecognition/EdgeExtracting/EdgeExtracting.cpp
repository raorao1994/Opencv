// EdgeExtracting.cpp : �������̨Ӧ�ó������ڵ㡣
//http://blog.csdn.net/morewindows/article/category/1291764
#include "stdafx.h"
//�����Σ�ͼ���Canny��Ե���  
#include <opencv2/opencv.hpp>  
using namespace std;
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")  
IplImage *g_pSrcImage, *g_pCannyImg;
const char *pstrWindowsCannyTitle = "��Ե���ͼ";
//cvCreateTrackbar�Ļص�����  
void on_trackbar(int threshold)
{
	/*
	��trackbarλ�ñ��ı��ʱ��ϵͳ���������ص���������������pos����Ϊ��ʾtrackbarλ�õ���ֵ��
	*/
	//canny��Ե���  
	cvCanny(g_pSrcImage, g_pCannyImg, threshold, threshold * 3, 3);
	cvShowImage(pstrWindowsCannyTitle, g_pCannyImg);
}
int main()
{
	const char *pstrImageName = "D://8.JPG";
	const char *pstrWindowsSrcTitle = "ԭͼ";
	const char *pstrWindowsToolBar = "Threshold";

	//���ļ�������ͼ��ĻҶ�ͼCV_LOAD_IMAGE_GRAYSCALE - �Ҷ�ͼ  
	g_pSrcImage = cvLoadImage(pstrImageName, CV_LOAD_IMAGE_GRAYSCALE);
	g_pCannyImg = cvCreateImage(cvGetSize(g_pSrcImage), IPL_DEPTH_8U, 1);

	//��������  
	cvNamedWindow(pstrWindowsSrcTitle, CV_WINDOW_AUTOSIZE);
	cvNamedWindow(pstrWindowsCannyTitle, CV_WINDOW_AUTOSIZE);

	//����������  
	int nThresholdEdge = 1;
	/*  
		��һ��������ʾ��trackbar�����ơ�
		�ڶ���������ʾ�������ƣ���trackbar����ʾ����������ڡ�
		������������ʾ����ʱ�����λ�á�
		���ĸ�������ʾ����λ�õ����ֵ����Сֵ�̶�Ϊ0��
		�����������ʾ�ص�������������λ���б仯ʱ��ϵͳ����øûص�������
	*/
	cvCreateTrackbar(pstrWindowsToolBar, pstrWindowsCannyTitle, &nThresholdEdge, 100, on_trackbar);
	
	//��ָ����������ʾͼ��  
	cvShowImage(pstrWindowsSrcTitle, g_pSrcImage);
	on_trackbar(1);

	//�ȴ������¼�  
	cvWaitKey();

	cvDestroyWindow(pstrWindowsSrcTitle);
	cvDestroyWindow(pstrWindowsCannyTitle);
	cvReleaseImage(&g_pSrcImage);
	cvReleaseImage(&g_pCannyImg);
	return 0;
}

