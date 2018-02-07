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

using namespace std;
using namespace cv;
using namespace cv::ml;
using namespace cv::xfeatures2d;

//����ѵ������
void categorizer::make_train_set()
{
	cout << "��ȡѵ����..." << endl;
	string categor;
	#pragma region ����
	//�ݹ����rescursive ֱ�Ӷ���������������iΪ������㣨�в�������end_iter�����յ�
	//for (recursive_directory_iterator i(TRAIN_FOLDER), end_iter; i != end_iter; i++)
	//{
	//	// level == 0��ΪĿ¼����ΪTRAIN__FOLDER���������
	//	if (i.level() == 0)
	//	{
	//		// ����Ŀ��������ΪĿ¼������
	//		categor = (i->path()).filename().string();
	//		category_name.push_back(categor);
	//	}
	//	else
	//	{
	//		// ��ȡ�ļ����µ��ļ���level 1��ʾ����һ��ѵ��ͼ��ͨ��multimap��������������Ŀ���Ƶ�ѵ��ͼ��һ�Զ��ӳ��
	//		string filename = string(TRAIN_FOLDER) + categor + string("/") + (i->path()).filename().string();
	//		Mat temp = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
	//		pair<string, Mat> p(categor, temp);
	//		//�õ�ѵ����
	//		train_set.insert(p);
	//	}
	//}  
	#pragma endregion

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
				categor = p.assign(TRAIN_FOLDER).append("\\").append(fileinfo.name);
				category_name.push_back(categor);
			}
			else
			{
				// ��ȡ�ļ����µ��ļ���level 1��ʾ����һ��ѵ��ͼ��ͨ��multimap��������������Ŀ���Ƶ�ѵ��ͼ��һ�Զ��ӳ��
				string filename = p.assign(TRAIN_FOLDER).append("\\").append(fileinfo.name);
				Mat temp = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
				pair<string, Mat> p(categor, temp);

				//�õ�ѵ����
				train_set.insert(p);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	category_size = category_name.size();
	cout << "���� " << category_size << "���������..." << endl;
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
	//featureDetector = SurfFeatureDetector::create();
	featureDetector= SURF::create(800);
	descriptorExtractor = SurfDescriptorExtractor::create();

	bowtrainer = new BOWKMeansTrainer(clusters);
	descriptorMatcher = new FlannBasedMatcher();
	bowDescriptorExtractor = new BOWImgDescriptorExtractor(descriptorExtractor, descriptorMatcher);

	#pragma region �ɱ�������
	//boost���ļ� ���������ļ���  directory_iterator(p)���ǵ���������㣬
	//�޲�����directory_iterator()���ǵ��������յ㡣
	//directory_iterator begin_iter(TEMPLATE_FOLDER);
	//directory_iterator end_iter;
	////��ȡ��Ŀ¼�µ������ļ���
	//for (; begin_iter != end_iter; ++begin_iter)
	//{
	//	string filename = string(TEMPLATE_FOLDER) + begin_iter->path().filename().string();
	//	string sub_category = remove_extention(begin_iter->path().filename().string());
	//	//����ģ��ͼƬ
	//	Mat image = imread(filename);
	//	Mat templ_image;
	//	//�洢ԭͼģ��
	//	result_objects[sub_category] = image;
	//}  
	#pragma endregion

	//���������ļ���  directory_iterator(p)���ǵ���������㣬
	//�ļ����  
	long   hFile = 0;
	//�ļ���Ϣ  
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
				string filename = p.assign(TEMPLATE_FOLDER).append("\\").append(fileinfo.name);
				string sub_category = remove_extention(fileinfo.name);
				//����ģ��ͼƬ
				Mat image = imread(filename);
				Mat templ_image;
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
//����bag of words
void categorizer::compute_bow_image()
{
	cout << "����bag of words..." << endl;
	FileStorage va_fs(DATA_FOLDER "vocab.xml", FileStorage::READ);
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
	string bow_path = string(DATA_FOLDER) + string("bow.txt");
	ifstream read_file(bow_path);

	//��BOW�Ѿ����ڣ�����Ҫ����
	if (!read_file)
	{
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
			//push_back(Mat);��ԭ����Mat�����һ�к��ټӼ���,Ԫ��ΪMatʱ�� �����ͺ��е���Ŀ ����;�����������ͬ��
			allsample_bow[cate_nam].push_back(imageDescriptor);
		}
		//�����һ���ı���Ϊ�����ж���׼��
		ofstream ous(bow_path);
		ous << "flag";
		cout << "bag of words�������..." << endl;
	}
}
//ѵ��������
void categorizer::trainSvm()
{
	int flag = 0;
	for (int k = 0; k<category_size; k++)
	{
		string svm_file_path = string(DATA_FOLDER) + category_name[k] + string("SVM.xml");
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
		stor_svms->setKernel(SVM::POLY);//CvSVM::LINEAR
		stor_svms->setDegree(1.0);
		stor_svms->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));
		//����ѵ������
		//SVMParams svmParams;
		//svmParams.svm_type = CvSVM::C_SVC;
		//svmParams.kernel_type = CvSVM::LINEAR;
		//svmParams.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);

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
			//stor_svms[i].train(tem_Samples, responses, Mat(), Mat(), svmParams);
			stor_svms->train(tem_Samples, ROW_SAMPLE, responses);//?������
			//�洢svm
			string svm_filename = string(DATA_FOLDER) + category_name[i] + string("SVM.xml");
			stor_svms[i].save(svm_filename.c_str());
		}
		cout << "������ѵ�����..." << endl;
	}
}
//�Բ���ͼƬ���з���
void categorizer::category_by_svm()
{
	cout << "������࿪ʼ..." << endl;
	Mat gray_pic;
	Mat threshold_image;
	string prediction_category;
	float curConfidence;

	//directory_iterator begin_train(TEST_FOLDER);
	//directory_iterator end_train;
	//for (; begin_train != end_train; ++begin_train)
	//{
	//	//��ȡ��Ŀ¼�µ�ͼƬ��
	//	string train_pic_name = (begin_train->path()).filename().string();
	//	string train_pic_path = string(TEST_FOLDER) + string("/") + (begin_train->path()).filename().string();
	//	//��ȡͼƬ
	//	cout << train_pic_path << endl;
	//	Mat input_pic = imread(train_pic_path);
	//	imshow("����ͼƬ��", input_pic);
	//	cvtColor(input_pic, gray_pic, CV_BGR2GRAY);
	//	// ��ȡBOW������
	//	vector<KeyPoint>kp;
	//	Mat test;
	//	featureDetector->detect(gray_pic, kp);
	//	bowDescriptorExtractor->compute(gray_pic, kp, test);
	//	int sign = 0;
	//	float best_score = -2.0f;
	//	for (int i = 0; i<category_size; i++)
	//	{
	//		string cate_na = category_name[i];
	//		string f_path = string(DATA_FOLDER) + cate_na + string("SVM.xml");
	//		FileStorage svm_fs(f_path, FileStorage::READ);
	//		//��ȡSVM.xml
	//		if (svm_fs.isOpened())
	//		{
	//			svm_fs.release();
	//			//SVM st_svm;
	//			string svm_xml_path = f_path.c_str();
	//			Ptr<SVM> st_svm = StatModel::load<SVM>(svm_xml_path);
	//			//st_svm.load(f_path.c_str());
	//			if (sign == 0)
	//			{
	//				//float score_Value = st_svm.predict(test, true);
	//				//float class_Value = st_svm.predict(test, false);
	//				float score_Value = st_svm->predict(test);
	//				float class_Value = st_svm->predict(test);
	//				sign = (score_Value < 0.0f) == (class_Value < 0.0f) ? 1 : -1;
	//			}
	//			//curConfidence = sign * st_svm.predict(test, true);
	//			curConfidence = sign * st_svm->predict(test);
	//		}
	//		else
	//		{
	//			if (sign == 0)
	//			{
	//				//float scoreValue = stor_svms[i].predict(test, true);
	//				//float classValue = stor_svms[i].predict(test, false);
	//				float scoreValue = stor_svms[i].predict(test);
	//				float classValue = stor_svms[i].predict(test);
	//				sign = (scoreValue < 0.0f) == (classValue < 0.0f) ? 1 : -1;
	//			}
	//			//curConfidence = sign * stor_svms[i].predict(test, true);
	//			curConfidence = sign * stor_svms[i].predict(test);
	//		}
	//		if (curConfidence>best_score)
	//		{
	//			best_score = curConfidence;
	//			prediction_category = cate_na;
	//		}
	//	}
	//	//��ͼƬд����Ӧ���ļ�����
	//	directory_iterator begin_iterater(RESULT_FOLDER);
	//	directory_iterator end_iterator;
	//	//��ȡ��Ŀ¼�µ��ļ���
	//	for (; begin_iterater != end_iterator; ++begin_iterater)
	//	{
	//		if (begin_iterater->path().filename().string() == prediction_category)
	//		{
	//			string filename = string(RESULT_FOLDER) + prediction_category + string("/") + train_pic_name;
	//			imwrite(filename, input_pic);
	//		}
	//	}
	//	//��ʾ���
	//	namedWindow("Dectect Object");
	//	cout << "����ͼ���ڣ� " << prediction_category << endl;
	//	imshow("Dectect Object", result_objects[prediction_category]);
	//	waitKey(0);
	//}

	//�ļ����  
	long   hFile = 0;
	//�ļ���Ϣ  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(TEST_FOLDER).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
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

				string cate_na = category_name[i];

				string f_path = string(DATA_FOLDER) + cate_na + string("SVM.xml");
				FileStorage svm_fs(f_path, FileStorage::READ);
				//��ȡSVM.xml
				if (svm_fs.isOpened())
				{

					svm_fs.release();
					//SVM st_svm;
					string svm_xml_path = f_path.c_str();
					Ptr<SVM> st_svm = StatModel::load<SVM>(svm_xml_path);
					//st_svm.load(f_path.c_str());

					if (sign == 0)
					{
						//float score_Value = st_svm.predict(test, true);
						//float class_Value = st_svm.predict(test, false);
						float score_Value = st_svm->predict(test);
						float class_Value = st_svm->predict(test);
						sign = (score_Value < 0.0f) == (class_Value < 0.0f) ? 1 : -1;
					}
					//curConfidence = sign * st_svm.predict(test, true);
					curConfidence = sign * st_svm->predict(test);
				}
				else
				{
					if (sign == 0)
					{
						//float scoreValue = stor_svms[i].predict(test, true);
						//float classValue = stor_svms[i].predict(test, false);
						float scoreValue = stor_svms[i].predict(test);
						float classValue = stor_svms[i].predict(test);
						sign = (scoreValue < 0.0f) == (classValue < 0.0f) ? 1 : -1;
					}
					//curConfidence = sign * stor_svms[i].predict(test, true);
					curConfidence = sign * stor_svms[i].predict(test);

				}

				if (curConfidence>best_score)
				{
					best_score = curConfidence;
					prediction_category = cate_na;
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
						string filename1 = string(RESULT_FOLDER) + prediction_category + string("/") + train_pic_name;
						imwrite(filename1, input_pic);
					}
				} while (_findnext(hFile1, &fileinfo1) == 0);
				_findclose(hFile1);
			}
			//��ʾ���
			namedWindow("Dectect Object");
			cout << "����ͼ���ڣ� " << prediction_category << endl;
			imshow("Dectect Object", result_objects[prediction_category]);
			waitKey(0);

		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}
