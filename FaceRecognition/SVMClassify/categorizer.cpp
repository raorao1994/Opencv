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

//构造训练集合
void categorizer::make_train_set()
{
	cout << "读取训练集..." << endl;
	string categor;

}

//移除扩展名，用来讲模板组织成类目
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
//构造函数
categorizer::categorizer(int _clusters)
{
	cout << "开始初始化...." << endl;
	clusters = _clusters;
	//初始化指针
	featureDetector = SurfFeatureDetector::create();
	descriptorExtractor = SurfDescriptorExtractor::create();

	bowtrainerl = new BOWKMeansTrainer(clusters);
	descriptorMatcher = new FlannBasedMatcher();
	bowDescriptorExtractor = new BOWImgDescriptorExtractor(descriptorExtractor, descriptorMatcher);

	cout << "初始化完毕..." << endl;
	make_train_set();
}
// 训练图片feature聚类，得出词典
void categorizer::build_vacab()
{
	FileStorage vacab_fs(DATA_FOLDER "vocab.xml", FileStorage::READ);
	//如果之前已经生成好，就不需要重新聚类生成词典
	if (vacab_fs.isOpened())
	{
		cout << "图片已经聚类，词典已经存在.." << endl;
		vacab_fs.release();
	}
	else
	{
		Mat vocab_descriptors;
		// 对于每一幅模板，提取SURF算子，存入到vocab_descriptors中
		multimap<string, Mat> ::iterator i = train_set.begin();
		for (; i != train_set.end(); i++)
		{
			vector<KeyPoint>kp;
			Mat templ = (*i).second;
			Mat descrip;
			featureDetector->detect(templ, kp);
			descriptorExtractor->compute(templ, kp, descrip);
			//push_back(Mat);在原来的Mat的最后一行后再加几行,元素为Mat时， 其类型和列的数目 必须和矩阵容器是相同的
			vocab_descriptors.push_back(descrip);
		}
		cout << "训练图片开始聚类..." << endl;
		//将每一副图的Surf特征利用add函数加入到bowTraining中去,就可以进行聚类训练了
		bowtrainer->add(vocab_descriptors);
		// 对SURF描述子进行聚类
		vocab = bowtrainer->cluster();
		cout << "聚类完毕，得出词典..." << endl;

		//以文件格式保存词典
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
