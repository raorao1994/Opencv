// SVMClassify.cpp : �������̨Ӧ�ó������ڵ㡣
// ����surf������ȡ��feature���࣬SVM���������ʶ���㷨

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
	//��������
	c.build_vacab();
	//����BOW
	c.compute_bow_image();
	//ѵ��������
	c.trainSvm();
	//������ͼƬ����
	c.category_by_svm();
    return 0;
}

