// ObjectMarker.cpp : 定义控制台应用程序的入口点。
// 负样本生成器

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace cv;

//从背景图片中随机抽取图像块，多用于生成负样本
#define kImageBlockWidth 200        //图像块大小
#define kImageBlockHeight 200        
#define kLoopTimes 50    //期望样本数

int main()
{
	int originX = 0, originY = 0;
	int width_limited = 0, height_limited = 0;
	int width = 0, height = 0;
	//IplImage *bgImage = cvLoadImage("../data/neg/bg.jpg");
	IplImage *bgImage = cvLoadImage("08.jpg");
	IplImage *blockImage = cvCreateImage(cvSize(kImageBlockWidth, kImageBlockHeight), bgImage->depth, bgImage->nChannels);
	width = bgImage->width;
	height = bgImage->height;
	width_limited = width - kImageBlockWidth;
	height_limited = height - kImageBlockHeight;
	cout << width_limited << "   " << height_limited;
	for (int i = 0; i < kLoopTimes; i++)
	{
		originX = rand() % width_limited;
		originY = rand() % height_limited;
		cvZero(blockImage);
		CvPoint2D32f center_block = cvPoint2D32f(originX + kImageBlockWidth / 2, originY + kImageBlockHeight / 2);
		cvGetRectSubPix(bgImage, blockImage, center_block);
		char saveFileName[100] = { '\0' };
		sprintf(saveFileName, "../data/(%d).jpg", i + 1);
		cvSaveImage(saveFileName, blockImage);
	}

	cvReleaseImage(&bgImage);
	cvReleaseImage(&blockImage);
	system("pause");
	return 0;
}

