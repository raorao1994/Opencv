// 04基于块的反向投影.cpp : 定义控制台应用程序的入口点。
// 基于块的反向投影（cvCalcArrBackProjectPatch）

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
	IplImage* Ihand_model, *Ihand_test, *Ihand_mask, *hsv1, *hsv2, *hsv3, *hsv4, *Icup_model, *Icup_test, *Icup_mask;

	//未处理的肤色模板图像
	if (!(Ihand_model = cvLoadImage("../data/2.png")))
		return -1;
	//寻找手掌反向投影的测试图像
	if (!(Ihand_test = cvLoadImage("../data/3.png")))
		return -2;
	//用于处理肤色模板图像的掩码，此处调入图像掩码应为单通道
	if (!(Ihand_mask = cvLoadImage("../data/2.png",CV_LOAD_IMAGE_GRAYSCALE)))
		return -3;
	//未处理的杯子颜色模板图像
	if (!(Icup_model = cvLoadImage("../data/1.jpg")))
		return -4;
	//寻找杯子反向投影的测试图像
	if (!(Icup_test = cvLoadImage("../data/2.jpg")))
		return -5;
	//用于处理杯子颜色模板图像的掩码 可获取杯子反向投影块大小
	if (!(Icup_mask = cvLoadImage("../data/1.jpg",CV_LOAD_IMAGE_GRAYSCALE)))
		return -6;

	int hand_patch_width = 5;                       //手掌反向投影块宽度
	int hand_patch_height = 5;                      //手掌反向投影块高度

	int cup_patch_width = Icup_model->width;        //杯子反向投影块宽度
	int cup_patch_height = Icup_model->height;      //杯子反向投影块高度

	cvXorS(Ihand_mask, cvScalar(255), Ihand_mask);          //掩码图像按位异或，求反，以手掌以外区域作为掩码
	cvXorS(Icup_mask, cvScalar(255), Icup_mask);            //掩码图像按位异或，求反，以杯子以外区域作为掩码

	cvSet(Ihand_model, cvScalarAll(0), Ihand_mask);         //将手掌以外区域变黑，生成肤色模板图像
	cvSet(Icup_model, cvScalarAll(0), Icup_mask);           //将手掌以外区域变黑，生成肤色模板图像

	hsv1 = cvCreateImage(cvGetSize(Ihand_model), Ihand_model->depth, Ihand_model->nChannels);   //HSV
	hsv2 = cvCreateImage(cvGetSize(Ihand_test), Ihand_test->depth, Ihand_test->nChannels);      //HSV
	hsv3 = cvCreateImage(cvGetSize(Icup_model), Icup_model->depth, Icup_model->nChannels);      //HSV
	hsv4 = cvCreateImage(cvGetSize(Icup_test), Icup_test->depth, Icup_test->nChannels);         //HSV

	cvCvtColor(Ihand_model, hsv1, CV_BGR2HSV);      //源图像->HSV格式图像
	cvCvtColor(Ihand_test, hsv2, CV_BGR2HSV);       //源图像->HSV格式图像
	cvCvtColor(Icup_model, hsv3, CV_BGR2HSV);       //源图像->HSV格式图像
	cvCvtColor(Icup_test, hsv4, CV_BGR2HSV);        //源图像->HSV格式图像

													//反向投影图像 大小:测试图像-块的大小 浮点型数组
	IplImage *Iback_projection_patch_hand = cvCreateImage(
		cvSize(Ihand_test->width - hand_patch_width + 1, Ihand_test->height - hand_patch_height + 1),
		IPL_DEPTH_32F, 1);
	IplImage *Iback_projection_patch_cup = cvCreateImage(
		cvSize(Icup_test->width - cup_patch_width + 1, Icup_test->height - cup_patch_height + 1),
		IPL_DEPTH_32F, 1);

	//色调(hue) 饱和度(saturation) 明度(value) 创建三通道图像
	IplImage *h_plane_1 = cvCreateImage(cvSize(hsv1->width, hsv1->height), IPL_DEPTH_8U, 1);
	IplImage *s_plane_1 = cvCreateImage(cvSize(hsv1->width, hsv1->height), IPL_DEPTH_8U, 1);
	IplImage *v_plane_1 = cvCreateImage(cvSize(hsv1->width, hsv1->height), IPL_DEPTH_8U, 1);

	IplImage *h_plane_2 = cvCreateImage(cvSize(hsv2->width, hsv2->height), IPL_DEPTH_8U, 1);
	IplImage *s_plane_2 = cvCreateImage(cvSize(hsv2->width, hsv2->height), IPL_DEPTH_8U, 1);
	IplImage *v_plane_2 = cvCreateImage(cvSize(hsv2->width, hsv2->height), IPL_DEPTH_8U, 1);

	IplImage *h_plane_3 = cvCreateImage(cvSize(hsv3->width, hsv3->height), IPL_DEPTH_8U, 1);
	IplImage *s_plane_3 = cvCreateImage(cvSize(hsv3->width, hsv3->height), IPL_DEPTH_8U, 1);
	IplImage *v_plane_3 = cvCreateImage(cvSize(hsv3->width, hsv3->height), IPL_DEPTH_8U, 1);

	IplImage *h_plane_4 = cvCreateImage(cvSize(hsv4->width, hsv4->height), IPL_DEPTH_8U, 1);
	IplImage *s_plane_4 = cvCreateImage(cvSize(hsv4->width, hsv4->height), IPL_DEPTH_8U, 1);
	IplImage *v_plane_4 = cvCreateImage(cvSize(hsv4->width, hsv4->height), IPL_DEPTH_8U, 1);

	IplImage *planes1[] = { h_plane_1, s_plane_1 };                     //色相饱和度数组
	IplImage *planes2[] = { h_plane_2, s_plane_2 };                     //色相饱和度数组
	IplImage *planes3[] = { h_plane_3, s_plane_3 };                     //色相饱和度数组
	IplImage *planes4[] = { h_plane_4, s_plane_4 };                     //色相饱和度数组

	cvSplit(hsv1, h_plane_1, s_plane_1, v_plane_1, NULL);       //图像分割
	cvSplit(hsv2, h_plane_2, s_plane_2, v_plane_2, NULL);       //图像分割
	cvSplit(hsv3, h_plane_3, s_plane_3, v_plane_3, NULL);
	cvSplit(hsv4, h_plane_4, s_plane_4, v_plane_4, NULL);

	int h_bins = 30, s_bins = 32;               //h维bins的个数，s维bins的个数               

												//建立模板和测试直方图
	CvHistogram *hist_model_hand, *hist_test_hand, *hist_model_cup, *hist_test_cup;

	int hist_size[] = { h_bins, s_bins };       //对应维数包含bins个数的数组
	float h_ranges[] = { 0, 180 };              //H通道划分范围 饱和度0-180
	float s_ranges[] = { 0, 255 };              //S通道划分范围
	float* ranges[] = { h_ranges, s_ranges };   //划分范围数对, ****均匀bin，range只要最大最小边界

	//创建直方图 （维数，对应维数bins个数，密集矩阵方式存储，划分范围数对，均匀直方图）
	hist_model_hand = cvCreateHist(2, hist_size, CV_HIST_ARRAY, ranges, 1);
	hist_test_hand = cvCreateHist(2, hist_size, CV_HIST_ARRAY, ranges, 1);
	hist_model_cup = cvCreateHist(2, hist_size, CV_HIST_ARRAY, ranges, 1);
	hist_test_cup = cvCreateHist(2, hist_size, CV_HIST_ARRAY, ranges, 1);
	//计算直方图
	cvCalcHist(planes1, hist_model_hand, 0, 0);     //计算直方图（图像，直方图结构，不累加，掩码）肤色直方图
	cvCalcHist(planes2, hist_test_hand, 0, 0);      //计算直方图（图像，直方图结构，不累加，掩码）测试直方图
	cvCalcHist(planes3, hist_model_cup, 0, 0);      //计算直方图（图像，直方图结构，不累加，掩码）杯色直方图
	cvCalcHist(planes4, hist_test_cup, 0, 0);       //计算直方图（图像，直方图结构，不累加，掩码）测试直方图
													//直方图归一化
	cvNormalizeHist(hist_model_hand, 1.0);          //直方图归一化
	cvNormalizeHist(hist_test_hand, 1.0);           //直方图归一化
	cvNormalizeHist(hist_model_cup, 1.0);           //直方图归一化
	cvNormalizeHist(hist_test_cup, 1.0);            //直方图归一化

													//绘制可视化直方图
	int scale = 10;     //直方图颜色值图像显示倍数
	IplImage* Ihist_model_hand = cvCreateImage(cvSize(h_bins*scale, s_bins*scale), 8, 3);   //300*320
	IplImage* Ihist_test_hand = cvCreateImage(cvSize(h_bins*scale, s_bins*scale), 8, 3);    //300*320
	IplImage* Ihist_model_cup = cvCreateImage(cvSize(h_bins*scale, s_bins*scale), 8, 3);    //300*320
	IplImage* Ihist_test_cup = cvCreateImage(cvSize(h_bins*scale, s_bins*scale), 8, 3);     //300*320

																							//直方图颜色值图像清零
	cvZero(Ihist_model_hand);
	cvZero(Ihist_test_hand);
	cvZero(Ihist_model_cup);
	cvZero(Ihist_test_cup);

	//以小灰度块填充图像
	float max_value_model_hand = 0; //直方图中最大值，为映射做准备
	float max_value_test_hand = 0;
	float max_value_model_cup = 0;  //直方图中最大值，为映射做准备
	float max_value_test_cup = 0;

	cvGetMinMaxHistValue(hist_model_hand, NULL, &max_value_model_hand, NULL, NULL); //直方图最大值
	cvGetMinMaxHistValue(hist_test_hand, NULL, &max_value_test_hand, NULL, NULL);   //直方图最大值
	cvGetMinMaxHistValue(hist_model_cup, NULL, &max_value_model_cup, NULL, NULL);   //直方图最大值
	cvGetMinMaxHistValue(hist_test_cup, NULL, &max_value_test_cup, NULL, NULL);     //直方图最大值

	for (int h = 0; h < h_bins; h++)
	{
		for (int s = 0; s < s_bins; s++)
		{
			float bin_val_model_hand = cvGetReal2D(hist_model_hand->bins, h, s);  //bin中的浮点数
			float bin_val_test_hand = cvGetReal2D(hist_test_hand->bins, h, s);    //bin中的浮点数
			float bin_val_model_cup = cvGetReal2D(hist_model_cup->bins, h, s);    //bin中的浮点数
			float bin_val_test_cup = cvGetReal2D(hist_test_cup->bins, h, s);      //bin中的浮点数

			int intensity1 = cvRound(bin_val_model_hand * 255 / max_value_model_hand);  //映射到255空间
			int intensity2 = cvRound(bin_val_test_hand * 255 / max_value_test_hand);    //归一后太小
			int intensity3 = cvRound(bin_val_model_cup * 255 / max_value_model_cup);    //映射到255空间
			int intensity4 = cvRound(bin_val_test_cup * 255 / max_value_test_cup);      //归一后太小

			cvRectangle(Ihist_model_hand, cvPoint(h*scale, s*scale),            //绘制小灰度块填充图像
				cvPoint((h + 1)*scale - 1, (s + 1)*scale - 1),
				CV_RGB(intensity1, intensity1, intensity1), CV_FILLED);
			cvRectangle(Ihist_test_hand, cvPoint(h*scale, s*scale),
				cvPoint((h + 1)*scale - 1, (s + 1)*scale - 1),
				CV_RGB(intensity2, intensity2, intensity2), CV_FILLED);
			cvRectangle(Ihist_model_cup, cvPoint(h*scale, s*scale),         //绘制小灰度块填充图像
				cvPoint((h + 1)*scale - 1, (s + 1)*scale - 1),
				CV_RGB(intensity3, intensity3, intensity3), CV_FILLED);
			cvRectangle(Ihist_test_cup, cvPoint(h*scale, s*scale),
				cvPoint((h + 1)*scale - 1, (s + 1)*scale - 1),
				CV_RGB(intensity4, intensity4, intensity4), CV_FILLED);
		}
	}

	CvSize hand_patch_size = cvSize(hand_patch_width, hand_patch_height);   //手掌反向投影块尺寸
	CvSize cup_patch_size = cvSize(cup_patch_width, cup_patch_height);      //杯子反向投影块尺寸

	//做区域检测器 采样窗口小于目标 测试图像数组 块的反向投影图像 块大小 模板直方图 比较方式（相关） 归一化水平
	cvCalcArrBackProjectPatch((CvArr **)planes2, Iback_projection_patch_hand, hand_patch_size, hist_model_hand, CV_COMP_CORREL, 1);
	//做目标检测器 采样窗口等于目标 测试图像数组 块的反向投影图像 块大小 模板直方图 比较方式（相关） 归一化水平
	cvCalcArrBackProjectPatch((CvArr **)planes4, Iback_projection_patch_cup, cup_patch_size, hist_model_cup, CV_COMP_CORREL, 1);

	cvNamedWindow("Mask_Hand", 1);
	cvNamedWindow("Model_Hand", 1);
	cvNamedWindow("Test_Hand", 1);
	cvNamedWindow("HIST_Model_Hand", 1);
	cvNamedWindow("HIST_Test_Hand", 1);
	cvNamedWindow("BACK_Projection_Hand", 1);

	cvNamedWindow("Mask_Cup", 1);
	cvNamedWindow("Model_Cup", 1);
	cvNamedWindow("Test_Cup", 1);
	cvNamedWindow("HIST_Model_Cup", 1);
	cvNamedWindow("HIST_Test_Cup", 1);
	cvNamedWindow("BACK_Projection_Cup", 1);

	cvShowImage("Mask_Hand", Ihand_mask);
	cvShowImage("Model_Hand", Ihand_model);
	cvShowImage("Test_Hand", Ihand_test);
	cvShowImage("HIST_Model_Hand", Ihist_model_hand);
	cvShowImage("HIST_Test_Hand", Ihist_test_hand);
	cvShowImage("BACK_Projection_Hand", Iback_projection_patch_hand);

	cvShowImage("Mask_Cup", Icup_mask);
	cvShowImage("Model_Cup", Icup_model);
	cvShowImage("Test_Cup", Icup_test);
	cvShowImage("HIST_Model_Cup", Ihist_model_cup);
	cvShowImage("HIST_Test_Cup", Ihist_test_cup);
	cvShowImage("BACK_Projection_Cup", Iback_projection_patch_cup);

	cvWaitKey(0);
	//system("pause");

	cvReleaseHist(&hist_model_hand);
	cvReleaseHist(&hist_test_hand);
	cvReleaseHist(&hist_model_cup);
	cvReleaseHist(&hist_test_cup);

	cvReleaseImage(&Ihand_model);
	cvReleaseImage(&Ihand_test);
	cvReleaseImage(&Ihand_mask);
	cvReleaseImage(&hsv1);
	cvReleaseImage(&hsv2);
	cvReleaseImage(&hsv3);
	cvReleaseImage(&hsv4);
	cvReleaseImage(&Icup_model);
	cvReleaseImage(&Icup_test);
	cvReleaseImage(&Icup_mask);
	cvReleaseImage(&h_plane_1);
	cvReleaseImage(&s_plane_1);
	cvReleaseImage(&v_plane_1);
	cvReleaseImage(&h_plane_2);
	cvReleaseImage(&s_plane_2);
	cvReleaseImage(&v_plane_2);
	cvReleaseImage(&h_plane_3);
	cvReleaseImage(&s_plane_3);
	cvReleaseImage(&v_plane_3);
	cvReleaseImage(&h_plane_4);
	cvReleaseImage(&s_plane_4);
	cvReleaseImage(&v_plane_4);
	cvReleaseImage(&Ihist_model_hand);
	cvReleaseImage(&Ihist_test_hand);
	cvReleaseImage(&Ihist_model_cup);
	cvReleaseImage(&Ihist_test_cup);
	cvReleaseImage(&Iback_projection_patch_hand);
	cvReleaseImage(&Iback_projection_patch_cup);

	cvDestroyAllWindows();
}

