// FindRotation.cpp : 定义控制台应用程序的入口点。
// OpenCV轮廓检测，计算物体旋转角度


#include "stdafx.h"
#include <iostream>  
#include <vector>  
#include <opencv2/opencv.hpp>   
#include <opencv2/core/core.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#define PI 3.1415926  
using namespace std;
using namespace cv;

int hough_line(Mat src)
{
	//【1】载入原始图和Mat变量定义     
	Mat srcImage = src;//imread("1.jpg");  //工程目录下应该有一张名为1.jpg的素材图  
	Mat midImage, dstImage;//临时变量和目标图的定义  
	//【2】进行边缘检测和转化为灰度图  
	Canny(srcImage, midImage, 50, 200, 3);//进行一此canny边缘检测  
	cvtColor(midImage, dstImage, CV_GRAY2BGR);//转化边缘检测后的图为灰度图  
	//【3】进行霍夫线变换  
	vector<Vec4i> lines;//定义一个矢量结构lines用于存放得到的线段矢量集合  
	HoughLinesP(midImage, lines, 1, CV_PI / 180, 80, 50, 10);
	//【4】依次在图中绘制出每条线段  
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(dstImage, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(186, 88, 255), 1, CV_AA);
	}
	//【5】显示原始图    
	imshow("【原始图】", srcImage);
	//【6】边缘检测后的图   
	imshow("【边缘检测后的图】", midImage);
	//【7】显示效果图    
	imshow("【效果图】", dstImage);
	//waitKey(0);
	return 0;
}

int main()
{
	// Read input binary image
	char *image_name = "test.png";//test.png
	Mat image = imread(image_name, 0);
	if (!image.data)
		return 0;
	imshow("原图1", image);
	// 从文件中加载原图
	IplImage *pSrcImage = cvLoadImage(image_name, CV_LOAD_IMAGE_UNCHANGED);
	// 转为2值图
	cvThreshold(pSrcImage, pSrcImage, 200, 255, THRESH_BINARY_INV);
	image = cvarrToMat(pSrcImage);
	imwrite("binary.jpg", image);
	cvtColor(image, image, CV_BGR2GRAY);
	// Get the contours(轮廓) of the connected components（构成要素、部件）
	vector<vector<Point>> contours;
	findContours(image,
		contours, // a vector of contours 
		CV_RETR_EXTERNAL, // retrieve the external contours
		CV_CHAIN_APPROX_NONE); // retrieve all pixels of each contours
	// Print contours' length
	cout << "Contours: " << contours.size() << endl;
	vector<vector<Point>>::const_iterator itContours = contours.begin();
	for (; itContours != contours.end(); ++itContours)
	{
		cout << "Size: " << itContours->size() << endl;
	}
	// draw black contours on white image
	Mat result(image.size(), CV_8U, Scalar(255));
	drawContours(result, contours,
		-1, // draw all contours
		Scalar(0), // in black
		2); // with a thickness of 2
	imshow("轮廓2", result);
	// Eliminate too short or too long contours
	int cmin = 100;  // minimum contour length
	int cmax = 1000; // maximum contour length
	vector<vector<Point>>::const_iterator itc = contours.begin();
	while (itc != contours.end()) {
		if (itc->size() < cmin || itc->size() > cmax)
			//擦除
			itc = contours.erase(itc);
		else
			++itc;
	}
	// draw contours on the original(原始) image
	Mat original = imread(image_name);
	drawContours(original, contours,
		-1, // draw all contours
		Scalar(255, 255, 0), // in white
		2); // with a thickness of 2
	imshow("原始轮廓3", original);
	// Let's now draw black contours on white image
	result.setTo(Scalar(255));
	drawContours(result, contours,
		-1, // draw all contours
		Scalar(0), // in black
		1); // with a thickness of 1
	image = imread("binary.jpg", 0);
	// testing the bounding box
	//霍夫变换进行直线检测，此处使用的是probabilistic Hough transform（HoughLinesP）而不是standard Hough transform（HoughLines）  
	Mat result_line(image.size(), CV_8U, Scalar(255));
	result_line = result.clone();
	//hough_line(result_line);
	vector<Vec4i> lines;
	HoughLinesP(result_line, lines, 1, CV_PI / 180, 80, 50, 10);
	for (int i = 0; i < lines.size(); i++)
	{
		line(result_line, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(0, 0, 0), 2, 8, 0);
	}
	imshow("画线4", result_line);
	//获取边缘的最小矩形框
	vector<vector<Point>>::const_iterator itc_rec= contours.begin();
	while (itc_rec!=contours.end())
	{
	  Rect r0= boundingRect(Mat(*(itc_rec)));
	  rectangle(result,r0,Scalar(0),2);
	      ++itc_rec;
	}
	imshow("一些图形描述5",result);
	CvBox2D	End_Rage2D;
	CvPoint2D32f rectpoint[4];
	//开辟内存空间
	CvMemStorage *storage = cvCreateMemStorage(0);
	//CvSeq类型 存放检测到的图像轮廓边缘所有的像素值，坐标值特征的结构体以链表形式
	CvSeq* contour;
	//简短地检索黑色(零)背景中白色(非零)连接组件的外部和(可选)内部边界。
	//这函数可选参数还有不少
	//如果contour不为空，表示找到一个以上轮廓，这样写法只显示一个轮廓
	//如改为for(; contour; contour = contour->h_next) 就可以同时显示多个轮廓
	IplImage* img_i = cvLoadImage(image_name, CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* img_temp = cvCreateImage(cvGetSize(img_i), 8, 1);
	cvThreshold(img_i, img_i, 128, 255, CV_THRESH_BINARY);
	cvCopy(img_i, img_temp);
	cvFindContours(img_temp, storage, &contour, sizeof(CvContour),
		CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
	for (; contour; contour = contour->h_next)
	{
		End_Rage2D = cvMinAreaRect2(contour);
		//代入cvMinAreaRect2这个函数得到最小包围矩形  这里已得出被测物体的角度，宽度,高度，和中点坐标点存放在CvBox2D类型的结构体中，  
		//主要工作基本结束。
		for (int i = 0; i < 4; i++)
		{  
			line(result, cvPointFrom32f(rectpoint[i]), 
				cvPointFrom32f(rectpoint[(i + 1) % 4]), Scalar(125), 2);
		}
		cvBoxPoints(End_Rage2D, rectpoint);
		cout << " angle:\n" << (float)End_Rage2D.angle << endl;      //被测物体旋转角度
	}
	imshow("结果6", result);
	waitKey();
	return 0;
}

