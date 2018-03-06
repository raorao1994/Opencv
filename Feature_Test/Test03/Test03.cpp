// Test03.cpp : �������̨Ӧ�ó������ڵ㡣
//SVM������ͼ�����

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

//����Ŀ���Ƶ����ݵ�mapӳ��
map<string, Mat> result_objects;
//�������ѵ��ͼ����ӳ��
map<string, Mat> allsample_bow;
//����Ŀ���Ƶ�ѵ��ͼ����ӳ�䣬�ؼ��ֿ����ظ�����
multimap<string, Mat> train_set;
//ѵ���ĵõ���SVM
Ptr<SVM> stor_svms;
//��Ŀ���ƣ�Ҳ����train_folder���õ�Ŀ¼��
vector<string> category_name;
//��Ŀ��Ŀ
int category_size;
//��SURF���������Ӿ��ʻ�ľ�����Ŀ
int clusters;
//���ѵ����ͼƬ�ʵ�
Mat vocab;

//���������detectors����������ȡ��extractors���;����Ptr
Ptr<SurfFeatureDetector> featureDetector;
Ptr<DescriptorExtractor> descriptorExtractor;

Ptr<BOWKMeansTrainer> bowtrainer;
Ptr<BOWImgDescriptorExtractor> bowDescriptorExtractor;
Ptr<FlannBasedMatcher> descriptorMatcher;

//���庯��
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

