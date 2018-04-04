// HIST.cpp : 定义控制台应用程序的入口点。
// 直方图的计算与显示
#include "stdafx.h"
#include <stdlib.h>  
#include <stdio.h> 
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;
//值为255点当前点 全局变量才可通过普通成员引用变更其值
CvPoint Current_Point;

bool find_point(IplImage *img, char val);

int main()
{	
	//阈值类型
	int threshold_type = CV_THRESH_BINARY;
	//上一个区域面积
	int Last_Area = 0; 
	//当前区域面积
	int Current_Area = 0;
	//阈值
	double threshold = 65;
	//值为255点的上一点
	CvPoint Last_Point;
	//被填充区域统计属性
	CvConnectedComp comp;
	//源图像 HSV格式图像
	IplImage *src1, *hsv, *Igray, *Ithreshold, *Itemp, *Iopen, *Imask;
	//初始化上一点
	Last_Point = cvPoint(0, 0);
	//初始化当前点
	Current_Point = cvPoint(0, 0);
	//加载图像
	if (!(src1 = cvLoadImage("handle.jpg")))
		return -1;

	//此处调入图像掩码应为单通道
	//if (!(Imask = cvLoadImage("D:\\Template\\OpenCV\\Template46_hue-saturation_Hist\\Debug\\cup2.jpg", CV_LOAD_IMAGE_GRAYSCALE)))
	//  return -2;

	hsv = cvCreateImage(cvGetSize(src1), src1->depth, src1->nChannels);
	Igray = cvCreateImage(cvGetSize(src1), src1->depth, 1);
	Ithreshold = cvCreateImage(cvGetSize(src1), src1->depth, 1);
	Itemp = cvCreateImage(cvGetSize(src1), src1->depth, 1);
	Iopen = cvCreateImage(cvGetSize(src1), src1->depth, 1);
	//生成手掌掩码图像用
	Imask = cvCreateImage(cvGetSize(src1), src1->depth, 1); 
	//源图像->HSV格式图像
	cvCvtColor(src1, hsv, CV_BGR2HSV);
	//源图像->灰度图像
	cvCvtColor(src1, Igray, CV_BGR2GRAY);
	//二值阈值化
	cvThreshold(Igray, Ithreshold, threshold, 255, threshold_type);
	//开运算，去除小亮区域，其他联结 NULL:3*3参考点为中心的核
	cvMorphologyEx(Ithreshold, Iopen, Itemp, NULL, CV_MOP_OPEN, 1);

	cvNamedWindow("原图", 1);
	cvNamedWindow("灰度图", 1);
	cvNamedWindow("二值化", 1);
	cvNamedWindow("开运算", 1);
	cvNamedWindow("FLOOD_FILL", 1);

	cvShowImage("原图", src1);
	cvShowImage("灰度图", Igray);
	cvShowImage("二值化", Ithreshold);
	cvShowImage("开运算", Iopen);
	cvShowImage("FLOOD_FILL", Imask);

	//漫水填充 获得手掌掩码
	cvNamedWindow("FLOOD_FILL", 1);
	//复制生成手掌掩码
	cvCopy(Iopen, Imask);

	do
	{
		//找像素值为255的像素点
		if (find_point(Imask, 255))
		{

			cout << " X: " << Current_Point.x << " Y: " << Current_Point.y << endl;
			//对值为255的点进行漫水填充，值100
			cvFloodFill(Imask, Current_Point, cvScalar(100), cvScalar(0), cvScalar(0),
				&comp, 8 | CV_FLOODFILL_FIXED_RANGE);
			//当前区域面积
			Current_Area = comp.area;
			//当前区域大于上一区域，上一区域清0
			if (Last_Area<Current_Area)
			{
				if (Last_Area>0)
					//上一区域赋值0
					cvFloodFill(Imask, Last_Point, cvScalar(0), cvScalar(0), cvScalar(0),
						&comp, 8 | CV_FLOODFILL_FIXED_RANGE);
				cvShowImage("FLOOD_FILL", Imask);
				cvWaitKey(500);
				//当前区域赋值给上一区域
				Last_Area = Current_Area;
				//当前点赋值给上一点
				Last_Point = Current_Point;
				//memcpy(&Last_Point, &Current_Point, sizeof(CvPoint)); //错误，此方法复制无法正常使用掩码
			}
			//当前区域小于等于上一区域，当前区域清0
			else                                            
			{
				if (Current_Area>0)
					//当前区域赋值0
					cvFloodFill(Imask, Current_Point, cvScalar(0), cvScalar(0), cvScalar(0),
						&comp, 8 | CV_FLOODFILL_FIXED_RANGE);
				cvShowImage("FLOOD_FILL", Imask);
				cvWaitKey(500);
			}
		}
		//最后剩余的最大区域赋值255
		else
		{
			cvFloodFill(Imask, Last_Point, cvScalar(255), cvScalar(0), cvScalar(0), &comp, 8 | CV_FLOODFILL_FIXED_RANGE);
			cvShowImage("FLOOD_FILL", Imask);
			cvWaitKey(500);
			//上一区域赋值0
			break;
		}
	} while (true);

	cvSaveImage("Imask.jpg", Imask);

	//色调(hue) 饱和度(saturation) 明度(value)
	IplImage *h_plane = cvCreateImage(cvSize(hsv->width, hsv->height), IPL_DEPTH_8U, 1);
	IplImage *s_plane = cvCreateImage(cvSize(hsv->width, hsv->height), IPL_DEPTH_8U, 1);
	IplImage *v_plane = cvCreateImage(cvSize(hsv->width, hsv->height), IPL_DEPTH_8U, 1);
	//色相饱和度数组
	IplImage *planes[] = { h_plane,s_plane };
	//图像分割
	
	//cvCvtPixToPlane(hsv, h_plane, s_plane, v_plane, NULL);
	cvSplit(hsv, h_plane, s_plane, v_plane, NULL);

	int h_bins = 30, s_bins = 32;

	//建立直方图
	CvHistogram *hist;
	//对应维数包含bins个数的数组
	int hist_size[] = { h_bins, s_bins };
	//H通道划分范围 饱和度0-180
	float h_ranges[] = { 0, 180 };
	//S通道划分范围
	float s_ranges[] = { 0, 255 };
	//划分范围数对, ****均匀bin，range只要最大最小边界
	float* ranges[] = { h_ranges, s_ranges };   
	//创建直方图 （维数，对应维数bins个数，密集矩阵方式存储，划分范围数对，均匀直方图）
	hist = cvCreateHist(2, hist_size, CV_HIST_ARRAY, ranges, 1);
	//计算直方图（图像，直方图结构，不累加，掩码）
	cvCalcHist(planes, hist, 0, Imask);
	//直方图归一化
	cvNormalizeHist(hist, 1.0);         

	//绘制可视化直方图
	int scale = 10;
	IplImage* hist_img = cvCreateImage(cvSize(h_bins*scale, s_bins*scale), 8, 3);   //300*320
	cvZero(hist_img);

	//以小灰度块填充图像
	float max_value = 0;
	//获取直方图最大值
	cvGetMinMaxHistValue(hist, NULL, &max_value, NULL, NULL);

	for (int h = 0; h < h_bins; h++)
	{
		for (int s = 0; s < s_bins; s++)
		{
			//获取直方图相应bin中的浮点数
			//float bin_val = cvQueryHistValue_2D(hist, h, s);
			float bin_val = cvGetReal2D(hist, h, s);
			//映射到255空间，归一后太小，难辨
			int intensity = cvRound(bin_val * 255 / max_value); 
			cvRectangle(hist_img, cvPoint(h*scale, s*scale),
				cvPoint((h + 1)*scale - 1, (s + 1)*scale - 1),
				CV_RGB(intensity, intensity, intensity), CV_FILLED);
		}
	}

	cvNamedWindow("HIST_Image", 1);
	cvShowImage("HIST_Image", hist_img);

	cvWaitKey();

	cvReleaseHist(&hist);

	cvReleaseImage(&src1);
	cvReleaseImage(&hsv);
	cvReleaseImage(&Igray);
	cvReleaseImage(&Ithreshold);
	cvReleaseImage(&Itemp);
	cvReleaseImage(&Iopen);
	cvReleaseImage(&Imask);
	cvReleaseImage(&h_plane);
	cvReleaseImage(&s_plane);
	cvReleaseImage(&v_plane);
	cvReleaseImage(&hist_img);

	cvDestroyWindow("src1");
	cvDestroyWindow("HIST_Image");
	cvDestroyWindow("GRAY_Image");
	cvDestroyWindow("THRESHHOLD_Image");
	cvDestroyWindow("OPEN_Image");
	cvDestroyWindow("FLOOD_FILL");
	cvDestroyWindow("HIST_Image");
}

/******************遍历图像，指针算法********************/
bool find_point(IplImage *img, char val)
{
	char* ptr = NULL;

	if (img->nChannels == 1)
	{
		ptr = img->imageData;
		if (ptr != NULL)
		{
			for (int i = 0; i < img->height; i++)       //矩阵指针行寻址
			{
				ptr = (img->imageData + i*(img->widthStep));   //i 行 j 列
				for (int j = 0; j < img->width; j++)    //矩阵指针列寻址
				{
					if (ptr[j] == val)              //判断某点像素是否为255
					{
						Current_Point.x = j;        /********局部变量此方式 无法实现赋值********/
						Current_Point.y = i;
						return true;
					}
				}
			}
		}
	}
	return false;
}