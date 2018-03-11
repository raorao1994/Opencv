// Test05.cpp : �������̨Ӧ�ó������ڵ㡣
//ģ��ƥ��

#include "stdafx.h"
#include <iostream>  
#include <stdio.h>  
#include "opencv2/core.hpp"  
#include "opencv2/core/utility.hpp"  
#include "opencv2/core/ocl.hpp"  
#include "opencv2/imgcodecs.hpp"  
#include "opencv2/highgui.hpp"  
#include "opencv2/features2d.hpp"  
#include "opencv2/calib3d.hpp"  
#include "opencv2/imgproc.hpp"  
#include "opencv2/flann.hpp"  
#include "opencv2/xfeatures2d.hpp"  
#include "opencv2/ml.hpp"  

using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;
using namespace cv::ml;
using namespace cv::flann;

int main()
{
	Mat image01 = imread("D:/1.jpg");
	Mat image02 = imread("D:/2.jpg");
	Mat image1, image2;
	image1 = image01.clone();
	image2 = image02.clone();

	//��ȡ������    
	Ptr<SurfFeatureDetector> surfDetector=SurfFeatureDetector::create(800);  //hessianThreshold,����������ֵ���������޶�������ĸ���   
	vector<KeyPoint> keyPoint1, keyPoint2;
	surfDetector->detect(image1, keyPoint1);
	surfDetector->detect(image2, keyPoint2);

	//����������    
	drawKeypoints(image1, keyPoint1, image1, Scalar::all(-1));
	drawKeypoints(image2, keyPoint2, image2, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	imshow("KeyPoints of image1", image1);
	imshow("KeyPoints of image2", image2);

	//������������Ϊ�±ߵ�������ƥ����׼��    
	Ptr<SurfDescriptorExtractor> SurfDescriptor = SurfDescriptorExtractor::create();
	Mat imageDesc1, imageDesc2;
	SurfDescriptor->compute(image1, keyPoint1, imageDesc1);
	SurfDescriptor->compute(image2, keyPoint2, imageDesc2);

	//��һ������ʾ����������  
	Mat imageDescShow1;
	Mat imageDescShow2;
	normalize(imageDesc1, imageDescShow1, 0, 255, CV_MINMAX);
	normalize(imageDesc2, imageDescShow2, 0, 255, CV_MINMAX);
	convertScaleAbs(imageDescShow1, imageDescShow1);
	convertScaleAbs(imageDescShow2, imageDescShow2);
	imshow("������1", imageDescShow1);
	imshow("������2", imageDescShow2);

	//������ƥ�䲢��ʾƥ����    
	//BruteForceMatcher<L2<float>> matcher;    
	FlannBasedMatcher matcher;
	vector<DMatch> matchePoints;
	matcher.match(imageDesc1, imageDesc2, matchePoints, Mat());

	//�������������  
	cout << "����������ǰ���룺" << endl;
	for (int i = 0; i<matchePoints.size(); i++) //��������㰴��������ǰ����  
	{
		cout << matchePoints[i].distance << endl;
	}
	cout << endl << endl;
	cout << "������sort�������룺" << endl;
	sort(matchePoints.begin(), matchePoints.end()); //�������С��������  
	for (int i = 0; i<matchePoints.size(); i++)//��������㰴��������ǰ������  
	{
		cout << matchePoints[i].distance << endl;
	}

	//��ȡǿ������  
	//��ȡ����ǰN��������ƥ����  
	vector<DMatch> goodMatchePoints;
	for (int i = 0; i<10; i++)
	{
		goodMatchePoints.push_back(matchePoints[i]);
	}

	//��������ƥ���  
	Mat imageOutput;
	drawMatches(image01, keyPoint1, image02, keyPoint2, goodMatchePoints, imageOutput, Scalar::all(-1),
		Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	imwrite("ss.jpg", imageOutput);
	imshow("Mathch Points", imageOutput);
	waitKey();
	return 0;
}

