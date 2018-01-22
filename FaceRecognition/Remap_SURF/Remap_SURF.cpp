// Remap_SURF.cpp : 定义控制台应用程序的入口点。
//特征点监测
#include "stdafx.h"

#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv2/core/core.hpp"  
#include "opencv2/features2d/features2d.hpp" 
#include "opencv2/xfeatures2d/nonfree.hpp"
//#include "opencv2/nonfree/nonfree.hpp"  
#include <iostream>


using namespace cv;
using namespace cv::xfeatures2d;
void shine_upon();
void SURF_FUNC();

int main()
{
	//shine_upon();
	SURF_FUNC();
	waitKey(0);
    return 0;
}
//映射
void shine_upon()
{
	//变量定义
	Mat srcImage, dstImage;
	Mat map_x, map_y;
	//载入原始图
	srcImage = imread("d:/8.jpg", 1);
	if (!srcImage.data) {
		printf("read image erro");
	}
	imshow("True Image", srcImage);
	dstImage.create(srcImage.size(), srcImage.type());
	map_x.create(srcImage.size(), CV_32SC1);
	map_y.create(srcImage.size(), CV_32SC1);
	//进行重映射操作
	//双层循环，遍历每一个像素点，改变map_x & map_y的值  
	char key = '3';
	for (int j = 0; j < srcImage.rows; j++)
	{
		for (int i = 0; i < srcImage.cols; i++)
		{
			switch (key)
			{
			case '1': // 键盘【1】键按下，进行第一种重映射操作  
				if (i > srcImage.cols*0.25 && i < srcImage.cols*0.75 && j > srcImage.rows*0.25 && j < srcImage.rows*0.75)
				{
					map_x.at<float>(j, i) = static_cast<float>(2 * (i - srcImage.cols*0.25) + 0.5);
					map_y.at<float>(j, i) = static_cast<float>(2 * (j - srcImage.rows*0.25) + 0.5);
				}
				else
				{
					map_x.at<float>(j, i) = 0;
					map_y.at<float>(j, i) = 0;
				}
				break;
			case '2':// 键盘【2】键按下，进行第二种重映射操作  
				map_x.at<float>(j, i) = static_cast<float>(i);
				map_y.at<float>(j, i) = static_cast<float>(srcImage.rows - j);
				break;
			case '3':// 键盘【3】键按下，进行第三种重映射操作  
				map_x.at<float>(j, i) = static_cast<float>(srcImage.cols - i);
				map_y.at<float>(j, i) = static_cast<float>(j);
				break;
			case '4':// 键盘【4】键按下，进行第四种重映射操作  
				map_x.at<float>(j, i) = static_cast<float>(srcImage.cols - i);
				map_y.at<float>(j, i) = static_cast<float>(srcImage.rows - j);
				break;
			}
		}
	}
	//进行重映射操作  
	remap(srcImage, dstImage, map_x, map_y, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));
	imshow("remap image", dstImage);
}	 
//特征点
void SURF_FUNC()
{
	//【1】载入源图片并显示  
	Mat srcImage1 = imread("d:/8.jpg", 1);
	Mat srcImage2 = imread("d:/11.jpg", 1);
	if (!srcImage1.data || !srcImage2.data)//检测是否读取成功  
	{
		printf("读取图片错误，请确定目录下是否有imread函数指定名称的图片存在~！ \n"); return ;
	}
	imshow("原始图1", srcImage1);
	imshow("原始图2", srcImage2);

	//【2】定义需要用到的变量和类  
	int minHessian = 400;//定义SURF中的hessian阈值特征点检测算子

	std::vector<KeyPoint> keypoints_1, keypoints_2;
	//Opencv3.0以上 #include "opencv2/xfeatures2d/nonfree.hpp"
	Ptr<SURF> detector = SURF::create(minHessian);
	detector->detect(srcImage1, keypoints_1);
	detector->detect(srcImage1, keypoints_2);
	//Opencv3.0以下 "opencv2/nonfree/nonfree.hpp"  
	//SurfFeatureDetector detector(minHessian);//定义一个SurfFeatureDetector（SURF） 特征检测类对象  
	//std::vector<KeyPoint> keypoints_1, keypoints_2;//vector模板类是能够存放任意类型的动态数组，能够增加和压缩数据  

	//【3】调用detect函数检测出SURF特征关键点，保存在vector容器中  
	//detector.detect(srcImage1, keypoints_1);
	//detector.detect(srcImage2, keypoints_2);

	//【4】绘制特征关键点  
	Mat img_keypoints_1; Mat img_keypoints_2;
	drawKeypoints(srcImage1, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	drawKeypoints(srcImage2, keypoints_2, img_keypoints_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

	//【5】显示效果图  
	imshow("特征点检测效果图1", img_keypoints_1);
	imshow("特征点检测效果图2", img_keypoints_2);
}

