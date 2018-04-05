// ֱ��ͼƥ��.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

float throld = 5.0;//ֱ��ͼʶ��ֵ
int h_bins = 30, s_bins = 32;

//�������ڲü�ͼ��
//ͼ��Ϊ�Ҷ�ͼ��
void CutImg(Mat img,Mat Temp) 
{

}
//ʶ��ƥ���
//�ж�ͼ���Ƿ�ʶ��ɹ�
//����ģ��ͼ���ֵͼ��ʹ����ͼ���άֱ��ͼ
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

//��ȡ��άֱ��ͼ����
//����ͼ��Ϊ�Ҷ�ͼ
void GetSig(InputArray img,OutputArray sig)
{
	//1����ȡHSV�ռ�ͼ��
	Mat hsv;
	//Դͼ��->HSV��ʽͼ��
	cvtColor(img, hsv, CV_BGR2HSV);
	//2������HSVͼ��ռ䵥ͨ��ͼ��
	//ɫ��(hue) ���Ͷ�(saturation) ����(value)
	//vector<Mat> channels;
	//Mat h_plane = Mat(hsv.size(), IPL_DEPTH_8U, 1);
	//Mat s_plane = Mat(hsv.size(), IPL_DEPTH_8U, 1);
	//Mat v_plane = Mat(hsv.size(), IPL_DEPTH_8U, 1);
	////3������ɫ�౥�Ͷ�����,���ָ�ͼ��
	//Mat planes[] = { h_plane, s_plane };
	//split(hsv, channels);//����ɫ��ͨ��  
	//h_plane = channels.at(0);
	//s_plane = channels.at(1);
	//v_plane = channels.at(2);

	//4������ֱ��ͼ
	int channels[] = { 0, 1 };
	MatND hist;
	int hist_size[] = { h_bins, s_bins };       //��Ӧά������bins����������
	float h_ranges[] = { 0, 180 };              //Hͨ�����ַ�Χ ���Ͷ�0-180
	float s_ranges[] = { 0, 255 };              //Sͨ�����ַ�Χ
	//���ַ�Χ����, ****����bin��rangeֻҪ�����С�߽�
	const float* ranges[] = { h_ranges, s_ranges };  
	//����ֱ��ͼ ��ά������Ӧά��bins�������ܼ�����ʽ�洢�����ַ�Χ���ԣ�����ֱ��ͼ��
	calcHist(&hsv, 1, channels, Mat(), // do not use mask
		hist, 2, hist_size, ranges,
		true, // ֱ��ͼ��һ�µ�
		false);
	//5������ֱ��ͼ����
	int numrows = h_bins*s_bins;
	Mat sig2 = Mat(numrows, 3, CV_32FC1);
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
	sig.create(sig2.size(), sig2.type());
	sig2.copyTo(sig);
	sig2.release();
}

void show(InputArray src, OutputArray img)
{
	Mat _src = src.getMat();
	img.create(_src.size(), _src.type());
	_src.copyTo(img);
	_src.release();
}

int main()
{
	Mat tempSrc = imread("../../HIST/data/2.png",1);
	Mat srcSrc = imread("../../HIST/data/3.png",1);
	Mat sig1, sig2;
	GetSig(tempSrc,sig1);
	GetSig(srcSrc,sig2);

	float emd = EMD(sig1, sig2, CV_DIST_L2);
	if (emd < throld)
		return true;
	else
		return false;
	waitKey(0);
	system("pause");
}

