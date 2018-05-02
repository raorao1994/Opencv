// ORB_Recognition.cpp : 定义控制台应用程序的入口点。
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

using namespace std;
using namespace cv;

int main()
{
	Mat img1 = imread("d:/phone.jpg", CV_LOAD_IMAGE_GRAYSCALE);

	//识别特征点
	vector<KeyPoint> keypoints1, keypoints2;
	Ptr<ORB> orb=ORB::create();
	orb->detect(img1,keypoints1);
	
	//描述
	Mat descriptors1, descriptors2;
	orb->compute(img1, keypoints1, descriptors1);
	

	VideoCapture cap(0);
	Mat frame,gary;
	//匹配
	BFMatcher matcher(NORM_HAMMING);
	vector<DMatch> matches;
	while (true)
	{
		cap >> frame;
		cvtColor(frame, gary, CV_RGB2GRAY);
		orb->detect(gary, keypoints2);
		if (keypoints2.empty())
			continue;
		orb->compute(gary, keypoints2, descriptors2);
		matcher.match(descriptors1, descriptors2, matches);
		//绘制
		Mat result;
		drawMatches(img1, keypoints1, gary, keypoints2, matches, result);
		imshow("result", result);
		waitKey(2);
	}
	system("pause");
    return 0;
}

