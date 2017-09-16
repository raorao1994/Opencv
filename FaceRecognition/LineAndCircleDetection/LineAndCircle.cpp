//-----------------------------------【头文件包含部分】---------------------------------------  
//      描述：包含程序所依赖的头文件  
//----------------------------------------------------------------------------------------------   
#include "stdafx.h"
#include <opencv2/opencv.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  

//-----------------------------------【命名空间声明部分】--------------------------------------  
//      描述：包含程序所使用的命名空间  
//-----------------------------------------------------------------------------------------------   
using namespace std;
using namespace cv;
//-----------------------------------【全局变量声明部分】--------------------------------------  
//      描述：全局变量声明  
//-----------------------------------------------------------------------------------------------  
Mat g_srcImage, g_dstImage, g_midImage;//原始图、中间图和效果图  
vector<Vec4i> g_lines;//定义一个矢量结构g_lines用于存放得到的线段矢量集合  
					  //变量接收的TrackBar位置参数  
int g_nthreshold = 100;

//-----------------------------------【全局函数声明部分】--------------------------------------  
//      描述：全局函数声明  
//-----------------------------------------------------------------------------------------------  
static void on_HoughLines(int, void*);//回调函数  
//-----------------------------------【main( )函数】--------------------------------------------  
//      描述：控制台应用程序的入口函数，我们的程序从这里开始  
//-----------------------------------------------------------------------------------------------  
//-----------------------------------【main( )函数】--------------------------------------------  
//      描述：控制台应用程序的入口函数，我们的程序从这里开始  
//-----------------------------------------------------------------------------------------------  
int line()
{
	//改变console字体颜色  
	system("color 3F");

	//载入原始图和Mat变量定义     
	Mat g_srcImage = imread("d:/11.jpg");  //工程目录下应该有一张名为1.jpg的素材图  

									   //显示原始图    
	imshow("【原始图】", g_srcImage);

	//创建滚动条  
	namedWindow("【效果图】", 1);
	createTrackbar("值", "【效果图】", &g_nthreshold, 200, on_HoughLines);

	//进行边缘检测和转化为灰度图  
	Canny(g_srcImage, g_midImage, 50, 200, 3);//进行一次canny边缘检测  
	cvtColor(g_midImage, g_dstImage, CV_GRAY2BGR);//转化边缘检测后的图为灰度图  

	//调用一次回调函数，调用一次HoughLinesP函数  
	on_HoughLines(g_nthreshold, 0);
	HoughLinesP(g_midImage, g_lines, 1, CV_PI / 180, 80, 50, 10);
	//显示效果图    
	imshow("【效果图】", g_dstImage);
	waitKey(0);
	return 0;

}


//-----------------------------------【on_HoughLines( )函数】--------------------------------  
//      描述：【顶帽运算/黑帽运算】窗口的回调函数  
//----------------------------------------------------------------------------------------------  
static void on_HoughLines(int, void*)
{
	//定义局部变量储存全局变量  
	Mat dstImage = g_dstImage.clone();
	Mat midImage = g_midImage.clone();

	//调用HoughLinesP函数  
	vector<Vec4i> mylines;
	HoughLinesP(midImage, mylines, 1, CV_PI / 180, g_nthreshold + 1, 50, 10);

	//循环遍历绘制每一条线段  
	for (size_t i = 0; i < mylines.size(); i++)
	{
		Vec4i l = mylines[i];
		line(dstImage, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(23, 180, 55), 1, CV_AA);
	}
	//显示图像  
	imshow("【效果图】", dstImage);
}

int circle()
{
	//【1】载入原始图和Mat变量定义     
	Mat srcImage = imread("d:/11.jpg");  //工程目录下应该有一张名为1.jpg的素材图  
	Mat midImage, dstImage;//临时变量和目标图的定义  

						   //【2】显示原始图  
	imshow("【原始图】", srcImage);

	//【3】转为灰度图，进行图像平滑  
	cvtColor(srcImage, midImage, CV_BGR2GRAY);//转化边缘检测后的图为灰度图  
	GaussianBlur(midImage, midImage, Size(9, 9), 2, 2);

	//【4】进行霍夫圆变换  
	vector<Vec3f> circles;
	HoughCircles(midImage, circles, CV_HOUGH_GRADIENT, 1.5, 10, 200, 100, 0, 0);

	//【5】依次在图中绘制出圆  
	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		//绘制圆心  
		circle(srcImage, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		//绘制圆轮廓  
		circle(srcImage, center, radius, Scalar(155, 50, 255), 3, 8, 0);
	}

	//【6】显示效果图    
	imshow("【效果图】", srcImage);

	waitKey(0);

	return 0;
}