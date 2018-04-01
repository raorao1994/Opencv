// GeoMatchMy.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h> 
#include <iostream>
#include <time.h>
#include "GeoMatch1.h"

using namespace std;

int main()
{
	GeoMatch1 GM;				//对象实现几何匹配
	int lowThreshold = 10;		//deafult value
	int highThreashold = 100;	//deafult value

	double minScore = 0.7;		//deafult value
	double greediness = 0.8;		//0.8 deafult value

	double total_time = 0;
	double score = 0;
	CvPoint result;

	//加载模版图像
	IplImage* templateImage = cvLoadImage("0.jpg", -1);
	//加载识别图像
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
	cout << "\n 基于边缘的模板匹配程序\n"<<endl;
	cout << " ------------------------------------\n";

	if (!GM.CreateGeoMatchModel(grayTemplateImg, lowThreshold, highThreashold))
	{
		cout << "ERROR: 无法创建识别模型";
		return 0;
	}
	GM.DrawContours(templateImage, CV_RGB(255, 0, 0), 4);
	cout << " 识别模型创建成功" << "低阈值 = " << lowThreshold << " 高阈值 = " << highThreashold << endl;
	CvSize searchSize = cvSize(searchImage->width, searchImage->height);
	IplImage* graySearchImg = cvCreateImage(searchSize, IPL_DEPTH_8U, 1);

	// Convert color image to gray image. 
	if (searchImage->nChannels == 3)
		cvCvtColor(searchImage, graySearchImg, CV_RGB2GRAY);
	else
	{
		cvCopy(searchImage, graySearchImg);
	}
	cout << " 找到物体" << " Minumum分数 = " << minScore << " Greediness = " << greediness << "\n\n";
	cout << " ------------------------------------\n";
	clock_t start_time1 = clock();
	score = GM.FindGeoMatchModel(graySearchImg, minScore, greediness, &result);
	clock_t finish_time1 = clock();
	total_time = (double)(finish_time1 - start_time1) / CLOCKS_PER_SEC;
	cout << "识别时间 = " << total_time * 1000 << "ms"<<endl;
	if (score>minScore) // if score is atleast 0.4
	{
		cout << " 识别到的图像位置为： [" << result.x << ", " << result.y << "]\n Score = " << score << "\n 识别时间 = " << total_time * 1000 << "ms";
		GM.DrawContours(searchImage, result, CV_RGB(0, 255, 0), 4);
	}
	else
		cout << " 物体未找到";

	cout << "\n ------------------------------------\n\n";
	cout << "\n 请按下任意案件进行下一步!";

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

