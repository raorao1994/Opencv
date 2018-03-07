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
#include"opencv2/flann.hpp"  
#include"opencv2/xfeatures2d.hpp"  
#include"opencv2/ml.hpp"  

using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;
using namespace cv::ml;
using namespace cv::flann;

int main()
{
	//-------------读取模板------------
	cv::Mat img_object = imread("D:/3.jpg");
	//-------------图像处理---------
	cv::Mat img_scene = imread("D:/2.jpg");

	/*
	// 检测surf特征点
	int minHessian = 400;
	OrbDescriptorExtractor detector(minHessian);

	std::vector<KeyPoint> keypoints_1, keypoints_2;
	detector.detect(img_1, keypoints_1);
	detector.detect(img_2, keypoints_2);
	//-- Step 2: Calculate descriptors (feature vectors)
	OrbDescriptorExtractor extractor;
	Mat descriptors_1, descriptors_2;
	extractor.compute(img_1, keypoints_1, descriptors_1);
	extractor.compute(img_2, keypoints_2, descriptors_2);

	//-- Step 3: Matching descriptor vectors with a brute force matcher
	BFMatcher matcher(NORM_L2);
	std::vector< DMatch > matches;
	matcher.match(descriptors_1, descriptors_2, matches);

	//-- Draw matches
	Mat img_matches;
	drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, img_matches);*/

	// 读取数据
	//cv::Mat img_object = cv::imread("doll01.jpg");
	//cv::Mat img_scene = cv::imread("doll012.jpg");
	if (!img_object.data || !img_scene.data) {
		cout << "Error reading images." << endl;
		return 0;
	}

	// 构建特征检测器和描述子提取器
	Ptr<SURF> detector = SURF::create(800);
	Ptr<SURF> descriptor = SURF::create();

	// 检测特征点
	vector<KeyPoint> kp_object, kp_scene;
	detector->detect(img_object, kp_object);
	detector->detect(img_scene, kp_scene);

	// 计算描述子
	Mat desp_object, desp_scene;
	descriptor->compute(img_object, kp_object, desp_object);
	descriptor->compute(img_scene, kp_scene, desp_scene);

	/*
	if (desp_object.type() != CV_32F) {
	desp_object.convertTo(desp_object, CV_32F);
	}

	if (desp_scene.type() != CV_32F) {
	desp_scene.convertTo(desp_scene, CV_32F);
	}
	*/

	// 匹配描述子
	vector<DMatch> matches;
	BFMatcher matcher;
	//FlannBasedMatcher matcher(new LshIndexParams(20, 10, 2));
	//matcher.match(desp_object, desp_scene, matches);
	matcher.match(desp_object, desp_scene, matches);
	//cout << "Find total " << matches.size() << " matches." << endl;

	// 筛选匹配
	//double min_dist = 100000;
	//for (int i = 0; i < matches.size(); i++) {
	//	float a = matches[i].distance;
	//	if (a < min_dist) {
	//		min_dist = matches[i].distance;
	//	}
	//}

	//vector<cv::DMatch> good_matches;
	//for (int i = 0; i < matches.size(); i++) {
	//
	//	if (matches[i].distance < 3 * min_dist) {
	//		good_matches.push_back(matches[i]);
	//	}
	//}

	// 显示匹配
	//cout << "Good matches=" << matches.size() << endl;
	Mat img_matches;
	drawMatches(img_object, kp_object, img_scene, kp_scene, matches, img_matches);

	// 定位目标
	vector<Point2f> obj_points;
	vector<Point2f> scene;

	for (int i = 0; i < matches.size(); i++) {
		obj_points.push_back(kp_object[matches[i].queryIdx].pt);
		scene.push_back(kp_scene[matches[i].trainIdx].pt);
	}
	Mat H = findHomography(obj_points, scene, CV_RANSAC);


	vector<Point2f> obj_corners(4);
	vector<Point2f> scene_corners(4);
	obj_corners[0] = Point(0, 0);
	obj_corners[1] = Point(img_object.cols, 0);
	obj_corners[2] = Point(img_object.cols, img_object.rows);
	obj_corners[3] = Point(0, img_object.rows);

	perspectiveTransform(obj_corners, scene_corners, H);

	line(img_matches, scene_corners[0] + Point2f(img_object.cols, 0), scene_corners[1] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[1] + Point2f(img_object.cols, 0), scene_corners[2] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[2] + Point2f(img_object.cols, 0), scene_corners[3] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, scene_corners[3] + Point2f(img_object.cols, 0), scene_corners[0] + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);

	imshow("结果", img_matches);
	cvWaitKey(0);
	return 0;
	//Mat dstSize;
	//resize(img_matches, dstSize, Size(2 * h, w));
}

