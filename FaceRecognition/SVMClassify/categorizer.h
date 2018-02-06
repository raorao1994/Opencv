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
	//����Ŀ���Ƶ����ݵ�mapӳ��
	map<string, Mat> result_bjects;
	//�������ѵ��ͼ����ӳ��
	map<string, Mat> allsample_bow;
	//����Ŀ���Ƶ�ѵ��ͼ����ӳ�䣬�ؼ��ֿ����ظ�����
	multimap<string, Mat> train_set;
	//ѵ���ĵõ���SVM
	SVM *stor_svms;
	//��Ŀ���ƣ�Ҳ����train_folder���õ�Ŀ¼��
	vector<string> category_name;
	//��Ŀ��Ŀ
	int category_size;
	//��SURF���������Ӿ��ʻ�ľ�����Ŀ
	int clusters;
	//���ѵ����ͼƬ�ʵ�
	Mat vocab;

	//���������detectors����������ȡ��extractors   ���;����Ptr
	Ptr<FastFeatureDetector> featureDetector;
	Ptr<DescriptorExtractor> descriptorExtractor;

	Ptr<BOWKMeansTrainer> bowtrainer;
	Ptr<BOWImgDescriptorExtractor> bowDescriptorExtractor;
	Ptr<FlannBasedMatcher> descriptorMatcher;

	//ѵ������
	void make_train_set();
	//�Ƴ���չ����������ģ����֯����Ŀ
	string remove_extention(string);
public:
	categorizer();
	~categorizer();
	//���캯��
	categorizer(int);
	//��۵õ��ֵ�
	void build_vacab();
	//����BOW
	void compute_bow_image();
	//������ͼƬ����
	void category_by_svm();
};

