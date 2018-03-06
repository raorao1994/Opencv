// Test03.cpp : 定义控制台应用程序的入口点。
//SVM特征点图像分类

#include "stdafx.h"
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
#include<io.h>

#define DATA_FOLDER "E:/SVN/OpenCV/trunk/Data/svm_data"
#define TRAIN_FOLDER "E:/SVN/OpenCV/trunk/Data/svm_data/train_images"
#define TEMPLATE_FOLDER "E:/SVN/OpenCV/trunk/Data/svm_data/templates"
#define TEST_FOLDER "E:/SVN/OpenCV/trunk/Data/svm_data/test_image"
#define RESULT_FOLDER "E:/SVN/OpenCV/trunk/Data/svm_data/result_image"

using namespace std;
using namespace cv;
using namespace cv::ml;
using namespace cv::xfeatures2d;

//从类目名称到数据的map映射
map<string, Mat> result_objects;
//存放所有训练图集的映射
map<string, Mat> allsample_bow;
//从类目名称到训练图集的映射，关键字可以重复出现
multimap<string, Mat> train_set;
//训练的得到的SVM
Ptr<SVM> stor_svms;
//类目名称，也就是train_folder设置的目录名
vector<string> category_name;
//类目数目
int category_size;
//用SURF特征构造视觉词汇的聚类数目
int clusters;
//存放训练的图片词典
Mat vocab;

//特征检测器detectors与描述子提取器extractors泛型句柄类Ptr
Ptr<SurfFeatureDetector> featureDetector;
Ptr<DescriptorExtractor> descriptorExtractor;

Ptr<BOWKMeansTrainer> bowtrainer;
Ptr<BOWImgDescriptorExtractor> bowDescriptorExtractor;
Ptr<FlannBasedMatcher> descriptorMatcher;

//定义函数
void make_train_set();
string remove_extention(string filename);
void categorizer(int _clusters);
void build_vacab();
void compute_bow_image();
void trainSvm();
void category_by_svm();
void GetAllImg(string categor);

int main()
{
    return 0;
}

