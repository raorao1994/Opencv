// Transformation_SURF.cpp : 定义控制台应用程序的入口点。
//OpenCV仿射变换 & SURF特征点描述合辑
#include "stdafx.h"
#include "opencv2/core/core.hpp"  
#include "opencv2/features2d/features2d.hpp"  
#include "opencv2/highgui/highgui.hpp"
//#include<opencv2/legacy/legacy.hpp>  
#include <opencv2/xfeatures2d/nonfree.hpp>
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  
#include <iostream>  
using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;

void Transformation();
void SURF_FUNC();
int main()
{
	//Transformation();
	SURF_FUNC();
	// 等待用户按任意按键退出程序  
	waitKey(0);
    return 0;
}
//仿射变化
static void Transformation()
{
	//【0】改变console字体颜色  
	system("color 1A");

	//【1】参数准备  
	//定义两组点，代表两个三角形  
	Point2f srcTriangle[3];
	Point2f dstTriangle[3];
	//定义一些Mat变量  
	Mat rotMat(2, 3, CV_32FC1);
	Mat warpMat(2, 3, CV_32FC1);
	Mat srcImage, dstImage_warp, dstImage_warp_rotate;

	//【2】加载源图像并作一些初始化  
	srcImage = imread("d:/8.jpg", 1);
	if (!srcImage.data) { printf("读取图片错误，请确定目录下是否有imread函数指定的图片存在~！ \n"); return; }
	// 设置目标图像的大小和类型与源图像一致  
	dstImage_warp = Mat::zeros(srcImage.rows, srcImage.cols, srcImage.type());

	//【3】设置源图像和目标图像上的三组点以计算仿射变换  
	srcTriangle[0] = Point2f(0, 0);
	srcTriangle[1] = Point2f(static_cast<float>(srcImage.cols - 1), 0);
	srcTriangle[2] = Point2f(0, static_cast<float>(srcImage.rows - 1));

	dstTriangle[0] = Point2f(static_cast<float>(srcImage.cols*0.0), static_cast<float>(srcImage.rows*0.33));
	dstTriangle[1] = Point2f(static_cast<float>(srcImage.cols*0.65), static_cast<float>(srcImage.rows*0.35));
	dstTriangle[2] = Point2f(static_cast<float>(srcImage.cols*0.15), static_cast<float>(srcImage.rows*0.6));

	//【4】求得仿射变换  
	warpMat = getAffineTransform(srcTriangle, dstTriangle);

	//【5】对源图像应用刚刚求得的仿射变换  
	warpAffine(srcImage, dstImage_warp, warpMat, dstImage_warp.size());

	//【6】对图像进行缩放后再旋转  
	// 计算绕图像中点顺时针旋转50度缩放因子为0.6的旋转矩阵  
	Point center = Point(dstImage_warp.cols / 2, dstImage_warp.rows / 2);
	double angle = -30.0;
	double scale = 0.8;
	// 通过上面的旋转细节信息求得旋转矩阵  
	rotMat = getRotationMatrix2D(center, angle, scale);
	// 旋转已缩放后的图像  
	warpAffine(dstImage_warp, dstImage_warp_rotate, rotMat, dstImage_warp.size());


	//【7】显示结果  
	imshow("窗口1", srcImage);
	imshow("窗口2", dstImage_warp);
	imshow("窗口3", dstImage_warp_rotate);
}
//SURF识别
static void SURF_FUNC()
{
	//【1】载入素材图  
	Mat srcImage1 = imread("d:/9.jpg", 1);
	Mat srcImage2 = imread("d:/12.jpg", 1);
	if (!srcImage1.data || !srcImage2.data)
	{
		printf("读取图片错误，请确定目录下是否有imread函数指定的图片存在~！ \n"); return;
	}

	//【2】使用SURF算子检测关键点  
	int minHessian = 400;//SURF算法中的hessian阈值
	Ptr<SURF> detector = SURF::create(minHessian);
	//SurfFeatureDetector detector(minHessian);//定义一个SurfFeatureDetector（SURF） 特征检测类对象    
	std::vector<KeyPoint> keyPoint1, keyPoints2;//vector模板类，存放任意类型的动态数组  

												//【3】调用detect函数检测出SURF特征关键点，保存在vector容器中  
	detector->detect(srcImage1, keyPoint1);
	detector->detect(srcImage2, keyPoints2);

	//【4】计算描述符（特征向量）  
	//SurfDescriptorExtractor extractor;
	Ptr<SURF> extractor = SURF::create();
	Mat descriptors1, descriptors2;
	extractor->compute(srcImage1, keyPoint1, descriptors1);
	extractor->compute(srcImage2, keyPoints2, descriptors2);

	//【5】使用BruteForce进行匹配  
	// 实例化一个匹配器  
	//BruteForceMatcher< L2<float> > matcher;
	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce");
	std::vector< DMatch > matches;
	//匹配两幅图中的描述子（descriptors）  
	matcher->match(descriptors1, descriptors2, matches);

	//【6】绘制从两个图像中匹配出的关键点  
	Mat imgMatches;
	drawMatches(srcImage1, keyPoint1, srcImage2, keyPoints2, matches, imgMatches);//进行绘制  
	//【7】显示效果图  
	imshow("匹配图", imgMatches);
}


