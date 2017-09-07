// ImageROI.cpp : 定义控制台应用程序的入口点。
//(源代码)http://blog.csdn.net/poem_qianmo/article/details/20911629
// ROI区域图像叠加&初级图像混合 全剖析

#include "stdafx.h"
#include <opencv2\opencv.hpp>

const char *path1 = "D://8.jpg";
const char *path11 = "D://10.jpg";
const char *path2 = "D://9.jpg";
using namespace cv;
//----------------------------------【ROI_AddImage( )函数】----------------------------------  
// 函数名：ROI_AddImage（）  
//     描述：利用感兴趣区域ROI实现图像叠加  
//----------------------------------------------------------------------------------------------  
bool ROI_AddImage1()
{

	//【1】读入图像  
	Mat srcImage1 = imread(path1);
	Mat logoImage = imread(path2);
	if (!srcImage1.data) { printf("你妹，读取srcImage1错误~！ \n"); return false; }
	if (!logoImage.data) { printf("你妹，读取logoImage错误~！ \n"); return false; }

	//【2】定义一个Mat类型并给其设定ROI区域  
	Mat imageROI = srcImage1(Rect(100, 100, logoImage.cols, logoImage.rows));

	//【3】加载掩模（必须是灰度图）  
	Mat mask = imread(path2, 0);

	//【4】将掩膜拷贝到ROI  
	logoImage.copyTo(imageROI, mask);

	//【5】显示结果  
	namedWindow("<1>利用ROI实现图像叠加示例窗口");
	imshow("<1>利用ROI实现图像叠加示例窗口", srcImage1);

	return true;
}

//---------------------------------【LinearBlending（）函数】-------------------------------------  
// 函数名：LinearBlending（）  
// 描述：利用cv::addWeighted（）函数实现图像线性混合  
//--------------------------------------------------------------------------------------------  
bool LinearBlending1()
{
	//【0】定义一些局部变量  
	double alphaValue = 0.5;
	double betaValue;
	Mat srcImage2, srcImage3, dstImage;

	//【1】读取图像 ( 两幅图片需为同样的类型和尺寸 )  
	srcImage2 = imread(path1);
	srcImage3 = imread(path11);

	if (!srcImage2.data) { printf("你妹，读取srcImage2错误~！ \n"); return false; }
	if (!srcImage3.data) { printf("你妹，读取srcImage3错误~！ \n"); return false; }

	//【2】做图像混合加权操作  
	betaValue = (1.0 - alphaValue);
	addWeighted(srcImage2, alphaValue, srcImage3, betaValue, 0.0, dstImage);

	//【3】创建并显示原图窗口  
	namedWindow("<2>线性混合示例窗口【原图】 by浅墨", 1);
	imshow("<2>线性混合示例窗口【原图】 by浅墨", srcImage2);

	namedWindow("<3>线性混合示例窗口【效果图】 by浅墨", 1);
	imshow("<3>线性混合示例窗口【效果图】 by浅墨", dstImage);

	return true;

}

bool ROI_LinearBlending1()
{

	//【1】读取图像
	Mat srcImage4 = imread(path1, 1);
	Mat logoImage = imread(path2);

	if (!srcImage4.data) { printf("你妹，读取srcImage4错误~！ \n"); return false; }
	if (!logoImage.data) { printf("你妹，读取logoImage错误~！ \n"); return false; }

	//【2】定义一个Mat类型并给其设定ROI区域
	Mat imageROI;
	//方法一
	imageROI = srcImage4(Rect(100, 10, logoImage.cols, logoImage.rows));
	//方法二
	//imageROI=srcImage4(Range(250,250+logoImage.rows),Range(200,200+logoImage.cols));

	//【3】将logo加到原图上
	addWeighted(imageROI, 0.5, logoImage, 0.3, 0., imageROI);

	//【4】显示结果
	namedWindow("<4>区域线性图像混合示例窗口 by浅墨");
	imshow("<4>区域线性图像混合示例窗口 by浅墨", srcImage4);

	return true;
}
int main()
{
	//ROI_AddImage();
	//LinearBlending1();
	ROI_LinearBlending1();
	int i=waitKey(0);
    return 0;
}

