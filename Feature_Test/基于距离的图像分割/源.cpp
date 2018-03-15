#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>

using namespace std;
using namespace cv;


void do1(Mat grayImg)
{
	/* 1.����仯 distance transform*/
	Mat distImg;
	distanceTransform(grayImg, distImg, DIST_L1, 3, 5); //����任�������ֵͼ���������ͼ�񣬾������ͣ���Ĥ��С�����ͼ�������5��CV_32F

														/* 5.�Ծ���任������й�һ����[0~1]֮�䣬�ݶ�Ч�� */
	normalize(distImg, distImg, 0, 1, NORM_MINMAX); //��׼����һ����[0~1]
	imshow("05�Ծ���任������й�һ��", distImg);

	/* 6.����仯�Ķ�ֵ�� binary again�����������*/
	threshold(distImg, distImg, 0.4, 1, THRESH_BINARY); //ȫ����ֵ0.4-1
	imshow("06����仯�Ķ�ֵ��", distImg);

	/* 7.��ʴ���õ������߷� */
	Mat k1 = Mat::ones(3, 3, CV_8UC1); //�ṹԪ��ȫ��1,�ṹ�Ĵ�СӰ��ָ�Ч��
	erode(distImg, distImg, k1, Point(-1, -1)); //��ʴ
	imshow("07��ʴ", distImg);

	/* 8.Ѱ������ find contours*/
	Mat dist_8u;
	distImg.convertTo(dist_8u, CV_8U);
	vector<vector<Point>> contours;
	findContours(dist_8u, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0)); //Ѱ��������ʡȥ��νṹ�������ⲿ�����������յ����
																					  /* 9.��������ǲ���� create marker image */
	Mat markers = Mat::zeros(grayImg.size(), CV_32SC1); //�ںڱ����ϻ��������
	for (size_t i = 0; i < contours.size(); i++) {
		drawContours(markers, contours, static_cast<int>(i), Scalar::all(static_cast<int>(i) + 1), -1); //������������䣺���룬�����㣬���������ţ���ɫ��תΪint������ɫ���Ǵ�0��ʼ�ģ�+1�����0��ʼ����thickness����ֵ�����߿���ֵ������䣩
	}
	circle(markers, Point(9, 9), 5, Scalar(255, 255, 255), -1); //���Ͻǻ�Բ��ԭͼ��Բ�ģ��뾶������ɫ��thickness����ֵ����Բ�߿���ֵ�������Բ��
	imshow("08Ѱ������", markers * 1000);

	/* 10.ִ�з�ˮ�룬�ָ���� perform watershed */
	watershed(grayImg, markers);  //��ˮ��任,�ָ����������ԭͼ8λ3ͨ��������߷��������
	Mat mark = Mat::zeros(markers.size(), CV_8UC1);
	markers.convertTo(mark, CV_8UC1);
	bitwise_not(mark, mark, Mat()); //λȡ������ڱ���
	imshow("09ִ�з�ˮ��", mark);  //������Ҷȵȼ���һ��

							  /* 11.��������ɫ��ÿ���ָ�������ʾ���ս�� fill with color and display final result*/
	vector<Vec3b> colors;
	for (size_t i = 0; i < contours.size(); i++) {  //���������ɫ
		int r = theRNG().uniform(0, 255);
		int g = theRNG().uniform(0, 255);
		int b = theRNG().uniform(0, 255);
		colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
	}
	Mat dst = Mat::zeros(markers.size(), CV_8UC3);
	for (int row = 0; row < markers.rows; row++) {
		for (int col = 0; col < markers.cols; col++) {
			int index = markers.at<int>(row, col); //ȡ�÷�ˮ��ָ����Ĳ�ͬ�Ҷȵȼ���ֵ

			if (index > 0 && index <= static_cast<int>(contours.size())) {
				dst.at<Vec3b>(row, col) = colors[index - 1];  //ÿһ����0�Ҷȵȼ���һ��ɫ
			}
			else {
				dst.at<Vec3b>(row, col) = Vec3b(0, 0, 0); //�Ƕ����ź�ɫ
			}
		}
	}
	imshow("10���", dst);
}
void do2(Mat grayImg)
{
	Mat thresholdimg;
	threshold(grayImg, thresholdimg, 200, 255, THRESH_BINARY);
	Mat k = Mat::ones(5, 5, CV_8UC1); //�ṹԪ��ȫ��1,�ṹ�Ĵ�СӰ��ָ�Ч��
	dilate(thresholdimg, thresholdimg, k);
	//imshow("thresholdimg", thresholdimg);
	Mat distImg;
	distanceTransform(thresholdimg, distImg, DIST_L1, 3, 5);
	imshow("distImg", distImg);
	normalize(distImg, distImg, 0, 1, NORM_MINMAX);
	imshow("�Ծ���任������й�һ��", distImg);
	threshold(distImg, distImg, 0.5, 255, THRESH_BINARY);
	Mat k1 = Mat::ones(55, 55, CV_8UC1); //�ṹԪ��ȫ��1,�ṹ�Ĵ�СӰ��ָ�Ч��
										 //erode(distImg, distImg, k1, Point(-1, -1)); //��ʴ
										 //erode(distImg, distImg, k1, Point(-1, -1)); //��ʴ
	imshow("��ʴ", distImg);
}