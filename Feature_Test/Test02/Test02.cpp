// Test02.cpp : �������̨Ӧ�ó������ڵ㡣
//SURF����ʶ��-��������

#include "stdafx.h"
#include "opencv2/core.hpp" 
#include <opencv2/imgcodecs.hpp>
#include "opencv2/highgui.hpp" 
#include "opencv2/features2d.hpp"  
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;
int main()
{
	Mat img = imread("D:/1.jpg");
	
	Ptr<SURF> surf = SURF::create(800);

	vector<KeyPoint> key;

	surf->detect(img,key);
	Mat outImg;
	drawKeypoints(img, key, outImg);

	imshow("���",outImg);

	while (waitKey(0)==27)
	{
		
	}
    return 0;
}

