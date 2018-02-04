// StatisticsCars.cpp : �������̨Ӧ�ó������ڵ㡣
//ʵʱͳ����Ƶ�еĳ�����
#include "stdafx.h"
#include "stdafx.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <iostream>
#include "GMM.h"

using namespace cv;
using namespace std;
string getstring(const int n)
{

	std::stringstream newstr;
	newstr << n;
	return newstr.str();

}
int main()
{
	bool stop = false;
	//get video file fullpath
	char* videoFilename = "F:\\SVN\\OpenCV\\trunk\\Data\\video1.avi";
	VideoCapture cap(videoFilename);//��Ĭ�ϵ�����ͷ
	if (!cap.isOpened())
	{
		return -1;
	}
	//����ͼ�����
	Mat img;
	GMM* model = new GMM(100);
	//
	int i = 0;
	int cars = 0;
	while (!stop)
	{
		//cut out one image 
		cap >> img;
		imshow("ԭͼ", img);
		Mat testimg;
		img.copyTo(testimg);
		cvtColor(testimg, testimg, CV_BGR2GRAY);
		//if image number is less than 200
		if (i < model->END_FRAME)
		{
			model->Train(testimg);
		}
		//if image number is greater than 200
		else
		{
			Mat binaryzationImg= model->Apply(testimg);
			medianBlur(binaryzationImg, binaryzationImg, 3);
			dilate(binaryzationImg, binaryzationImg, Mat(), Point(-1, -1), 3);
			erode(binaryzationImg, binaryzationImg, Mat(), Point(-1, -1), 6);
			dilate(binaryzationImg, binaryzationImg, Mat(), Point(-1, -1), 3);
			imshow("��ֵ��ͼ��", binaryzationImg);

			//��⳵��
			Mat findc;
			binaryzationImg.copyTo(findc);
			vector<vector<Point>> contours;
			//Ѱ������
			cv::findContours(findc, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
			const int maxArea = 100;
			size_t s = contours.size();
			RNG rng;
			for (size_t i = 0; i < s; i++)
			{
				double area = abs(contourArea(contours[i]));
				if (area > maxArea)
				{
					Scalar sca_color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
					Rect mr = boundingRect(Mat(contours[i]));
					rectangle(img, mr, sca_color, 2, 8, 0);
					//���ԶԶ�̬Ŀ�������Ӧ����  
				}
			}
			//char text[50];
			//sprintf_s(text, "count��", getstring(s));
			//putText(img, text, Point(50, 50), 3, 1, Scalar(0, 255, 255), 2, 8, false);
			cars += s;
			cout <<"������"<<cars<< "ʵʱ����" << s << endl;
			imshow("���ԭͼ", img);
		}
		cout << "��" << i << "��ͼƬ" << endl;
		if (waitKey(10) == 27) //Esc���˳�      
		{
			stop = true;
		}
		i++;
	}
    return 0;
}

