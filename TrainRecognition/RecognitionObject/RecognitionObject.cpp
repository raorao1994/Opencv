// RecognitionObject.cpp : 定义控制台应用程序的入口点。
// 加载模型进行识别

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace cv;

CvHaarClassifierCascade* cascade = 0;
CvMemStorage *storage = 0;

void detect_and_draw(IplImage* img)
{
	double scale = 1.2;
	//Just some pretty colors to draw with
	static CvScalar colors[] = {
		 { 0,0,255 } , { 0,128,255  },{  0,255,255  },{  0,255,0  },
		{  255,128,0  },{  255,255,0  },{ 255,0,0  },{ 255,0,255  }
	};
	  //Image Preparation 
	  // 
	IplImage* gray = cvCreateImage(cvSize(img->width, img->height), 8, 1);
	IplImage* small_img = cvCreateImage(cvSize(cvRound(img->width / scale), cvRound(img->height / scale)), 8, 1);
	cvCvtColor(img, gray, CV_BGR2GRAY);
	cvResize(gray, small_img, CV_INTER_LINEAR);

	cvEqualizeHist(small_img, small_img); //直方图均衡

										  //Detect objects if any 
										  // 
	cvClearMemStorage(storage);
	double t = (double)cvGetTickCount();
	CvSeq* objects = cvHaarDetectObjects(small_img,
		cascade,
		storage,
		1.1,
		2,
		0/*CV_HAAR_DO_CANNY_PRUNING*/,
		cvSize(30, 30));

	t = (double)cvGetTickCount() - t;
	printf("detection time = %gms\n", t / ((double)cvGetTickFrequency()*1000.));

	//Loop through found objects and draw boxes around them 
	for (int i = 0; i<(objects ? objects->total : 0); ++i)
	{
		CvRect* r = (CvRect*)cvGetSeqElem(objects, i);
		cvRectangle(img, cvPoint(r->x*scale, r->y*scale), cvPoint((r->x + r->width)*scale, (r->y + r->height)*scale), colors[i % 8]);
	}
	for (int i = 0; i < (objects ? objects->total : 0); i++)
	{
		CvRect* r = (CvRect*)cvGetSeqElem(objects, i);
		CvPoint center;
		int radius;
		center.x = cvRound((r->x + r->width*0.5)*scale);
		center.y = cvRound((r->y + r->height*0.5)*scale);
		radius = cvRound((r->width + r->height)*0.25*scale);
		cvCircle(img, center, radius, colors[i % 8], 3, 8, 0);
	}

	cvShowImage("result", img);
	cvReleaseImage(&gray);
	cvReleaseImage(&small_img);
}

int main()
{
	char *cascade_name = "hand.xml"; //上文最终生成的xml文件命名为"CASCADE_HEAD_MY.xml"
	cascade = (CvHaarClassifierCascade*)cvLoad(cascade_name, 0, 0, 0); //加载xml文件

	if (!cascade)
	{
		fprintf(stderr, "ERROR: 不能加载分类器\n");
		system("pause");
		return -1;
	}
	storage = cvCreateMemStorage(0);
	cvNamedWindow("face", 1);

	const char* filename = "1.jpg";
	IplImage* image = cvLoadImage(filename, 1);

	if (image)
	{
		detect_and_draw(image); //函数见下方
		cvWaitKey(0);
		cvReleaseImage(&image);
	}
	cvDestroyWindow("result");
	return 0;
}

