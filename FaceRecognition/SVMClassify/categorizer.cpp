#include "stdafx.h"
#include "categorizer.h"
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
using namespace cv::xfeatures2d;

//����ѵ������
void categorizer::make_train_set()
{
	cout << "��ȡѵ����..." << endl;
	string categor;

}

//�Ƴ���չ����������ģ����֯����Ŀ
string categorizer::remove_extention(string filename)
{
	int lastIndex = filename.find_last_of(".");
	string name = filename.substr(0, lastIndex);
	return name;
}

categorizer::categorizer()
{

}


categorizer::~categorizer()
{
}

categorizer::categorizer(int _clusters)
{
	cout << "��ʼ��ʼ��...." << endl;
	clusters = _clusters;
	//��ʼ��ָ��
	featureDetector = SurfFeatureDetector::create();
	descriptorExtractor = SurfDescriptorExtractor::create();

	bowtrainerl = new BOWKMeansTrainer(clusters);
	descriptorMatcher = new FlannBasedMatcher();
	bowDescriptorExtractor = new BOWImgDescriptorExtractor(descriptorExtractor, descriptorMatcher);

	cout << "��ʼ�����..." << endl;
	make_train_set();
}

void categorizer::build_vacab()
{

}

void categorizer::compute_bow_image()
{

}

void categorizer::category_by_svm()
{

}
