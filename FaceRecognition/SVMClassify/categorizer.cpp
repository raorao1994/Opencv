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
#include "Config.h"

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
//���캯��
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
// ѵ��ͼƬfeature���࣬�ó��ʵ�
void categorizer::build_vacab()
{
	FileStorage vacab_fs(DATA_FOLDER "vocab.xml", FileStorage::READ);
	//���֮ǰ�Ѿ����ɺã��Ͳ���Ҫ���¾������ɴʵ�
	if (vacab_fs.isOpened())
	{
		cout << "ͼƬ�Ѿ����࣬�ʵ��Ѿ�����.." << endl;
		vacab_fs.release();
	}
	else
	{
		Mat vocab_descriptors;
		// ����ÿһ��ģ�壬��ȡSURF���ӣ����뵽vocab_descriptors��
		multimap<string, Mat> ::iterator i = train_set.begin();
		for (; i != train_set.end(); i++)
		{
			vector<KeyPoint>kp;
			Mat templ = (*i).second;
			Mat descrip;
			featureDetector->detect(templ, kp);
			descriptorExtractor->compute(templ, kp, descrip);
			//push_back(Mat);��ԭ����Mat�����һ�к��ټӼ���,Ԫ��ΪMatʱ�� �����ͺ��е���Ŀ ����;�����������ͬ��
			vocab_descriptors.push_back(descrip);
		}
		cout << "ѵ��ͼƬ��ʼ����..." << endl;
		//��ÿһ��ͼ��Surf��������add�������뵽bowTraining��ȥ,�Ϳ��Խ��о���ѵ����
		bowtrainer->add(vocab_descriptors);
		// ��SURF�����ӽ��о���
		vocab = bowtrainer->cluster();
		cout << "������ϣ��ó��ʵ�..." << endl;

		//���ļ���ʽ����ʵ�
		FileStorage file_stor(DATA_FOLDER "vocab.xml", FileStorage::WRITE);
		file_stor << "vocabulary" << vocab;
		file_stor.release();
	}
}

void categorizer::compute_bow_image()
{

}

void categorizer::category_by_svm()
{

}
