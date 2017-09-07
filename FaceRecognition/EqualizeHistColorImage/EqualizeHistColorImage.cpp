// EqualizeHistColorImage.cpp : �������̨Ӧ�ó������ڵ㡣
//http://blog.csdn.net/morewindows/article/details/8364722
//�ھ�ƪ ��ɫֱ��ͼ���⻯
#include "stdafx.h"
#include <opencv2/opencv.hpp>  
using namespace std;
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")  
//��ɫͼ���ֱ��ͼ���⻯  
IplImage* EqualizeHistColorImage(IplImage *pImage)
{
	IplImage *pEquaImage = cvCreateImage(cvGetSize(pImage), pImage->depth, 3);

	// ԭͼ��ֳɸ�ͨ�����پ��⻯,���ϲ�����ɫͼ���ֱ��ͼ���⻯  
	const int MAX_CHANNEL = 4;
	IplImage *pImageChannel[MAX_CHANNEL] = { NULL };

	int i;
	for (i = 0; i < pImage->nChannels; i++)
		pImageChannel[i] = cvCreateImage(cvGetSize(pImage), pImage->depth, 1);

	cvSplit(pImage, pImageChannel[0], pImageChannel[1], pImageChannel[2], pImageChannel[3]);

	for (i = 0; i < pImage->nChannels; i++)
		cvEqualizeHist(pImageChannel[i], pImageChannel[i]);

	cvMerge(pImageChannel[0], pImageChannel[1], pImageChannel[2], pImageChannel[3], pEquaImage);

	for (i = 0; i < pImage->nChannels; i++)
		cvReleaseImage(&pImageChannel[i]);

	return pEquaImage;
}
int main(int argc, char** argv)
{
	const char *pstrWindowsSrcTitle = "ԭͼ";
	const char *pstrWindowsHisEquaTitle = "ֱ��ͼ���⻯��";

	// ���ļ��м���ԭͼ  
	IplImage *pSrcImage = cvLoadImage("d://8.jpg", CV_LOAD_IMAGE_UNCHANGED);
	IplImage *pHisEquaImage = EqualizeHistColorImage(pSrcImage);

	cvNamedWindow(pstrWindowsSrcTitle, CV_WINDOW_AUTOSIZE);
	cvNamedWindow(pstrWindowsHisEquaTitle, CV_WINDOW_AUTOSIZE);
	cvShowImage(pstrWindowsSrcTitle, pSrcImage);
	cvShowImage(pstrWindowsHisEquaTitle, pHisEquaImage);


	cvWaitKey(0);

	cvDestroyWindow(pstrWindowsSrcTitle);
	cvDestroyWindow(pstrWindowsHisEquaTitle);
	cvReleaseImage(&pSrcImage);
	cvReleaseImage(&pHisEquaImage);
	return 0;
}

