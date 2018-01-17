// 22cvFindContours.cpp : �������̨Ӧ�ó������ڵ㡣
/************************************************************************/
/* ��ȡ�������ַ����Աȼ���������'���ȼ�'����                         */
/************************************************************************/
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>  
using namespace cv;
int main()
{
	IplImage* img = cvLoadImage("1.png", CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* img_temp = cvCreateImage(cvGetSize(img), 8, 1);
	cvThreshold(img, img, 128, 255, CV_THRESH_BINARY);
	CvMemStorage* mem_storage = cvCreateMemStorage(0);
	CvSeq *first_contour = NULL, *c = NULL;    
	// 1��      
	cvNamedWindow("contour1");
	cvCopy(img, img_temp);
	double t = (double)cvGetTickCount();
	cvFindContours(img_temp, mem_storage, &first_contour);
	cvZero(img_temp);
	cvDrawContours(
		img_temp,
		first_contour,
		cvScalar(100),
		cvScalar(100),
		1
	);
	t = (double)cvGetTickCount() - t;
	cvShowImage("contour1", img_temp);
	printf("run1 = %gms\n", t / (cvGetTickFrequency()*1000.));
	cvClearMemStorage(mem_storage);
	// 2��      
	cvNamedWindow("contour2");
	cvCopy(img, img_temp);
	t = (double)cvGetTickCount();
	CvContourScanner scanner = cvStartFindContours(img_temp, mem_storage);
	while (cvFindNextContour(scanner));
	first_contour = cvEndFindContours(&scanner);
	cvZero(img_temp);
	cvDrawContours(
		img_temp,
		first_contour,
		cvScalar(100),
		cvScalar(100),
		1
	);
	t = (double)cvGetTickCount() - t;
	cvShowImage("contour2", img_temp);
	printf("run2 = %gms\n", t / (cvGetTickFrequency()*1000.));
	cvClearMemStorage(mem_storage);
	cvReleaseImage(&img);
	cvReleaseImage(&img_temp);
	/************************************************************************/
	/* ������ run1 = 16.1431ms run2 = 15.8677ms (�ο�)
	�������Կ϶��������㷨ʱ�临�Ӷ�����ͬ��                                     */
	/************************************************************************/
	//////////////////////////////////////////////////////////////////////////      
	// �������ַ�������˶���������ȡ,������������������cvDrawContours��ʹ��      
	// ��cvDrawContours ������5������Ϊ max_level ����ICVL��������:      
	//      
	// �������������ȼ�������ȼ�Ϊ0�����Ƶ��������������Ϊ1��������������������ͬ�ļ�����������      
	// ���ֵΪ2�����е�����������ȼ�Ϊ2����������ͬ�����������е�һ��������������֡����ֵΪ������      
	// ����������ͬ�������������������ֱ������Ϊabs(max_level)-1����������      
	//      
	// ���źö���߳��ζ��޷����ȼ��ĺ���,���Ҳ���ʱ������>=1 ������Ч������һ��      
	// ֻ����ȡ����ʱ�����ȡģʽ��Ϊ CV_RETR_CCOMP CV_RETR_TREE ʱ���������������      
	//      
	// ����FindContours ������������������ȡģʽ(mode)������������:      
	// CV_RETR_CCOMP - ��ȡ�������������ҽ�����֯Ϊ����� hierarchy: ����Ϊ��ͨ�����Χ�߽磬�β�Ϊ�����ڲ�߽硣       
	// CV_RETR_TREE - ��ȡ���������������ع�Ƕ��������ȫ�� hierarchy       
	//       
	// �����õ�һ�ַ������в���      

	cvNamedWindow("contour_test");
	cvNamedWindow("contour_raw");
	img = cvLoadImage("1.png", CV_LOAD_IMAGE_GRAYSCALE);
	cvShowImage("contour_raw", img);
	cvThreshold(img, img, 128, 255, CV_THRESH_BINARY);
	img_temp = cvCloneImage(img);
	cvFindContours(
		img_temp,
		mem_storage,
		&first_contour,
		sizeof(CvContour),
		CV_RETR_CCOMP           //#1 ���������      
	);

	cvZero(img_temp);
	cvDrawContours(
		img_temp,
		first_contour,
		cvScalar(100),
		cvScalar(100),
		1                       //#2 ���������      
	);
	cvShowImage("contour_test", img_temp);
	/************************************************************************/
	/* (1, 2) = (CV_RETR_CCOMP, 1)  ��ͼ1
	(1, 2) = (CV_RETR_CCOMP, 2)  ��ͼ2
	(1, 2) = (CV_RETR_TREE, 1)   ��ͼ3
	(1, 2) = (CV_RETR_TREE, 2)   ��ͼ4
	(1, 2) = (CV_RETR_TREE, 6)   ��ͼ5
	������CV_RETR_CCOMP ֻ��ͼ���Ϊ�������,����ʹβ�,һ�ȼ�����ֻƥ��������ӽ�
	���ڲ�������2�ȼ�
	CV_RETR_TREE ��������⵽�ڰ��ȼ�1 - n ȫ������
	CV_RETR_LIST ȫ��������Ϊ1��                        */
	/************************************************************************/
	cvWaitKey();
	cvReleaseImage(&img);
	cvReleaseImage(&img_temp);
	cvReleaseMemStorage(&mem_storage);
	cvDestroyAllWindows();
	return 0;
}