// SVMClassify.cpp : 定义控制台应用程序的入口点。
// 利用surf特征提取，feature聚类，SVM分类的物体识别算法

#include "stdafx.h"
#include "categorizer.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/xfeatures2d/cuda.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <map>
#include <fstream>
#include "Config.h"
#include<io.h>
#include "categorizer.h"

using namespace std;
using namespace cv;
using namespace cv::ml;
using namespace cv::xfeatures2d;


int main()
{
	int clusters = 1000;
	categorizer c(clusters);
	//特征聚类
	c.build_vacab();
	//构造BOW
	c.compute_bow_image();
	//训练分类器
	c.trainSvm();
	//将测试图片分类
	c.category_by_svm();
    return 0;
}