//构造训练集合
void make_train_set()
{
	cout << "读取训练集..." << endl;
	string categor;
	//递归迭代rescursive 直接定义两个迭代器：i为迭代起点（有参数），end_iter迭代终点
	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(TRAIN_FOLDER).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			//如果不是,加入列表  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				// 将类目名称设置为目录的名称
				//categor = p.assign(TRAIN_FOLDER).append("/").append(fileinfo.name);
				//添加类别名到类别目录
				//category_name.push_back(categor);
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					categor = fileinfo.name;
					GetAllImg(categor);
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	category_size = category_name.size();
	cout << "发现 " << category_size << "种类别物体..." << endl;
}
//移除扩展名，用来讲模板组织成类目
string remove_extention(string filename)
{
	int lastIndex = filename.find_last_of(".");
	string name = filename.substr(0, lastIndex);
	return name;
}
//构造函数
void categorizer(int _clusters)
{
	cout << "开始初始化...." << endl;
	clusters = _clusters;
	//初始化指针
	featureDetector = SurfFeatureDetector::create(800);
	descriptorExtractor = SurfDescriptorExtractor::create();

	bowtrainer = new BOWKMeansTrainer(clusters);
	descriptorMatcher = new FlannBasedMatcher();
	bowDescriptorExtractor = new BOWImgDescriptorExtractor(descriptorExtractor, descriptorMatcher);

	//遍历数据文件夹  directory_iterator(p)就是迭代器的起点，
	//文件句柄  
	long   hFile = 0;
	//文件信息 
	//添加测试数据
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(TEMPLATE_FOLDER).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			//如果不是,加入列表  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
			}
			else
			{
				string filename = p.assign(TEMPLATE_FOLDER).append(fileinfo.name);
				string sub_category = remove_extention(fileinfo.name);
				//读入模板图片
				Mat image = imread(filename);
				//存储原图模板
				result_objects[sub_category] = image;
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	cout << "初始化完毕..." << endl;
	//读取训练集
	make_train_set();
}
// 训练图片feature聚类，得出词典
void build_vacab()
{
	FileStorage vacab_fs(DATA_FOLDER "/vocab.xml", FileStorage::READ);
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
			//push_back(Mat);在原来的Mat的最后一行后再加几行,元素为Mat时,
			//其类型和列的数目 必须和矩阵容器是相同的
			vocab_descriptors.push_back(descrip);
		}
		cout << "训练图片开始聚类..." << endl;
		//将每一副图的Surf特征利用add函数加入到bowTraining中去,就可以进行聚类训练了
		bowtrainer->add(vocab_descriptors);
		// 对SURF描述子进行聚类
		vocab = bowtrainer->cluster();
		cout << "聚类完毕，得出词典..." << endl;

		//以文件格式保存词典
		FileStorage file_stor(DATA_FOLDER "/vocab.xml", FileStorage::WRITE);
		file_stor << "vocabulary" << vocab;
		file_stor.release();
	}
}
//构造bag of words
void compute_bow_image()
{
	cout << "构造bag of words..." << endl;
	FileStorage va_fs(DATA_FOLDER "/vocab.xml", FileStorage::READ);
	//如果词典存在则直接读取
	if (va_fs.isOpened())
	{
		Mat temp_vacab;
		va_fs["vocabulary"] >> temp_vacab;
		bowDescriptorExtractor->setVocabulary(temp_vacab);
		va_fs.release();
	}
	else
	{
		//对每张图片的特征点，统计这张图片各个类别出现的频率，作为这张图片的bag of words
		bowDescriptorExtractor->setVocabulary(vocab);
	}
	//如果bow.txt已经存在说明之前已经训练过了，下面就不用重新构造BOW
	string bow_path = string(DATA_FOLDER) + string("/bow.txt");
	ifstream read_file(bow_path);
	//如BOW已经存在，则不需要构造
	if (read_file && !read_file)
	{
		//读取allsample_bow
		/*FileStorage allsimple_data(DATA_FOLDER "/allsample_bow.xml", FileStorage::READ);
		allsimple_data["allsample_bow"] >> allsample_bow;
		allsimple_data.release();*/
		cout << "BOW 已经准备好..." << endl;
	}
	else {
		// 对于每一幅模板，提取SURF算子，存入到vocab_descriptors中
		multimap<string, Mat> ::iterator i = train_set.begin();
		for (; i != train_set.end(); i++)
		{
			vector<KeyPoint>kp;
			string cate_nam = (*i).first;
			Mat tem_image = (*i).second;
			Mat imageDescriptor;
			featureDetector->detect(tem_image, kp);
			bowDescriptorExtractor->compute(tem_image, kp, imageDescriptor);
			//push_back(Mat);在原来的Mat的最后一行后再加几行,元素为Mat时，
			//其类型和列的数目 必须和矩阵容器是相同的
			allsample_bow[cate_nam].push_back(imageDescriptor);
		}
		//保存allsample_bow
		/*FileStorage samplefile_stor(DATA_FOLDER "/allsample_bow.xml", FileStorage::WRITE);
		if (samplefile_stor.isOpened())
		{
		samplefile_stor << "allsample_bow" << allsample_bow;
		samplefile_stor.release();
		}*/
		//简单输出一个文本，为后面判断做准备
		ofstream ous(bow_path);
		ous << "flag";
		cout << "bag of words构造完毕..." << endl;
	}
}
//训练分类器
void trainSvm()
{
	int flag = 0;
	for (int k = 0; k<category_size; k++)
	{
		string svm_file_path = string(DATA_FOLDER) + "/" + category_name[k] + string("_SVM.xml");
		FileStorage svm_fil(svm_file_path, FileStorage::READ);
		//判断训练结果是否存在
		if (svm_fil.isOpened())
		{
			svm_fil.release();
			continue;
		}
		else
		{
			flag = -1;
			break;
		}
	}
	//如果训练结果已经存在则不需要重新训练
	if (flag != -1)
	{
		cout << "分类器已经训练完毕..." << endl;
	}
	else
	{
		stor_svms = SVM::create();
		//设置训练参数
		stor_svms->setType(SVM::C_SVC);
		stor_svms->setKernel(SVM::RBF);//CvSVM::LINEAR POLY  RBF
		stor_svms->setDegree(1.0);
		//stor_svms->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));
		stor_svms->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 1000, FLT_EPSILON));
		cout << "训练分类器..." << endl;
		for (int i = 0; i<category_size; i++)
		{
			Mat tem_Samples(0, allsample_bow.at(category_name[i]).cols,
				allsample_bow.at(category_name[i]).type());
			Mat responses(0, 1, CV_32SC1);
			tem_Samples.push_back(allsample_bow.at(category_name[i]));
			Mat posResponses(allsample_bow.at(category_name[i]).rows, 1, CV_32SC1, Scalar::all(1));
			responses.push_back(posResponses);
			for (auto itr = allsample_bow.begin(); itr != allsample_bow.end(); ++itr)
			{
				if (itr->first == category_name[i]) {
					continue;
				}
				tem_Samples.push_back(itr->second);
				Mat response(itr->second.rows, 1, CV_32SC1, Scalar::all(-1));
				responses.push_back(response);
			}
			cout << responses << endl;
			stor_svms->train(tem_Samples, ROW_SAMPLE, responses);//?有问题
			//存储svm
			string svm_filename = string(DATA_FOLDER) + "/" + category_name[i] + string("_SVM.xml");
			stor_svms->save(svm_filename.c_str());
		}
		cout << "分类器训练完毕..." << endl;
	}
}
//对测试图片进行分类
void category_by_svm()
{
	cout << "物体分类开始..." << endl;
	Mat gray_pic;
	Mat threshold_image;
	string prediction_category;
	float curConfidence;
	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(TEST_FOLDER).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				continue;
			}
			//获取该目录下的图片名
			string train_pic_name = fileinfo.name;
			string train_pic_path = string(TEST_FOLDER) + string("/") + train_pic_name;

			//读取图片
			cout << train_pic_path << endl;
			Mat input_pic = imread(train_pic_path);
			imshow("输入图片：", input_pic);
			cvtColor(input_pic, gray_pic, CV_BGR2GRAY);

			// 提取BOW描述子
			vector<KeyPoint>kp;
			Mat test;
			featureDetector->detect(gray_pic, kp);
			bowDescriptorExtractor->compute(gray_pic, kp, test);

			int sign = 0;
			float best_score = -2.0f;

			for (int i = 0; i<category_size; i++)
			{
				prediction_category = "未知类型";
				string cate_na = category_name[i];
				string f_path = string(DATA_FOLDER) + "/" + cate_na + string("_SVM.xml");
				FileStorage svm_fs(f_path, FileStorage::READ);
				//读取SVM.xml
				if (svm_fs.isOpened())
				{
					svm_fs.release();
					//SVM st_svm;
					string svm_xml_path = f_path.c_str();
					Ptr<SVM> st_svm = StatModel::load<SVM>(svm_xml_path);
					if (sign == 0)
					{
						Mat outmat;
						float score_Value = st_svm->predict(test);
						float class_Value = st_svm->predict(test);
						//sign = (score_Value < 0.0f) == (class_Value < 0.0f) ? 1 : -1;
						sign = (class_Value > 0.0f) ? 1 : -1;
						if (sign == 1) {
							prediction_category = cate_na;
							break;
						}
						sign = 0;
					}
				}
			}
			//文件句柄  
			long   hFile1 = 0;
			//文件信息  
			struct _finddata_t fileinfo1;
			string p1;
			if ((hFile1 = _findfirst(p1.assign(RESULT_FOLDER).append("\\*").c_str(), &fileinfo1)) != -1)
			{
				do
				{
					if (fileinfo1.name == prediction_category)
					{
						string filename1 = string(RESULT_FOLDER) + "/" + prediction_category + string("/") + train_pic_name;
						//imwrite(filename1, input_pic);
					}
				} while (_findnext(hFile1, &fileinfo1) == 0);
				_findclose(hFile1);
			}
			//显示输出
			cout << "这张图属于： " << prediction_category << endl;
			waitKey(0);

		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}
//加载某一目录下的所有图片
void GetAllImg(string categor)
{
	string p;
	string path = p.assign(TRAIN_FOLDER).append("/").append(categor);
	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	if ((hFile = _findfirst((path + "\\*").c_str(), &fileinfo)) != -1)
	{
		//添加类别名到类别目录
		category_name.push_back(categor);
		do
		{
			//如果是目录,迭代之,如果不是,加入列表
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				string str = fileinfo.name;
			}
			else
			{
				// 读取文件夹下的文件。level 1表示这是一副训练图，通过multimap容器来建立由类目名称到训练图的一对多的映射
				string filename = p.assign(path).append("/").append(fileinfo.name);
				Mat temp = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
				pair<string, Mat> p(categor, temp);
				//得到训练集
				train_set.insert(p);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}