// GeoMatchMy.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <stdio.h> 
#include <iostream>
#include <time.h>
#include "GeoMatch1.h"

using namespace std;

int main()
{
	GeoMatch1 GM;				//����ʵ�ּ���ƥ��
	int lowThreshold = 10;		//deafult value
	int highThreashold = 100;	//deafult value

	double minScore = 0.7;		//deafult value
	double greediness = 0.8;		//0.8 deafult value

	double total_time = 0;
	double score = 0;
	CvPoint result;

	//����ģ��ͼ��
	IplImage* templateImage = cvLoadImage("0.jpg", -1);
	//����ʶ��ͼ��
	IplImage* searchImage = cvLoadImage("2.jpg", -1);
	CvSize templateSize = cvSize(templateImage->width, templateImage->height);
	IplImage* grayTemplateImg = cvCreateImage(templateSize, IPL_DEPTH_8U, 1);

	// Convert color image to gray image.
	if (templateImage->nChannels == 3)
	{
		cvCvtColor(templateImage, grayTemplateImg, CV_RGB2GRAY);
	}
	else
	{
		cvCopy(templateImage, grayTemplateImg);
	}
	cout << "\n ���ڱ�Ե��ģ��ƥ�����\n"<<endl;
	cout << " ------------------------------------\n";

	if (!GM.CreateGeoMatchModel(grayTemplateImg, lowThreshold, highThreashold))
	{
		cout << "ERROR: �޷�����ʶ��ģ��";
		return 0;
	}
	GM.DrawContours(templateImage, CV_RGB(255, 0, 0), 4);
	cout << " ʶ��ģ�ʹ����ɹ�" << "����ֵ = " << lowThreshold << " ����ֵ = " << highThreashold << endl;
	CvSize searchSize = cvSize(searchImage->width, searchImage->height);
	IplImage* graySearchImg = cvCreateImage(searchSize, IPL_DEPTH_8U, 1);

	// Convert color image to gray image. 
	if (searchImage->nChannels == 3)
		cvCvtColor(searchImage, graySearchImg, CV_RGB2GRAY);
	else
	{
		cvCopy(searchImage, graySearchImg);
	}
	cout << " �ҵ�����" << " Minumum���� = " << minScore << " Greediness = " << greediness << "\n\n";
	cout << " ------------------------------------\n";
	clock_t start_time1 = clock();
	score = GM.FindGeoMatchModel(graySearchImg, minScore, greediness, &result);
	clock_t finish_time1 = clock();
	total_time = (double)(finish_time1 - start_time1) / CLOCKS_PER_SEC;
	cout << "ʶ��ʱ�� = " << total_time * 1000 << "ms"<<endl;
	if (score>minScore) // if score is atleast 0.4
	{
		cout << " ʶ�𵽵�ͼ��λ��Ϊ�� [" << result.x << ", " << result.y << "]\n Score = " << score << "\n ʶ��ʱ�� = " << total_time * 1000 << "ms";
		GM.DrawContours(searchImage, result, CV_RGB(0, 255, 0), 4);
	}
	else
		cout << " ����δ�ҵ�";

	cout << "\n ------------------------------------\n\n";
	cout << "\n �밴�����ⰸ��������һ��!";

	//Display result
	cvNamedWindow("Template", CV_WINDOW_AUTOSIZE);
	cvShowImage("Template", templateImage);
	cvNamedWindow("Search Image", CV_WINDOW_AUTOSIZE);
	cvShowImage("Search Image", searchImage);
	// wait for both windows to be closed before releasing images
	cvWaitKey(0);
	cvDestroyWindow("Search Image");
	cvDestroyWindow("Template");
	cvReleaseImage(&searchImage);
	cvReleaseImage(&graySearchImg);
	cvReleaseImage(&templateImage);
	cvReleaseImage(&grayTemplateImg);

    return 0;
}

