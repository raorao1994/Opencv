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

//构造训练集合
void categorizer::make_train_set()
{
	cout << "读取训练集..." << endl;
	string categor;
	#pragma region 迭代
	//递归迭代rescursive 直接定义两个迭代器：i为迭代起点（有参数），end_iter迭代终点
	//for (recursive_directory_iterator i(TRAIN_FOLDER), end_iter; i != end_iter; i++)
	//{
	//	// level == 0即为目录，因为TRAIN__FOLDER中设置如此
	//	if (i.level() == 0)
	//	{
	//		// 将类目名称设置为目录的名称
	//		categor = (i->path()).filename().string();
	//		category_name.push_back(categor);
	//	}
	//	else
	//	{
	//		// 读取文件夹下的文件。level 1表示这是一副训练图，通过multimap容器来建立由类目名称到训练图的一对多的映射
	//		string filename = string(TRAIN_FOLDER) + categor + string("/") + (i->path()).filename().string();
	//		Mat temp = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
	//		pair<string, Mat> p(categor, temp);
	//		//得到训练集
	//		train_set.insert(p);
	//	}
	//}  
	#pragma endregion

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
				categor = p.assign(TRAIN_FOLDER).append("\\").append(fileinfo.name);
				category_name.push_back(categor);
			}
			else
			{
				// 读取文件夹下的文件。level 1表示这是一副训练图，通过multimap容器来建立由类目名称到训练图的一对多的映射
				string filename = p.assign(TRAIN_FOLDER).append("\\").append(fileinfo.name);
				Mat temp = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
				pair<string, Mat> p(categor, temp);

				//得到训练集
				train_set.insert(p);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	category_size = category_name.size();
	cout << "发现 " << category_size << "种类别物体..." << endl;
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
	//featureDetector = SurfFeatureDetector::create();
	featureDetector= SURF::create(800);
	descriptorExtractor = SurfDescriptorExtractor::create();

	bowtrainer = new BOWKMeansTrainer(clusters);
	descriptorMatcher = new FlannBasedMatcher();
	bowDescriptorExtractor = new BOWImgDescriptorExtractor(descriptorExtractor, descriptorMatcher);

	#pragma region 旧便利代码
	//boost库文件 遍历数据文件夹  directory_iterator(p)就是迭代器的起点，
	//无参数的directory_iterator()就是迭代器的终点。
	//directory_iterator begin_iter(TEMPLATE_FOLDER);
	//directory_iterator end_iter;
	////获取该目录下的所有文件名
	//for (; begin_iter != end_iter; ++begin_iter)
	//{
	//	string filename = string(TEMPLATE_FOLDER) + begin_iter->path().filename().string();
	//	string sub_category = remove_extention(begin_iter->path().filename().string());
	//	//读入模板图片
	//	Mat image = imread(filename);
	//	Mat templ_image;
	//	//存储原图模板
	//	result_objects[sub_category] = image;
	//}  
	#pragma endregion

	//遍历数据文件夹  directory_iterator(p)就是迭代器的起点，
	//文件句柄  
	long   hFile = 0;
	//文件信息  
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
				string filename = p.assign(TEMPLATE_FOLDER).append("\\").append(fileinfo.name);
				string sub_category = remove_extention(fileinfo.name);
				//读入模板图片
				Mat image = imread(filename);
				Mat templ_image;
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
//构造bag of words
void categorizer::compute_bow_image()
{
	cout << "构造bag of words..." << endl;
	FileStorage va_fs(DATA_FOLDER "vocab.xml", FileStorage::READ);
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
	string bow_path = string(DATA_FOLDER) + string("bow.txt");
	ifstream read_file(bow_path);

	//如BOW已经存在，则不需要构造
	if (!read_file)
	{
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
			//push_back(Mat);在原来的Mat的最后一行后再加几行,元素为Mat时， 其类型和列的数目 必须和矩阵容器是相同的
			allsample_bow[cate_nam].push_back(imageDescriptor);
		}
		//简单输出一个文本，为后面判断做准备
		ofstream ous(bow_path);
		ous << "flag";
		cout << "bag of words构造完毕..." << endl;
	}
}
//训练分类器
void categorizer::trainSvm()
{
	int flag = 0;
	for (int k = 0; k<category_size; k++)
	{
		string svm_file_path = string(DATA_FOLDER) + category_name[k] + string("SVM.xml");
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
		stor_svms->setKernel(SVM::POLY);//CvSVM::LINEAR
		stor_svms->setDegree(1.0);
		stor_svms->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));
		//设置训练参数
		//SVMParams svmParams;
		//svmParams.svm_type = CvSVM::C_SVC;
		//svmParams.kernel_type = CvSVM::LINEAR;
		//svmParams.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);

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
			//stor_svms[i].train(tem_Samples, responses, Mat(), Mat(), svmParams);
			stor_svms->train(tem_Samples, ROW_SAMPLE, responses);//?有问题
			//存储svm
			string svm_filename = string(DATA_FOLDER) + category_name[i] + string("SVM.xml");
			stor_svms[i].save(svm_filename.c_str());
		}
		cout << "分类器训练完毕..." << endl;
	}
}
//对测试图片进行分类
void categorizer::category_by_svm()
{
	cout << "物体分类开始..." << endl;
	Mat gray_pic;
	Mat threshold_image;
	string prediction_category;
	float curConfidence;

	//directory_iterator begin_train(TEST_FOLDER);
	//directory_iterator end_train;
	//for (; begin_train != end_train; ++begin_train)
	//{
	//	//获取该目录下的图片名
	//	string train_pic_name = (begin_train->path()).filename().string();
	//	string train_pic_path = string(TEST_FOLDER) + string("/") + (begin_train->path()).filename().string();
	//	//读取图片
	//	cout << train_pic_path << endl;
	//	Mat input_pic = imread(train_pic_path);
	//	imshow("输入图片：", input_pic);
	//	cvtColor(input_pic, gray_pic, CV_BGR2GRAY);
	//	// 提取BOW描述子
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
	//		//读取SVM.xml
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
	//	//将图片写入相应的文件夹下
	//	directory_iterator begin_iterater(RESULT_FOLDER);
	//	directory_iterator end_iterator;
	//	//获取该目录下的文件名
	//	for (; begin_iterater != end_iterator; ++begin_iterater)
	//	{
	//		if (begin_iterater->path().filename().string() == prediction_category)
	//		{
	//			string filename = string(RESULT_FOLDER) + prediction_category + string("/") + train_pic_name;
	//			imwrite(filename, input_pic);
	//		}
	//	}
	//	//显示输出
	//	namedWindow("Dectect Object");
	//	cout << "这张图属于： " << prediction_category << endl;
	//	imshow("Dectect Object", result_objects[prediction_category]);
	//	waitKey(0);
	//}

	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(TEST_FOLDER).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
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

				string cate_na = category_name[i];

				string f_path = string(DATA_FOLDER) + cate_na + string("SVM.xml");
				FileStorage svm_fs(f_path, FileStorage::READ);
				//读取SVM.xml
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
						string filename1 = string(RESULT_FOLDER) + prediction_category + string("/") + train_pic_name;
						imwrite(filename1, input_pic);
					}
				} while (_findnext(hFile1, &fileinfo1) == 0);
				_findclose(hFile1);
			}
			//显示输出
			namedWindow("Dectect Object");
			cout << "这张图属于： " << prediction_category << endl;
			imshow("Dectect Object", result_objects[prediction_category]);
			waitKey(0);

		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}
