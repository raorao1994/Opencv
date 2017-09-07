// ImageZoom.cpp : �������̨Ӧ�ó������ڵ㡣
//http://blog.csdn.net/morewindows/article/category/1291764
//�ڶ�ƪ ����ͼ��

#include "stdafx.h"
#include <opencv2/opencv.hpp>  
using namespace std;
//���ؿ���̨����  
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")  
//�ڶ��Σ�ͼƬ����
int main()
{
	const char *pstrImageName = "D://8.jpg";
	const char *pstrSaveImageName = "D://8����ͼ.jpg";
	const char *pstrWindowsSrcTitle = "ԭͼ ";
	const char *pstrWindowsDstTitle = "����ͼ ";

	double fScale = 0.314;      //���ű���  
	CvSize czSize;              //Ŀ��ͼ��ߴ�  

								//���ļ��ж�ȡͼ��    
	IplImage *pSrcImage = cvLoadImage(pstrImageName, CV_LOAD_IMAGE_UNCHANGED);
	IplImage *pDstImage = NULL;

	//����Ŀ��ͼ���С  
	czSize.width = pSrcImage->width * fScale;
	czSize.height = pSrcImage->height * fScale;

	//����ͼ������  
	/*
	��һ��������ʾͼ��Ĵ�С��
	�ڶ���������ʾͼ�����ȣ�����ΪIPL_DEPTH_8U��IPL_DEPTH_16U�ȵȡ�
	������������ʾͼ���ͨ������
	*/
	pDstImage = cvCreateImage(czSize, pSrcImage->depth, pSrcImage->nChannels);
	/*
		��һ��������ʾ����ͼ��
		�ڶ���������ʾ���ͼ��
		������������ʾ��ֵ�������������������֣�
		CV_INTER_NN - ����ڲ�ֵ,
		CV_INTER_LINEAR - ˫���Բ�ֵ (ȱʡʹ��)
		CV_INTER_AREA - ʹ�����ع�ϵ�ز�������ͼ����Сʱ�򣬸÷������Ա��Ⲩ�Ƴ��֡���ͼ��Ŵ�ʱ�������� CV_INTER_NN ����..
		CV_INTER_CUBIC - ������ֵ.
	*/
	cvResize(pSrcImage, pDstImage, CV_INTER_AREA);

	//��������  
	cvNamedWindow(pstrWindowsSrcTitle, CV_WINDOW_AUTOSIZE);
	cvNamedWindow(pstrWindowsDstTitle, CV_WINDOW_AUTOSIZE);

	//��ָ����������ʾͼ��  
	cvShowImage(pstrWindowsSrcTitle, pSrcImage);
	cvShowImage(pstrWindowsDstTitle, pDstImage);

	//�ȴ������¼�  
	cvWaitKey();

	//����ͼƬ  
	cvSaveImage(pstrSaveImageName, pDstImage);

	cvDestroyWindow(pstrWindowsSrcTitle);
	cvDestroyWindow(pstrWindowsDstTitle);
	cvReleaseImage(&pSrcImage);
	cvReleaseImage(&pDstImage);
	return 0;
}

