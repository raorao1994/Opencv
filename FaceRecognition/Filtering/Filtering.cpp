// Filtering.cpp : 定义控制台应用程序的入口点。
//http://blog.csdn.net/poem_qianmo/article/details/22745559
//线性滤波专场：方框滤波、均值滤波与高斯滤波

//-----------------------------------【头文件包含部分】---------------------------------------
//     描述：包含程序所依赖的头文件
//----------------------------------------------------------------------------------------------
#include "stdafx.h"
#include <opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

//-----------------------------------【命名空间声明部分】---------------------------------------
//     描述：包含程序所使用的命名空间
//----------------------------------------------------------------------------------------------- 
using namespace std;
using namespace cv;

//-----------------------------------【全局变量声明部分】--------------------------------------
//     描述：全局变量声明
//-----------------------------------------------------------------------------------------------
Mat g_srcImage, g_dstImage1, g_dstImage2, g_dstImage3, g_dstImage4, g_dstImage5;//存储图片的Mat类型
int g_nBoxFilterValue = 3;  //方框滤波参数值
int g_nMeanBlurValue = 3;  //均值滤波参数值
int g_nGaussianBlurValue = 3;  //高斯滤波参数值
int g_nMedianBlurValue = 10;  //中值滤波参数值  
int g_nBilateralFilterValue = 10;  //双边滤波参数值  


//-----------------------------------【全局函数声明部分】--------------------------------------
//     描述：全局函数声明
//-----------------------------------------------------------------------------------------------
//轨迹条的回调函数
static void on_BoxFilter(int, void *);            //方框滤波
static void on_MeanBlur(int, void *);           //均值滤波
static void on_GaussianBlur(int, void *);       //高斯滤波
static void on_MedianBlur(int, void *);               //中值滤波器  
static void on_BilateralFilter(int, void*);                    //双边滤波器  
//-----------------------------------【main( )函数】--------------------------------------------
//     描述：控制台应用程序的入口函数，我们的程序从这里开始
//-----------------------------------------------------------------------------------------------
int main()
{
	//改变console字体颜色
	system("color5E");

	//载入原图
	g_srcImage = imread("d:\\8.jpg", 1);
	if (!g_srcImage.data) { printf("读取srcImage错误~！\n"); return false; }

	//克隆原图到三个Mat类型中
	g_dstImage1 = g_srcImage.clone();
	g_dstImage2 = g_srcImage.clone();
	g_dstImage3 = g_srcImage.clone();

	//显示原图
	namedWindow("【<0>原图窗口】", 1);
	imshow("【<0>原图窗口】", g_srcImage);


	//=================【<1>方框滤波】==================
	//创建窗口
	namedWindow("【<1>方框滤波】", 1);
	//创建轨迹条
	createTrackbar("内核值：", "【<1>方框滤波】", &g_nBoxFilterValue, 40, on_BoxFilter);
	on_MeanBlur(g_nBoxFilterValue, 0);
	imshow("【<1>方框滤波】", g_dstImage1);
	//================================================

	//=================【<2>均值滤波】==================
	//创建窗口
	namedWindow("【<2>均值滤波】", 1);
	//创建轨迹条
	createTrackbar("内核值：", "【<2>均值滤波】", &g_nMeanBlurValue, 40, on_MeanBlur);
	on_MeanBlur(g_nMeanBlurValue, 0);
	//================================================

	//=================【<3>高斯滤波】=====================
	//创建窗口
	namedWindow("【<3>高斯滤波】", 1);
	//创建轨迹条
	createTrackbar("内核值：", "【<3>高斯滤波】", &g_nGaussianBlurValue, 40, on_GaussianBlur);
	on_GaussianBlur(g_nGaussianBlurValue, 0);
	//================================================

	//=================【<4>中值滤波】===========================  
	//创建窗口  
	namedWindow("【<4>中值滤波】", 1);
	//创建轨迹条  
	createTrackbar("参数值：", "【<4>中值滤波】", &g_nMedianBlurValue, 50, on_MedianBlur);
	on_MedianBlur(g_nMedianBlurValue, 0);
	//=======================================================  


	//=================【<5>双边滤波】===========================  
	//创建窗口  
	namedWindow("【<5>双边滤波】", 1);
	//创建轨迹条  
	createTrackbar("参数值：", "【<5>双边滤波】", &g_nBilateralFilterValue, 50, on_BilateralFilter);
	on_BilateralFilter(g_nBilateralFilterValue, 0);
	//=======================================================  

	//输出一些帮助信息
	cout << endl << "\t请调整滚动条观察图像效果~\n\n"
		<< "\t按下“q”键时，程序退出~!\n";

	//按下“q”键时，程序退出
	while (char(waitKey(1)) != 'q') {}

	return 0;
}
												
//-----------------------------【on_BoxFilter( )函数】------------------------------------
//     描述：方框滤波操作的回调函数
//-----------------------------------------------------------------------------------------------
static void on_BoxFilter(int, void *)
{
	//方框滤波操作
	boxFilter(g_srcImage, g_dstImage1, -1, Size(g_nBoxFilterValue + 1, g_nBoxFilterValue + 1));
	//显示窗口
	imshow("【<1>方框滤波】", g_dstImage1);
}


//-----------------------------【on_MeanBlur( )函数】------------------------------------
//     描述：均值滤波操作的回调函数
//-----------------------------------------------------------------------------------------------
static void on_MeanBlur(int, void *)
{
	//均值滤波操作
	blur(g_srcImage, g_dstImage2, Size(g_nMeanBlurValue + 1, g_nMeanBlurValue + 1), Point(-1, -1));
	//显示窗口
	imshow("【<2>均值滤波】", g_dstImage2);
}


//-----------------------------【on_GaussianBlur( )函数】------------------------------------
//     描述：高斯滤波操作的回调函数
//-----------------------------------------------------------------------------------------------
static void on_GaussianBlur(int, void *)
{
	//高斯滤波操作
	GaussianBlur(g_srcImage, g_dstImage3, Size(g_nGaussianBlurValue * 2 + 1, g_nGaussianBlurValue * 2 + 1), 0, 0);
	//显示窗口
	imshow("【<3>高斯滤波】", g_dstImage3);
}
//-----------------------------【on_MedianBlur( )函数】------------------------------------  
//            描述：中值滤波操作的回调函数  
//-----------------------------------------------------------------------------------------------  
static void on_MedianBlur(int, void *)
{
	medianBlur(g_srcImage, g_dstImage4, g_nMedianBlurValue * 2 + 1);
	imshow("【<4>中值滤波】", g_dstImage4);
}


//-----------------------------【on_BilateralFilter( )函数】------------------------------------  
//            描述：双边滤波操作的回调函数  
//-----------------------------------------------------------------------------------------------  
static void on_BilateralFilter(int, void *)
{
	bilateralFilter(g_srcImage, g_dstImage5, g_nBilateralFilterValue, g_nBilateralFilterValue * 2, g_nBilateralFilterValue / 2);
	imshow("【<5>双边滤波】", g_dstImage5);
}
