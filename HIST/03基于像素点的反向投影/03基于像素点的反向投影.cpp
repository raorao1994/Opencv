// 03�������ص�ķ���ͶӰ.cpp : �������̨Ӧ�ó������ڵ㡣
// �������ص�ķ���ͶӰ��cvCalcBackProject��

#include "stdafx.h"
#include <stdlib.h>  
#include <stdio.h> 
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;

int main()
{
	IplImage* src1, *src2, *Imask, *hsv1, *hsv2;    //Դͼ�� HSV��ʽͼ��

	if (!(src1 = cvLoadImage("../data/2.png")))
		return -1;
	if (!(src2 = cvLoadImage("../data/3.png")))
		return -2;
	//�˴�����ͼ������ӦΪ��ͨ��
	if (!(Imask = cvLoadImage("../data/3.png", CV_LOAD_IMAGE_GRAYSCALE)))
		return -3;

	//����ͼ��λ����������µ����봦��ģ��
	cvXorS(Imask, cvScalar(255), Imask);
	cvSet(src1, cvScalarAll(0), Imask);

	hsv1 = cvCreateImage(cvGetSize(src1), src1->depth, src1->nChannels);
	hsv2 = cvCreateImage(cvGetSize(src2), src2->depth, src2->nChannels);

	cvCvtColor(src1, hsv1, CV_BGR2HSV); //Դͼ��->HSV��ʽͼ��
	cvCvtColor(src2, hsv2, CV_BGR2HSV); //Դͼ��->HSV��ʽͼ��

	//����ͶӰͼ��
	IplImage *back_projection = cvCreateImage(cvGetSize(src2), IPL_DEPTH_8U, 1);

	//ɫ��(hue) ���Ͷ�(saturation) ����(value)
	IplImage *h_plane_1 = cvCreateImage(cvSize(hsv1->width, hsv1->height), IPL_DEPTH_8U, 1);
	IplImage *s_plane_1 = cvCreateImage(cvSize(hsv1->width, hsv1->height), IPL_DEPTH_8U, 1);
	IplImage *v_plane_1 = cvCreateImage(cvSize(hsv1->width, hsv1->height), IPL_DEPTH_8U, 1);

	IplImage *h_plane_2 = cvCreateImage(cvSize(hsv2->width, hsv2->height), IPL_DEPTH_8U, 1);
	IplImage *s_plane_2 = cvCreateImage(cvSize(hsv2->width, hsv2->height), IPL_DEPTH_8U, 1);
	IplImage *v_plane_2 = cvCreateImage(cvSize(hsv2->width, hsv2->height), IPL_DEPTH_8U, 1);

	IplImage *planes1[] = { h_plane_1, s_plane_1 };                     //ɫ�౥�Ͷ�����
	IplImage *planes2[] = { h_plane_2, s_plane_2 };                     //ɫ�౥�Ͷ�����

	cvSplit(hsv1, h_plane_1, s_plane_1, v_plane_1, NULL);//ͼ��ָ�
	cvSplit(hsv2, h_plane_2, s_plane_2, v_plane_2, NULL);//ͼ��ָ�

	int h_bins = 30, s_bins = 32;

	//����ֱ��ͼ
	CvHistogram *hist_model, *hist_test;

	int hist_size[] = { h_bins, s_bins };       //��Ӧά������bins����������
	float h_ranges[] = { 0, 180 };              //Hͨ�����ַ�Χ ���Ͷ�0-180
	float s_ranges[] = { 0, 255 };              //Sͨ�����ַ�Χ
	float* ranges[] = { h_ranges, s_ranges };   //���ַ�Χ����, ****����bin��rangeֻҪ�����С�߽�
												//����ֱ��ͼ ��ά������Ӧά��bins�������ܼ�����ʽ�洢�����ַ�Χ���ԣ�����ֱ��ͼ��
	hist_model = cvCreateHist(2, hist_size, CV_HIST_ARRAY, ranges, 1);
	hist_test = cvCreateHist(2, hist_size, CV_HIST_ARRAY, ranges, 1);


	cvCalcHist(planes1, hist_model, 0, 0);      //����ֱ��ͼ��ͼ��ֱ��ͼ�ṹ�����ۼӣ����룩
	cvCalcHist(planes2, hist_test, 0, 0);       //����ֱ��ͼ��ͼ��ֱ��ͼ�ṹ�����ۼӣ����룩

	//cvNormalizeHist(hist_model, 1.0);         //ֱ��ͼ��һ��
	//cvNormalizeHist(hist_test, 1.0);          //ֱ��ͼ��һ��

	//���ƿ��ӻ�ֱ��ͼ
	int scale = 10;
	IplImage* hist_img_model = cvCreateImage(cvSize(h_bins*scale, s_bins*scale), 8, 3); //300*320
	IplImage* hist_img_test = cvCreateImage(cvSize(h_bins*scale, s_bins*scale), 8, 3);  //300*320

	cvZero(hist_img_model);
	cvZero(hist_img_test);

	//��С�Ҷȿ����ͼ��
	float max_value_model = 0;
	float max_value_test = 0;

	cvGetMinMaxHistValue(hist_model, NULL, &max_value_model, NULL, NULL);   //��ȡֱ��ͼ���ֵ
	cvGetMinMaxHistValue(hist_test, NULL, &max_value_test, NULL, NULL);     //��ȡֱ��ͼ���ֵ

	for (int h = 0; h < h_bins; h++)
	{
		for (int s = 0; s < s_bins; s++)
		{

			float bin_val_model = cvGetReal2D(hist_model->bins, h, s);    //��ȡֱ��ͼ��Ӧbin�еĸ�����
			float bin_val_test = cvGetReal2D(hist_test->bins, h, s);      //��ȡֱ��ͼ��Ӧbin�еĸ�����

			int intensity1 = cvRound(bin_val_model * 255 / max_value_model);//ӳ�䵽255�ռ�
			int intensity2 = cvRound(bin_val_test * 255 / max_value_test);  //��һ��̫С

			cvRectangle(hist_img_model, cvPoint(h*scale, s*scale),
				cvPoint((h + 1)*scale - 1, (s + 1)*scale - 1),
				CV_RGB(intensity1, intensity1, intensity1), CV_FILLED);
			cvRectangle(hist_img_test, cvPoint(h*scale, s*scale),
				cvPoint((h + 1)*scale - 1, (s + 1)*scale - 1),
				CV_RGB(intensity2, intensity2, intensity2), CV_FILLED);
		}
	}

	cvCalcBackProject(planes2, back_projection, hist_model);        //���ص�ķ���ͶӰ

	cvNamedWindow("Mask", 1);
	cvNamedWindow("Model", 1);
	cvNamedWindow("Test", 1);
	cvNamedWindow("HIST_Model", 1);
	cvNamedWindow("HIST_Test", 1);
	cvNamedWindow("BACK_Projection", 1);

	cvShowImage("Mask", Imask);
	cvShowImage("Model", src1);
	cvShowImage("Test", src2);
	cvShowImage("HIST_Model", hist_img_model);
	cvShowImage("HIST_Test", hist_img_test);
	cvShowImage("BACK_Projection", back_projection);

	cvWaitKey(0);
	//system("pause");

	cvReleaseHist(&hist_model);
	cvReleaseHist(&hist_test);

	cvReleaseImage(&Imask);
	cvReleaseImage(&src1);
	cvReleaseImage(&src2);
	cvReleaseImage(&hist_img_model);
	cvReleaseImage(&hist_img_test);
	cvReleaseImage(&h_plane_1);
	cvReleaseImage(&s_plane_1);
	cvReleaseImage(&v_plane_1);
	cvReleaseImage(&h_plane_2);
	cvReleaseImage(&s_plane_2);
	cvReleaseImage(&v_plane_2);
	cvReleaseImage(&back_projection);

	cvDestroyAllWindows();
}

