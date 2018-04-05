// 直方图匹配.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

float throld = 5.0;//直方图识别阀值
int h_bins = 30, s_bins = 32;

//滑动窗口裁剪图像
//图像都为灰度图像
void CutImg(Mat img,Mat Temp) 
{

}
//识别匹配度
//判断图像是否识别成功
//输入模版图像二值图像和待检测图像二维直方图
bool Contrast(Mat sig, Mat hist)
{
	Mat sig2;
	int numrows = h_bins*s_bins;
	sig2 = Mat(numrows, 3, CV_32FC1);
	for (int h = 0; h < h_bins; h++)
	{
		for (int s = 0; s < s_bins; s++)
		{
			float bin_val = hist.at<float>(h, s);
			sig2.at<float>(h*s_bins + s, 0) = bin_val;
			sig2.at<float>(h*s_bins + s, 1) = h;
			sig2.at<float>(h*s_bins + s, 2) = s;
		}
	}
	float emd = EMD(sig, sig2, CV_DIST_L2);
	if (emd < throld)
		return true;
	else
		return false;
}

//获取二维直方图数组
//输入图像为灰度图
void GetSig(InputArray img,OutputArray sig)
{
	//1、获取HSV空间图像
	Mat hsv = img.getMat();
	//源图像->HSV格式图像
	cvtColor(img, hsv, CV_BGR2HSV);
	//2、创建HSV图像空间单通道图层
	//色调(hue) 饱和度(saturation) 明度(value)
	//vector<Mat> channels;
	//Mat h_plane = Mat(hsv.size(), IPL_DEPTH_8U, 1);
	//Mat s_plane = Mat(hsv.size(), IPL_DEPTH_8U, 1);
	//Mat v_plane = Mat(hsv.size(), IPL_DEPTH_8U, 1);
	////3、创建色相饱和度数组,并分割图层
	//Mat planes[] = { h_plane, s_plane };
	//split(hsv, channels);//分离色彩通道  
	//h_plane = channels.at(0);
	//s_plane = channels.at(1);
	//v_plane = channels.at(2);

	//4、创建直方图
	int channels[] = { 0, 1 };
	MatND hist;
	int hist_size[] = { h_bins, s_bins };       //对应维数包含bins个数的数组
	float h_ranges[] = { 0, 180 };              //H通道划分范围 饱和度0-180
	float s_ranges[] = { 0, 255 };              //S通道划分范围
	//划分范围数对, ****均匀bin，range只要最大最小边界
	const float* ranges[] = { h_ranges, s_ranges };  
	//创建直方图 （维数，对应维数bins个数，密集矩阵方式存储，划分范围数对，均匀直方图）
	calcHist(&hsv, 1, channels, Mat(), // do not use mask
		hist, 2, hist_size, ranges,
		true, // 直方图是一致的
		false);
	//5、生成直方图数组
	Mat sig2=sig.getMat();
	int numrows = h_bins*s_bins;
	sig2 = Mat(numrows, 3, CV_32FC1);
	for (int h = 0; h < h_bins; h++)
	{
		for (int s = 0; s < s_bins; s++)
		{
			float bin_val = hist.at<float>(h, s);
			sig2.at<float>(h*s_bins + s, 0) = bin_val;
			sig2.at<float>(h*s_bins + s, 1) = h;
			sig2.at<float>(h*s_bins + s, 2) = s;
		}
	}
}

void show(InputArray src, OutputArray img)
{
	Mat _src=src.
	img.create()
}

int main()
{
	Mat tempSrc = imread("../../HIST/data/2.png",1);
	Mat srcSrc = imread("../../HIST/data/3.png",1);
	Mat sig1, sig2;
	GetSig(tempSrc,sig1);
	GetSig(srcSrc,sig2);

	/*float emd = EMD(sig1, sig2, CV_DIST_L2);
	if (emd < throld)
		return true;
	else
		return false;*/
	Mat ii;
	show(tempSrc,ii);
	imshow("ii",ii);
	system("prase");
}

