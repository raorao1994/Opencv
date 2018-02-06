#pragma once
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <map>
#include <fstream>

using namespace std;
using namespace cv;
using namespace cv::ml;

class categorizer
{
private:
	//从类目名称到数据的map映射
	map<string, Mat> result_bjects;
	//存放所有训练图集的映射
	map<string, Mat> allsample_bow;
	//从类目名称到训练图集的映射，关键字可以重复出现
	multimap<string, Mat> train_set;
	//训练的得到的SVM
	SVM *stor_svms;
	//类目名称，也就是train_folder设置的目录名
	vector<string> category_name;
	//类目数目
	int category_size;
	//用SURF特征构造视觉词汇的聚类数目
	int clusters;
	//存放训练的图片词典
	Mat vocab;

	//特征检测器detectors与描述子提取器extractors   泛型句柄类Ptr
	Ptr<FastFeatureDetector> featureDetector;
	Ptr<DescriptorExtractor> descriptorExtractor;

	Ptr<BOWKMeansTrainer> bowtrainer;
	Ptr<BOWImgDescriptorExtractor> bowDescriptorExtractor;
	Ptr<FlannBasedMatcher> descriptorMatcher;

	//训练集合
	void make_train_set();
	//移除扩展名、用来讲模版组织成类目
	string remove_extention(string);
public:
	categorizer();
	~categorizer();
	//构造函数
	categorizer(int);
	//类聚得到字典
	void build_vacab();
	//构造BOW
	void compute_bow_image();
	//将测试图片分类
	void category_by_svm();
};

