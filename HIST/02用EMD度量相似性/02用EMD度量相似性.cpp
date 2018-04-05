// HIST.cpp : �������̨Ӧ�ó������ڵ㡣
//��EMDֱ��ͼ����signature����EMD����������
#include "stdafx.h"
#include <stdlib.h>  
#include <stdio.h> 
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;

int main()
{
	//Դͼ�� HSV��ʽͼ��
	IplImage* src1, *src2, *Imask, *hsv1, *hsv2;

	if (!(src1 = cvLoadImage("../data/1.jpg")))
		return -1;
	if (!(src2 = cvLoadImage("../data/4.jpg")))
		return -2;
	if (!(Imask = cvLoadImage("../data/4.jpg", CV_LOAD_IMAGE_GRAYSCALE)))
		return -3;

	hsv1 = cvCreateImage(cvGetSize(src1), src1->depth, src1->nChannels);
	hsv2 = cvCreateImage(cvGetSize(src2), src2->depth, src2->nChannels);

	cvCvtColor(src1, hsv1, CV_BGR2HSV); //Դͼ��->HSV��ʽͼ��
	cvCvtColor(src2, hsv2, CV_BGR2HSV); //Դͼ��->HSV��ʽͼ��

										//ɫ��(hue) ���Ͷ�(saturation) ����(value)
	IplImage *h_plane_1 = cvCreateImage(cvSize(hsv1->width, hsv1->height), IPL_DEPTH_8U, 1);
	IplImage *s_plane_1 = cvCreateImage(cvSize(hsv1->width, hsv1->height), IPL_DEPTH_8U, 1);
	IplImage *v_plane_1 = cvCreateImage(cvSize(hsv1->width, hsv1->height), IPL_DEPTH_8U, 1);

	IplImage *h_plane_2 = cvCreateImage(cvSize(hsv2->width, hsv2->height), IPL_DEPTH_8U, 1);
	IplImage *s_plane_2 = cvCreateImage(cvSize(hsv2->width, hsv2->height), IPL_DEPTH_8U, 1);
	IplImage *v_plane_2 = cvCreateImage(cvSize(hsv2->width, hsv2->height), IPL_DEPTH_8U, 1);

	IplImage *planes1[] = { h_plane_1, s_plane_1 };//ɫ�౥�Ͷ�����
	IplImage *planes2[] = { h_plane_2, s_plane_2 };//ɫ�౥�Ͷ�����

												   //cvCvtPixToPlane(hsv1, h_plane_1, s_plane_1, v_plane_1, NULL);       //ͼ��ָ�
												   //ͼ��ָ�
	cvSplit(hsv1, h_plane_1, s_plane_1, v_plane_1, NULL);
	cvSplit(hsv2, h_plane_2, s_plane_2, v_plane_2, NULL);


	int h_bins = 30, s_bins = 32;

	//����ֱ��ͼ
	CvHistogram *hist1, *hist2;

	int hist_size[] = { h_bins, s_bins };       //��Ӧά������bins����������
	float h_ranges[] = { 0, 180 };              //Hͨ�����ַ�Χ ���Ͷ�0-180
	float s_ranges[] = { 0, 255 };              //Sͨ�����ַ�Χ
	float* ranges[] = { h_ranges, s_ranges };   //���ַ�Χ����, ****����bin��rangeֻҪ�����С�߽�
												//����ֱ��ͼ ��ά������Ӧά��bins�������ܼ�����ʽ�洢�����ַ�Χ���ԣ�����ֱ��ͼ��
	hist1 = cvCreateHist(2, hist_size, CV_HIST_ARRAY, ranges, 1);
	hist2 = cvCreateHist(2, hist_size, CV_HIST_ARRAY, ranges, 1);

	//cvCalcHist(planes1, hist1, 0, 0);       //����ֱ��ͼ��ͼ��ֱ��ͼ�ṹ�����ۼӣ����룩
	//cvCalcHist(planes2, hist2, 0, 0);       //����ֱ��ͼ��ͼ��ֱ��ͼ�ṹ�����ۼӣ����룩

	cvCalcHist(planes1, hist1, 0, Imask);     //����ֱ��ͼ��ͼ��ֱ��ͼ�ṹ�����ۼӣ����룩
	cvCalcHist(planes2, hist2, 0, Imask);     //����ֱ��ͼ��ͼ��ֱ��ͼ�ṹ�����ۼӣ����룩

											  //cvNormalizeHist(hist1, 1.0);            //ֱ��ͼ��һ��
											  //cvNormalizeHist(hist2, 1.0);            //ֱ��ͼ��һ��


	CvMat *sig1, *sig2;
	int numrows = h_bins*s_bins;

	//numrows�� 3�� ����
	sig1 = cvCreateMat(numrows, 3, CV_32FC1);
	sig2 = cvCreateMat(numrows, 3, CV_32FC1);

	for (int h = 0; h < h_bins; h++)
	{
		for (int s = 0; s < s_bins; s++)
		{
			//float bin_val = cvQueryHistValue_2D(hist1, h, s);
			float bin_val = cvGetReal2D(hist1->bins, h, s);
			//h:���� s_bins�����������г��ȣ�s:����  h*s_bins+s ��ǰbin��Ӧ��sig����   
			cvSet2D(sig1, h*s_bins + s, 0, cvScalar(bin_val));
			cvSet2D(sig1, h*s_bins + s, 1, cvScalar(h));
			cvSet2D(sig1, h*s_bins + s, 2, cvScalar(s));

			bin_val = cvGetReal2D(hist2->bins, h, s);
			cvSet2D(sig2, h*s_bins + s, 0, cvScalar(bin_val));
			cvSet2D(sig2, h*s_bins + s, 1, cvScalar(h));
			cvSet2D(sig2, h*s_bins + s, 2, cvScalar(s));
		}
	}

	float emd = cvCalcEMD2(sig1, sig2, CV_DIST_L2);
	printf("EMD���룺%f; ", emd);

	cvNamedWindow("SRC1", 1);
	cvNamedWindow("SRC2", 2);

	cvShowImage("SRC1", src1);
	cvShowImage("SRC2", src2);

	cvWaitKey(0);
	system("pause");

	cvReleaseMat(&sig1);
	cvReleaseMat(&sig2);

	cvReleaseHist(&hist1);
	cvReleaseHist(&hist2);

	cvReleaseImage(&src1);
	cvReleaseImage(&src2);
	cvReleaseImage(&Imask);
	cvReleaseImage(&hsv1);
	cvReleaseImage(&hsv2);
	cvReleaseImage(&h_plane_1);
	cvReleaseImage(&s_plane_1);
	cvReleaseImage(&v_plane_1);
	cvReleaseImage(&h_plane_2);
	cvReleaseImage(&s_plane_2);
	cvReleaseImage(&v_plane_2);

	cvDestroyAllWindows();
}


