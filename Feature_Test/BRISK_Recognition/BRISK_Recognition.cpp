// BRISK_Recognition.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/xfeatures2d.hpp>
#include<Windows.h> 

using namespace std;
using namespace cv;

void BRISK1();
void BRISK2();

int main()
{
	BRISK1();
	//BRISK2();
	system("pause");
    return 0;
}
//图片匹配
void BRISK1()
{
	//Load Image     
	Mat c_src1 = imread("D:/6.jpg");
	Mat c_src2 = imread("D:/7.jpg");
	Mat src1 = imread("D:/6.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	Mat src2 = imread("D:/7.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	if (!src1.data || !src2.data)
	{
		std::cout << " --(!) Error reading images " << std::endl; ;
	}

	//sift feature detect    
	Ptr<BRISK> detector= BRISK::create();
	vector<KeyPoint> kp1, kp2;
	detector->detect(src1, kp1);
	detector->detect(src2, kp2);
	//cv::BRISK extractor;    
	Mat des1, des2;//descriptor    
	detector->compute(src1, kp1, des1);
	detector->compute(src2, kp2, des2);
	Mat res1, res2;
	int drawmode = DrawMatchesFlags::DRAW_RICH_KEYPOINTS;
	drawKeypoints(c_src1, kp1, res1, Scalar::all(-1), drawmode);//在内存中画出特征点    
	drawKeypoints(c_src2, kp2, res2, Scalar::all(-1), drawmode);
	cout << "size of description of Img1: " << kp1.size() << endl;
	cout << "size of description of Img2: " << kp2.size() << endl;

	BFMatcher matcher(NORM_HAMMING);
	vector<DMatch> matches;
	matcher.match(des1, des2, matches);
	Mat img_match;
	drawMatches(src1, kp1, src2, kp2, matches, img_match);//,Scalar::all(-1),Scalar::all(-1),vector<char>(),drawmode);    
	cout << "number of matched points: " << matches.size() << endl;
	imshow("matches", img_match);
	cvWaitKey();
	cvDestroyAllWindows();
}

void BRISK2()
{
	//Load Image       
	Mat c_src1 = imread("d:/me.bmp");
	Mat src1 = imread("d:/me.bmp", CV_LOAD_IMAGE_GRAYSCALE);

	Ptr<BRISK> detector= BRISK::create();
	vector<KeyPoint> kp1, kp2;
	detector->detect(src1, kp1);

	Mat des1, des2;//descriptor      
	detector->compute(src1, kp1, des1);

	Mat res1, res2;
	int drawmode = DrawMatchesFlags::DRAW_RICH_KEYPOINTS;
	drawKeypoints(c_src1, kp1, res1, Scalar::all(-1), drawmode);//在内存中画出特征点      

	BFMatcher matcher(NORM_HAMMING);
	vector<DMatch> matches;

	Mat imgMatch;

	VideoCapture cap(0);
	Mat frame;
	Mat frameGray;
	vector<KeyPoint> kpf;
	Mat desf;
	Mat img_match;
	while (1) {
		cap >> frame;
		cvtColor(frame, frameGray, CV_RGB2GRAY);
		detector->detect(frameGray, kpf);
		if (kpf.empty())
			continue;
		//drawKeypoints(frameGray, kpf, frame, Scalar::all(-1), drawmode);
		detector->compute(frame, kpf, desf);
		matcher.match(des1, desf, matches);
		//imshow("frame", frame);
		//cout << "特征点sort排序后距离：" << endl;
		//sort(matches.begin(), matches.end()); //按距离从小到大排序  
		//提取强特征点  
		//获取排在前N个的最优匹配结果  
		//vector<DMatch> goodMatchePoints;
		/*for (int i = 0; i<20; i++)
		{
			goodMatchePoints.push_back(matches[i]);
		}*/
		/*for (int i = 0; i<matches.size(); i++)
		{
			if(matches[i].distance<60)
				goodMatchePoints.push_back(matches[i]);
		}
		cout << goodMatchePoints[0].distance << endl;*/
		//drawMatches(src1, kp1, frame, kpf, matches, img_match);
		drawMatches(src1, kp1, frame, kpf, matches, img_match);
		imshow("match", img_match);
		waitKey(2);
	}
}
