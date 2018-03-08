// ���ڷ�ˮ���ͼ��ָ�.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>

using namespace std;
using namespace cv;

int main()
{
	Mat src = imread("2.jpg");
	if (src.empty()) {
		printf("could not load image...\n");
		return -1;
	}
	namedWindow("ԭͼ", CV_WINDOW_AUTOSIZE);
	imshow("01ԭͼ", src);

	/* 1.תΪ�ڱ��� change background */
	for (int row = 0; row < src.rows; row++) {
		for (int col = 0; col < src.cols; col++) {
			if (src.at<Vec3b>(row, col) == Vec3b(255, 255, 255)) {
				src.at<Vec3b>(row, col)[0] = 0;
				src.at<Vec3b>(row, col)[1] = 0;
				src.at<Vec3b>(row, col)[2] = 0;
			}
		}
	}
	imshow("02��ɫ����ͼ��", src);

	/* 2.�� sharpen����߶Աȶȣ�ͻ����Ե */
	Mat kernel = (Mat_<float>(3, 3) << 1, 1, 1, 1, -8, 1, 1, 1, 1); //��
	Mat imgLaplance;
	Mat sharpenImg = src;
	filter2D(src, imgLaplance, CV_32F, kernel, Point(-1, -1), 0, BORDER_DEFAULT); //�о���˵�Ҫ��32λ��ȣ�������������ֵ�п��ܳ���255�����⾫�ȶ�ʧ����ֵģ�����ã�
	src.convertTo(sharpenImg, CV_32F);
	Mat resultImg = sharpenImg - imgLaplance;
	resultImg.convertTo(resultImg, CV_8UC3); //��������ת��
	//imgLaplance.convertTo(imgLaplance, CV_8UC3);
	imshow("03�񻯵Ľ��", resultImg);
	src = resultImg; //���񻯵Ľ��

	/* 3.תΪ�Ҷ��ٶ�ֵ�� convert to binary */
	Mat binaryImg;
	cvtColor(src, resultImg, CV_BGR2GRAY);
	threshold(resultImg, binaryImg, 40, 255, THRESH_BINARY | THRESH_OTSU); //�Զ���ֵ
	imshow("04��ֵ��", binaryImg);

	/* 4.����仯 distance transform*/
	Mat distImg;
	distanceTransform(binaryImg, distImg, DIST_L1, 3, 5); //����任�������ֵͼ���������ͼ�񣬾������ͣ���Ĥ��С�����ͼ�������5��CV_32F

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
	Mat markers = Mat::zeros(src.size(), CV_32SC1); //�ںڱ����ϻ��������
	for (size_t i = 0; i < contours.size(); i++) {
		drawContours(markers, contours, static_cast<int>(i), Scalar::all(static_cast<int>(i) + 1), -1); //������������䣺���룬�����㣬���������ţ���ɫ��תΪint������ɫ���Ǵ�0��ʼ�ģ�+1�����0��ʼ����thickness����ֵ�����߿���ֵ������䣩
	}
	circle(markers, Point(9, 9), 5, Scalar(255, 255, 255), -1); //���Ͻǻ�Բ��ԭͼ��Բ�ģ��뾶������ɫ��thickness����ֵ����Բ�߿���ֵ�������Բ��
	imshow("08Ѱ������", markers * 1000);

	/* 10.ִ�з�ˮ�룬�ָ���� perform watershed */
	watershed(src, markers);  //��ˮ��任,�ָ����������ԭͼ8λ3ͨ��������߷��������
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

	waitKey(0);
	return 0;
}

