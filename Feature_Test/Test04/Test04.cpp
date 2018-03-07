// Test04.cpp : 定义控制台应用程序的入口点。
//surf识别对应物体

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

int main()
{
	Mat a = imread("D:/3.jpg", IMREAD_GRAYSCALE);    //读取灰度图像  
	Mat b = imread("D:/2.jpg", IMREAD_GRAYSCALE);

	Ptr<SURF> surf;      //创建方式和2中的不一样  
	surf = SURF::create(800);

	BFMatcher matcher;
	Mat c, d;
	vector<KeyPoint>key1, key2;
	vector<DMatch> matches;

	surf->detectAndCompute(a, Mat(), key1, c);
	surf->detectAndCompute(b, Mat(), key2, d);

	matcher.match(c, d, matches);       //匹配  

	sort(matches.begin(), matches.end());  //筛选匹配点  
	vector< DMatch > good_matches;
	int ptsPairs = std::min(50, (int)(matches.size() * 0.15));
	cout << ptsPairs << endl;
	for (int i = 0; i < ptsPairs; i++)
	{
		good_matches.push_back(matches[i]);
	}
	Mat outimg;
	drawMatches(a, key1, b, key2, good_matches, outimg, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //绘制匹配点  

	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	for (size_t i = 0; i < good_matches.size(); i++)
	{
		obj.push_back(key1[good_matches[i].queryIdx].pt);
		scene.push_back(key2[good_matches[i].trainIdx].pt);
	}

	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = Point(0, 0);
	obj_corners[1] = Point(a.cols, 0);
	obj_corners[2] = Point(a.cols, a.rows);
	obj_corners[3] = Point(0, a.rows);
	std::vector<Point2f> scene_corners(4);

	Mat H = findHomography(obj, scene, RANSAC);      //寻找匹配的图像  
	perspectiveTransform(obj_corners, scene_corners, H);

	line(outimg, scene_corners[0] + Point2f((float)a.cols, 0), scene_corners[1] + Point2f((float)a.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);       //绘制  
	line(outimg, scene_corners[1] + Point2f((float)a.cols, 0), scene_corners[2] + Point2f((float)a.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);
	line(outimg, scene_corners[2] + Point2f((float)a.cols, 0), scene_corners[3] + Point2f((float)a.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);
	line(outimg, scene_corners[3] + Point2f((float)a.cols, 0), scene_corners[0] + Point2f((float)a.cols, 0), Scalar(0, 255, 0), 2, LINE_AA);
	imshow("输出图片", outimg);
	cvWaitKey(0);
	return 0;
}

