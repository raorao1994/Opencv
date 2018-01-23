/*
自实现BackgroundSubtractorMOG高斯混合背景/前景分割方法
https://www.cnblogs.com/tornadomeet/archive/2012/06/02/2531705.html
*/
#pragma once
#ifndef CIRCLE_H
#define CIRCLE_H
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;

class GMM
{
public:
	GMM();
	~GMM();
private:
	//定义变量
	static const int GMM_MAX_COMPONT=6;//高斯模型个数
	double GMM_LEARN_ALPHA=0.05;//定义学习率
	double GMM_THRESHOD_SUMW=0.7;//阈值
	double edit_threshold_w = 3.0;//更新权重阈值
	bool pause;//终止
	Mat w[GMM_MAX_COMPONT];//权重
	Mat u[GMM_MAX_COMPONT];//均值
	Mat sigma[GMM_MAX_COMPONT];//方差
	double defaultsigma = 15.0;//默认方差值 opencv 自带的gmm代码中用的是15.0
	Mat fit_num, gmask;//？二值化图像、全景、背景
	vector<Mat> output_m;//输出？
	Mat output_img;//输出图片
	float temp_w, temp_u, temp_sigma;//中间临时变量

private:
	//变量
	int NUM_FRAME=0;//已训练样本数
	//GMM整体初始化函数声明
	void GMM_Init(Mat img);
	//GMM第一帧初始化函数声明
	void GMM_First_Frame(Mat img);
	//GMM训练过程函数声明
	void GMM_Train(Mat img);
	//对输入图像每个像素GMM选择合适的个数函数声明
	void GMM_Fit_Num(Mat img);
	//GMM测试函数的实现
	void GMM_Test(Mat img);
	//连通域去噪函数声明
	void Find_Connected_Components(Mat img);
public:
	int END_FRAME;//训练样本个数
	GMM(int END_FRAME);//初始化训练样本个数
	void Train(Mat img);//训练
	Mat Apply(Mat img);//测试
};
#endif
