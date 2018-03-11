// 基于边缘的特征识别.cpp : 定义控制台应用程序的入口点。
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

//定义全局变量
//提取特征点
Ptr<SurfFeatureDetector> surfDetector = SurfFeatureDetector::create(800);
//特征点描述，为下边的特征点匹配做准备    
Ptr<SurfDescriptorExtractor> SurfDescriptor = SurfDescriptorExtractor::create();

//声明方法
vector<KeyPoint> GetTempleKetPoint(string path, OutputArray ContourImg);
Mat GetContourImg(string path);

int main()
{
	//1、加载模版图像,并找出图像边缘特征点
	Mat TempImg;
	vector<KeyPoint> TempKeyPoint= GetTempleKetPoint("t1.jpg", TempImg);
	//2、加载待识别图像，获取图像边缘图像
	Mat SrarchImg;
	vector<KeyPoint> SrarchKeyPoint = GetTempleKetPoint("1.jpg",SrarchImg);
	//3、根据模版识别特征点，在边缘图像中识别对应点，
	//3.1对特征点进行描述
	Mat imageDesc1, imageDesc2;
	SurfDescriptor->compute(TempImg, TempKeyPoint, imageDesc1);
	SurfDescriptor->compute(SrarchImg, SrarchKeyPoint, imageDesc2);
	//特征点匹配并显示匹配结果       
	FlannBasedMatcher matcher;
	vector<DMatch> matchePoints;
	matcher.match(imageDesc1, imageDesc2, matchePoints, Mat());

	//特征点排序并输出  
	cout << "特征点排序前距离：" << endl;
	for (int i = 0; i<matchePoints.size(); i++) //输出特征点按距离排序前内容  
	{
		cout << matchePoints[i].distance << endl;
	}
	sort(matchePoints.begin(), matchePoints.end()); //按距离从小到大排序
	cout << "特征点排序后距离：" << endl;
	for (int i = 0; i<matchePoints.size(); i++)
	{
		cout << matchePoints[i].distance << endl;
	}

	//提取强特征点，获取排在前N个的最优匹配结果
	vector<DMatch> goodMatchePoints;
	for (int i = 0; i<10; i++)
	{
		goodMatchePoints.push_back(matchePoints[i]);
	}

	//绘制最优匹配点  
	Mat imageOutput;
	drawMatches(TempImg, TempKeyPoint, SrarchImg, SrarchKeyPoint, goodMatchePoints, imageOutput, Scalar::all(-1),
		Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	imshow("imageOutput", imageOutput);
	waitKey(0);
    return 0;
}
//获取模版特征点
vector<KeyPoint> GetTempleKetPoint(string path, OutputArray ContourImg)
{
	//1.1获取图片边缘图像
	Mat ContoursImg = GetContourImg(path);
	ContourImg.create(ContoursImg.size(), ContoursImg.type());
	//Mat dst = ContourImg.getMat();
	ContoursImg.copyTo(ContourImg);
	//1.2检测模版特征点
	vector<KeyPoint> KeyPoint;
	surfDetector->detect(ContoursImg, KeyPoint);
	drawKeypoints(ContoursImg, KeyPoint, ContoursImg, Scalar::all(-1));
	//释放ContoursImg内存资源
	ContoursImg.release();
	return KeyPoint;
}
//获取图片边缘图像
Mat GetContourImg(string path)
{
	Mat TempleImgSrc = imread(path, CV_8UC1);
	Mat TempleImg;
	if (TempleImgSrc.channels() == 3)
	{
		cvtColor(TempleImgSrc, TempleImg, CV_BGR2GRAY);
	}
	else
	{
		TempleImgSrc.copyTo(TempleImg);
	}
	//释放原图内存资源
	TempleImgSrc.release();
	//1.1对模版图像进行边缘二值化处理
	Mat ThresholdImg;
	threshold(TempleImg, ThresholdImg, 100, 255, THRESH_BINARY | THRESH_OTSU);
	//释放模版内存资源
	TempleImg.release();
	//1.2对二值图像进行边缘提取操作
	vector<vector<Point>> contours;
	findContours(ThresholdImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	Mat ContoursImg = Mat::zeros(ThresholdImg.size(), CV_8UC1);
	drawContours(ContoursImg, contours, -1, Scalar(255), 4);
	return ContoursImg;
}
