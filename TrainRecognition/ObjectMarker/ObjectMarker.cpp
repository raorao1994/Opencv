// ObjectMarker.cpp : �������̨Ӧ�ó������ڵ㡣
// ������������

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace cv;

//�ӱ���ͼƬ�������ȡͼ��飬���������ɸ�����
#define kImageBlockWidth 200        //ͼ����С
#define kImageBlockHeight 200        
#define kLoopTimes 50    //����������

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

