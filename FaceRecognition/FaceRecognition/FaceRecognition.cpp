// FaceRecognition.cpp : �������̨Ӧ�ó������ڵ㡣
//http://blog.csdn.net/morewindows/article/details/8426318
//��ʮ��ƪ �������
#include "stdafx.h"
// Haar������� - ����ʶ��   
#include <opencv2/opencv.hpp>
#include <cstdio>  
#include <cstdlib>  
#include <Windows.h>  
using namespace std;


int main()
{
	// ����Haar������������  
	// haarcascade_frontalface_alt.xmlϵOpenCV�Դ��ķ����� �������һ����ϵ��ļ�·��  
	//const char *pstrCascadeFileName = "e://opencv2.3.1//data//haarcascades//haarcascade_frontalface_alt.xml";
	const char *pstrCascadeFileName = "D://opencv//build//data//haarcascades//haarcascade_frontalface_alt.xml";
	CvHaarClassifierCascade *pHaarCascade = NULL;
	//���ط������ļ�
	pHaarCascade = (CvHaarClassifierCascade*)cvLoad(pstrCascadeFileName);

	// ����ͼ��  
	const char *pstrImageName = "d://8.jpg";
	//����ͼ��
	IplImage *pSrcImage = cvLoadImage(pstrImageName, CV_LOAD_IMAGE_UNCHANGED);
	//ͼ��Ҷȴ���
	IplImage *pGrayImage = cvCreateImage(cvGetSize(pSrcImage), IPL_DEPTH_8U, 1);
	cvCvtColor(pSrcImage, pGrayImage, CV_BGR2GRAY);

	// ����ʶ������  
	if (pHaarCascade != NULL)
	{
		//��ɫ�б�
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
		//Storage���洢
		CvMemStorage *pcvMStorage = cvCreateMemStorage(0);
		//�������
		cvClearMemStorage(pcvMStorage);
		// ʶ��  
		DWORD dwTimeBegin, dwTimeEnd;
		dwTimeBegin = GetTickCount();
		CvSeq *pcvSeqFaces = cvHaarDetectObjects(pGrayImage, pHaarCascade, pcvMStorage);
		dwTimeEnd = GetTickCount();

		printf("��������: %d   ʶ����ʱ: %d ms\n", pcvSeqFaces->total, dwTimeEnd - dwTimeBegin);

		// ���  
		for (int i = 0; i <pcvSeqFaces->total; i++)
		{
			CvRect* r = (CvRect*)cvGetSeqElem(pcvSeqFaces, i);
			//��Բ
			//CvPoint center;
			//int radius;
			//center.x = cvRound((r->x + r->width * 0.5));
			//center.y = cvRound((r->y + r->height * 0.5));
			//radius = cvRound((r->width + r->height) * 0.25);
			//cvCircle(pSrcImage, center, radius, FaceCirclecolors[i % 8], 2);
			CvRect rr=cvRect(r->x,r->y,r->width,r->height);
			cvRectangleR(pSrcImage, rr, FaceCirclecolors[i % 8],2);
		}
		//�ͷ��ڴ���Դ
		cvReleaseMemStorage(&pcvMStorage);
	}

	const char *pstrWindowsTitle = "����ʶ��";
	cvNamedWindow(pstrWindowsTitle, CV_WINDOW_AUTOSIZE);
	cvShowImage(pstrWindowsTitle, pSrcImage);

	cvWaitKey(0);

	cvDestroyWindow(pstrWindowsTitle);
	cvReleaseImage(&pSrcImage);
	cvReleaseImage(&pGrayImage);
	return 0;
}
