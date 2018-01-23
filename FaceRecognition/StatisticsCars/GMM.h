/*
��ʵ��BackgroundSubtractorMOG��˹��ϱ���/ǰ���ָ��
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
	//�������
	static const int GMM_MAX_COMPONT=6;//��˹ģ�͸���
	double GMM_LEARN_ALPHA=0.05;//����ѧϰ��
	double GMM_THRESHOD_SUMW=0.7;//��ֵ
	double edit_threshold_w = 3.0;//����Ȩ����ֵ
	bool pause;//��ֹ
	Mat w[GMM_MAX_COMPONT];//Ȩ��
	Mat u[GMM_MAX_COMPONT];//��ֵ
	Mat sigma[GMM_MAX_COMPONT];//����
	double defaultsigma = 15.0;//Ĭ�Ϸ���ֵ opencv �Դ���gmm�������õ���15.0
	Mat fit_num, gmask;//����ֵ��ͼ��ȫ��������
	vector<Mat> output_m;//�����
	Mat output_img;//���ͼƬ
	float temp_w, temp_u, temp_sigma;//�м���ʱ����

private:
	//����
	int NUM_FRAME=0;//��ѵ��������
	//GMM�����ʼ����������
	void GMM_Init(Mat img);
	//GMM��һ֡��ʼ����������
	void GMM_First_Frame(Mat img);
	//GMMѵ�����̺�������
	void GMM_Train(Mat img);
	//������ͼ��ÿ������GMMѡ����ʵĸ�����������
	void GMM_Fit_Num(Mat img);
	//GMM���Ժ�����ʵ��
	void GMM_Test(Mat img);
	//��ͨ��ȥ�뺯������
	void Find_Connected_Components(Mat img);
public:
	int END_FRAME;//ѵ����������
	GMM(int END_FRAME);//��ʼ��ѵ����������
	void Train(Mat img);//ѵ��
	Mat Apply(Mat img);//����
};
#endif
