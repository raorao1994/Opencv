// Test05.cpp : 定义控制台应用程序的入口点。
//模版匹配

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

	//提取特征点    
	Ptr<SurfFeatureDetector> surfDetector=SurfFeatureDetector::create(800);  //hessianThreshold,海塞矩阵阈值，并不是限定特征点的个数   
	vector<KeyPoint> keyPoint1, keyPoint2;
	surfDetector->detect(image1, keyPoint1);
	surfDetector->detect(image2, keyPoint2);

	//绘制特征点    
	drawKeypoints(image1, keyPoint1, image1, Scalar::all(-1));
	drawKeypoints(image2, keyPoint2, image2, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	imshow("KeyPoints of image1", image1);
	imshow("KeyPoints of image2", image2);

	//特征点描述，为下边的特征点匹配做准备    
	Ptr<SurfDescriptorExtractor> SurfDescriptor = SurfDescriptorExtractor::create();
	Mat imageDesc1, imageDesc2;
	SurfDescriptor->compute(image1, keyPoint1, imageDesc1);
	SurfDescriptor->compute(image2, keyPoint2, imageDesc2);

	//归一化并显示出来描述子  
	Mat imageDescShow1;
	Mat imageDescShow2;
	normalize(imageDesc1, imageDescShow1, 0, 255, CV_MINMAX);
	normalize(imageDesc2, imageDescShow2, 0, 255, CV_MINMAX);
	convertScaleAbs(imageDescShow1, imageDescShow1);
	convertScaleAbs(imageDescShow2, imageDescShow2);
	imshow("描述子1", imageDescShow1);
	imshow("描述子2", imageDescShow2);

	//特征点匹配并显示匹配结果    
	//BruteForceMatcher<L2<float>> matcher;    
	FlannBasedMatcher matcher;
	vector<DMatch> matchePoints;
	matcher.match(imageDesc1, imageDesc2, matchePoints, Mat());

	//特征点排序并输出  
	cout << "特征点排序前距离：" << endl;
	for (int i = 0; i<matchePoints.size(); i++) //输出特征点按距离排序前内容  
	{
		cout << matchePoints[i].distance << endl;
	}
	cout << endl << endl;
	cout << "特征点sort排序后距离：" << endl;
	sort(matchePoints.begin(), matchePoints.end()); //按距离从小到大排序  
	for (int i = 0; i<matchePoints.size(); i++)//输出特征点按距离排序前后内容  
	{
		cout << matchePoints[i].distance << endl;
	}

	//提取强特征点  
	//获取排在前N个的最优匹配结果  
	vector<DMatch> goodMatchePoints;
	for (int i = 0; i<10; i++)
	{
		goodMatchePoints.push_back(matchePoints[i]);
	}

	//绘制最优匹配点  
	Mat imageOutput;
	drawMatches(image01, keyPoint1, image02, keyPoint2, goodMatchePoints, imageOutput, Scalar::all(-1),
		Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	imwrite("ss.jpg", imageOutput);
	imshow("Mathch Points", imageOutput);
	waitKey();
	return 0;
}

