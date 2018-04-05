// HIST.cpp : 定义控制台应用程序的入口点。
//从EMD直方图创建signature，用EMD度量相似性
#include "stdafx.h"
#include <stdlib.h>  
#include <stdio.h> 
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;

int main()
{
	//源图像 HSV格式图像
	IplImage* src1, *src2, *Imask, *hsv1, *hsv2;

	if (!(src1 = cvLoadImage("../data/1.jpg")))
		return -1;
	if (!(src2 = cvLoadImage("../data/4.jpg")))
		return -2;
	if (!(Imask = cvLoadImage("../data/4.jpg", CV_LOAD_IMAGE_GRAYSCALE)))
		return -3;

	hsv1 = cvCreateImage(cvGetSize(src1), src1->depth, src1->nChannels);
	hsv2 = cvCreateImage(cvGetSize(src2), src2->depth, src2->nChannels);

	cvCvtColor(src1, hsv1, CV_BGR2HSV); //源图像->HSV格式图像
	cvCvtColor(src2, hsv2, CV_BGR2HSV); //源图像->HSV格式图像

										//色调(hue) 饱和度(saturation) 明度(value)
	IplImage *h_plane_1 = cvCreateImage(cvSize(hsv1->width, hsv1->height), IPL_DEPTH_8U, 1);
	IplImage *s_plane_1 = cvCreateImage(cvSize(hsv1->width, hsv1->height), IPL_DEPTH_8U, 1);
	IplImage *v_plane_1 = cvCreateImage(cvSize(hsv1->width, hsv1->height), IPL_DEPTH_8U, 1);

	IplImage *h_plane_2 = cvCreateImage(cvSize(hsv2->width, hsv2->height), IPL_DEPTH_8U, 1);
	IplImage *s_plane_2 = cvCreateImage(cvSize(hsv2->width, hsv2->height), IPL_DEPTH_8U, 1);
	IplImage *v_plane_2 = cvCreateImage(cvSize(hsv2->width, hsv2->height), IPL_DEPTH_8U, 1);

	IplImage *planes1[] = { h_plane_1, s_plane_1 };//色相饱和度数组
	IplImage *planes2[] = { h_plane_2, s_plane_2 };//色相饱和度数组

												   //cvCvtPixToPlane(hsv1, h_plane_1, s_plane_1, v_plane_1, NULL);       //图像分割
												   //图像分割
	cvSplit(hsv1, h_plane_1, s_plane_1, v_plane_1, NULL);
	cvSplit(hsv2, h_plane_2, s_plane_2, v_plane_2, NULL);


	int h_bins = 30, s_bins = 32;

	//建立直方图
	CvHistogram *hist1, *hist2;

	int hist_size[] = { h_bins, s_bins };       //对应维数包含bins个数的数组
	float h_ranges[] = { 0, 180 };              //H通道划分范围 饱和度0-180
	float s_ranges[] = { 0, 255 };              //S通道划分范围
	float* ranges[] = { h_ranges, s_ranges };   //划分范围数对, ****均匀bin，range只要最大最小边界
												//创建直方图 （维数，对应维数bins个数，密集矩阵方式存储，划分范围数对，均匀直方图）
	hist1 = cvCreateHist(2, hist_size, CV_HIST_ARRAY, ranges, 1);
	hist2 = cvCreateHist(2, hist_size, CV_HIST_ARRAY, ranges, 1);

	//cvCalcHist(planes1, hist1, 0, 0);       //计算直方图（图像，直方图结构，不累加，掩码）
	//cvCalcHist(planes2, hist2, 0, 0);       //计算直方图（图像，直方图结构，不累加，掩码）

	cvCalcHist(planes1, hist1, 0, Imask);     //计算直方图（图像，直方图结构，不累加，掩码）
	cvCalcHist(planes2, hist2, 0, Imask);     //计算直方图（图像，直方图结构，不累加，掩码）

											  //cvNormalizeHist(hist1, 1.0);            //直方图归一化
											  //cvNormalizeHist(hist2, 1.0);            //直方图归一化


	CvMat *sig1, *sig2;
	int numrows = h_bins*s_bins;

	//numrows行 3列 矩阵
	sig1 = cvCreateMat(numrows, 3, CV_32FC1);
	sig2 = cvCreateMat(numrows, 3, CV_32FC1);

	for (int h = 0; h < h_bins; h++)
	{
		for (int s = 0; s < s_bins; s++)
		{
			//float bin_val = cvQueryHistValue_2D(hist1, h, s);
			float bin_val = cvGetReal2D(hist1->bins, h, s);
			//h:行数 s_bins：总列数（行长度）s:列数  h*s_bins+s 当前bin对应的sig行数   
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
	printf("EMD距离：%f; ", emd);

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


