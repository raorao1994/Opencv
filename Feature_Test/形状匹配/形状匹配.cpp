// ��״ƥ��.cpp : �������̨Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

using namespace std;
using namespace cv;

int main()
{
	//1.����ģ��ͼ�������  
	Mat templateImg = imread("11.png", CV_LOAD_IMAGE_COLOR);
	Mat copyImg1 = templateImg.clone();
	cvtColor(templateImg, templateImg, CV_BGR2GRAY);
	threshold(templateImg, templateImg, 100, 255, CV_THRESH_BINARY);//ȷ�������Ұ�  
	imshow("temp", templateImg);
	vector<vector<Point>> contours1;
	findContours(templateImg, contours1, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//���������  
	drawContours(copyImg1, contours1, -1, Scalar(100, 255, 100), 4, 8);
	//2.���Ҵ�����ͼ�������  
	Mat testImg = imread("22.png", CV_LOAD_IMAGE_COLOR);
	Mat copyImg2 = testImg.clone();
	cvtColor(testImg, testImg, CV_BGR2GRAY);
	threshold(testImg, testImg, 100, 255, CV_THRESH_BINARY);//ȷ�������Ұ�  
	imshow("thresh2", testImg);
	vector<vector<Point>> contours2;
	findContours(testImg, contours2, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//���������  
																			 //3.��״ƥ��---�Ƚ�������״������������ƶ�  
	for (int i = 0; i < contours2.size(); i++)//����������ͼ�������  
	{
		//���ش�������ģ������֮������ƶ�,a0ԽСԽ����  
		double a0 = matchShapes(contours1[0], contours2[i], CV_CONTOURS_MATCH_I1, 0);
		cout << "ģ�������������ͼ������" << i << "�����ƶ�:" << a0 << endl;//�����������������ƶ�  
		if (a0<0.1)//�����������ģ�����������ƶ�С��0.1  
		{
			drawContours(copyImg2, contours2, i, Scalar(100, 255, 100), 2, 8);//���ڴ�����ͼ���ϻ���������  
		}
		imshow("copyImg2", copyImg2);
		if (waitKey(0) == 27)//�ȴ�����������һ��������ESC���˳�  
		{
			cout << "ESC�˳�" << endl;
			break;
		}
	}
	waitKey(0);
	return 0;
}