//����ѵ������
void make_train_set()
{
	cout << "��ȡѵ����..." << endl;
	string categor;
	//�ݹ����rescursive ֱ�Ӷ���������������iΪ������㣨�в�������end_iter�����յ�
	//�ļ����  
	long   hFile = 0;
	//�ļ���Ϣ  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(TRAIN_FOLDER).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�����Ŀ¼,����֮  
			//�������,�����б�  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				// ����Ŀ��������ΪĿ¼������
				//categor = p.assign(TRAIN_FOLDER).append("/").append(fileinfo.name);
				//�������������Ŀ¼
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
	cout << "���� " << category_size << "���������..." << endl;
}
//�Ƴ���չ����������ģ����֯����Ŀ
string remove_extention(string filename)
{
	int lastIndex = filename.find_last_of(".");
	string name = filename.substr(0, lastIndex);
	return name;
}
//���캯��
void categorizer(int _clusters)
{
	cout << "��ʼ��ʼ��...." << endl;
	clusters = _clusters;
	//��ʼ��ָ��
	featureDetector = SurfFeatureDetector::create(800);
	descriptorExtractor = SurfDescriptorExtractor::create();

	bowtrainer = new BOWKMeansTrainer(clusters);
	descriptorMatcher = new FlannBasedMatcher();
	bowDescriptorExtractor = new BOWImgDescriptorExtractor(descriptorExtractor, descriptorMatcher);

	//���������ļ���  directory_iterator(p)���ǵ���������㣬
	//�ļ����  
	long   hFile = 0;
	//�ļ���Ϣ 
	//��Ӳ�������
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(TEMPLATE_FOLDER).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�����Ŀ¼,����֮  
			//�������,�����б�  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
			}
			else
			{
				string filename = p.assign(TEMPLATE_FOLDER).append(fileinfo.name);
				string sub_category = remove_extention(fileinfo.name);
				//����ģ��ͼƬ
				Mat image = imread(filename);
				//�洢ԭͼģ��
				result_objects[sub_category] = image;
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	cout << "��ʼ�����..." << endl;
	//��ȡѵ����
	make_train_set();
}
// ѵ��ͼƬfeature���࣬�ó��ʵ�
void build_vacab()
{
	FileStorage vacab_fs(DATA_FOLDER "/vocab.xml", FileStorage::READ);
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
			//push_back(Mat);��ԭ����Mat�����һ�к��ټӼ���,Ԫ��ΪMatʱ,
			//�����ͺ��е���Ŀ ����;�����������ͬ��
			vocab_descriptors.push_back(descrip);
		}
		cout << "ѵ��ͼƬ��ʼ����..." << endl;
		//��ÿһ��ͼ��Surf��������add�������뵽bowTraining��ȥ,�Ϳ��Խ��о���ѵ����
		bowtrainer->add(vocab_descriptors);
		// ��SURF�����ӽ��о���
		vocab = bowtrainer->cluster();
		cout << "������ϣ��ó��ʵ�..." << endl;

		//���ļ���ʽ����ʵ�
		FileStorage file_stor(DATA_FOLDER "/vocab.xml", FileStorage::WRITE);
		file_stor << "vocabulary" << vocab;
		file_stor.release();
	}
}
//����bag of words
void compute_bow_image()
{
	cout << "����bag of words..." << endl;
	FileStorage va_fs(DATA_FOLDER "/vocab.xml", FileStorage::READ);
	//����ʵ������ֱ�Ӷ�ȡ
	if (va_fs.isOpened())
	{
		Mat temp_vacab;
		va_fs["vocabulary"] >> temp_vacab;
		bowDescriptorExtractor->setVocabulary(temp_vacab);
		va_fs.release();
	}
	else
	{
		//��ÿ��ͼƬ�������㣬ͳ������ͼƬ���������ֵ�Ƶ�ʣ���Ϊ����ͼƬ��bag of words
		bowDescriptorExtractor->setVocabulary(vocab);
	}
	//���bow.txt�Ѿ�����˵��֮ǰ�Ѿ�ѵ�����ˣ�����Ͳ������¹���BOW
	string bow_path = string(DATA_FOLDER) + string("/bow.txt");
	ifstream read_file(bow_path);
	//��BOW�Ѿ����ڣ�����Ҫ����
	if (read_file && !read_file)
	{
		//��ȡallsample_bow
		/*FileStorage allsimple_data(DATA_FOLDER "/allsample_bow.xml", FileStorage::READ);
		allsimple_data["allsample_bow"] >> allsample_bow;
		allsimple_data.release();*/
		cout << "BOW �Ѿ�׼����..." << endl;
	}
	else {
		// ����ÿһ��ģ�壬��ȡSURF���ӣ����뵽vocab_descriptors��
		multimap<string, Mat> ::iterator i = train_set.begin();
		for (; i != train_set.end(); i++)
		{
			vector<KeyPoint>kp;
			string cate_nam = (*i).first;
			Mat tem_image = (*i).second;
			Mat imageDescriptor;
			featureDetector->detect(tem_image, kp);
			bowDescriptorExtractor->compute(tem_image, kp, imageDescriptor);
			//push_back(Mat);��ԭ����Mat�����һ�к��ټӼ���,Ԫ��ΪMatʱ��
			//�����ͺ��е���Ŀ ����;�����������ͬ��
			allsample_bow[cate_nam].push_back(imageDescriptor);
		}
		//����allsample_bow
		/*FileStorage samplefile_stor(DATA_FOLDER "/allsample_bow.xml", FileStorage::WRITE);
		if (samplefile_stor.isOpened())
		{
		samplefile_stor << "allsample_bow" << allsample_bow;
		samplefile_stor.release();
		}*/
		//�����һ���ı���Ϊ�����ж���׼��
		ofstream ous(bow_path);
		ous << "flag";
		cout << "bag of words�������..." << endl;
	}
}
//ѵ��������
void trainSvm()
{
	int flag = 0;
	for (int k = 0; k<category_size; k++)
	{
		string svm_file_path = string(DATA_FOLDER) + "/" + category_name[k] + string("_SVM.xml");
		FileStorage svm_fil(svm_file_path, FileStorage::READ);
		//�ж�ѵ������Ƿ����
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
	//���ѵ������Ѿ���������Ҫ����ѵ��
	if (flag != -1)
	{
		cout << "�������Ѿ�ѵ�����..." << endl;
	}
	else
	{
		stor_svms = SVM::create();
		//����ѵ������
		stor_svms->setType(SVM::C_SVC);
		stor_svms->setKernel(SVM::RBF);//CvSVM::LINEAR POLY  RBF
		stor_svms->setDegree(1.0);
		//stor_svms->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));
		stor_svms->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 1000, FLT_EPSILON));
		cout << "ѵ��������..." << endl;
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
			stor_svms->train(tem_Samples, ROW_SAMPLE, responses);//?������
			//�洢svm
			string svm_filename = string(DATA_FOLDER) + "/" + category_name[i] + string("_SVM.xml");
			stor_svms->save(svm_filename.c_str());
		}
		cout << "������ѵ�����..." << endl;
	}
}
//�Բ���ͼƬ���з���
void category_by_svm()
{
	cout << "������࿪ʼ..." << endl;
	Mat gray_pic;
	Mat threshold_image;
	string prediction_category;
	float curConfidence;
	//�ļ����  
	long   hFile = 0;
	//�ļ���Ϣ  
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
			//��ȡ��Ŀ¼�µ�ͼƬ��
			string train_pic_name = fileinfo.name;
			string train_pic_path = string(TEST_FOLDER) + string("/") + train_pic_name;

			//��ȡͼƬ
			cout << train_pic_path << endl;
			Mat input_pic = imread(train_pic_path);
			imshow("����ͼƬ��", input_pic);
			cvtColor(input_pic, gray_pic, CV_BGR2GRAY);

			// ��ȡBOW������
			vector<KeyPoint>kp;
			Mat test;
			featureDetector->detect(gray_pic, kp);
			bowDescriptorExtractor->compute(gray_pic, kp, test);

			int sign = 0;
			float best_score = -2.0f;

			for (int i = 0; i<category_size; i++)
			{
				prediction_category = "δ֪����";
				string cate_na = category_name[i];
				string f_path = string(DATA_FOLDER) + "/" + cate_na + string("_SVM.xml");
				FileStorage svm_fs(f_path, FileStorage::READ);
				//��ȡSVM.xml
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
			//�ļ����  
			long   hFile1 = 0;
			//�ļ���Ϣ  
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
			//��ʾ���
			cout << "����ͼ���ڣ� " << prediction_category << endl;
			waitKey(0);

		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}
//����ĳһĿ¼�µ�����ͼƬ
void GetAllImg(string categor)
{
	string p;
	string path = p.assign(TRAIN_FOLDER).append("/").append(categor);
	//�ļ����  
	long   hFile = 0;
	//�ļ���Ϣ  
	struct _finddata_t fileinfo;
	if ((hFile = _findfirst((path + "\\*").c_str(), &fileinfo)) != -1)
	{
		//�������������Ŀ¼
		category_name.push_back(categor);
		do
		{
			//�����Ŀ¼,����֮,�������,�����б�
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				string str = fileinfo.name;
			}
			else
			{
				// ��ȡ�ļ����µ��ļ���level 1��ʾ����һ��ѵ��ͼ��ͨ��multimap��������������Ŀ���Ƶ�ѵ��ͼ��һ�Զ��ӳ��
				string filename = p.assign(path).append("/").append(fileinfo.name);
				Mat temp = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
				pair<string, Mat> p(categor, temp);
				//�õ�ѵ����
				train_set.insert(p);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}