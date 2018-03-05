// Test4-pic.cpp : 定义控制台应用程序的入口点。
//SVM模型训练与分类的OpenCV实现 -图片分类
#include "stdafx.h"
#include <time.h>  
#include <opencv2/opencv.hpp>  
#include <opencv/cv.h>  
#include <iostream> 
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/ml/ml.hpp>  
#include <io.h>
#include <map>
#include <fstream>

using namespace std;
using namespace cv;
using namespace cv::ml;

void getClass(string path, Mat& trainingImages, vector<int>& trainingLabels);
void getFiles(string path, vector<string>& files);
void getData(string filePath, int index, Mat& trainingImages, vector<int>& trainingLabels);
int main()
{
	//获取训练数据
	Mat classes;
	Mat trainingData;
	Mat trainingImages;
	vector<int> trainingLabels;
	//getBubble(trainingImages, trainingLabels);
	//getNoBubble(trainingImages, trainingLabels);
	string basePath = "E:/SVN/OpenCV/trunk/Data/svm_data/train_images";
	basePath = "E:/TablewareRecognition/TablewareRecognition/data1";
	getClass(basePath, trainingImages, trainingLabels);

	Mat(trainingImages).copyTo(trainingData);
	trainingData.convertTo(trainingData, CV_32FC1);
	Mat(trainingLabels).copyTo(classes);
	//配置SVM训练器参数
	Ptr<SVM> svm = SVM::create();
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::LINEAR);//LINEAR
	svm->setDegree(0);
	svm->setGamma(1);
	svm->setCoef0(0);
	svm->setC(1);
	svm->setNu(0);
	svm->setP(0);
	//svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 1000, 0.01));
	svm->setTermCriteria(TermCriteria(TermCriteria::EPS, 100, 1e-6));
	//训练
	cout << "开始训练模型" << endl;
	svm->train(trainingData, ROW_SAMPLE, classes);
	//保存模型
	svm->save("svm.xml");
	cout << "训练好了！！！" << endl;
	getchar();
	return 0;
}
void getClass(string path, Mat& trainingImages, vector<int>& trainingLabels)
{
	int index = 1;
	intptr_t   hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	long i;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					cout << "开始加载" << fileinfo.name<<"类数据" << endl;
					getData(p.assign(path).append("\\").append(fileinfo.name), index, trainingImages, trainingLabels);
					index++;
				}	
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}
void getFiles(string path, vector<string>& files)
{
	intptr_t   hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	long i;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}
void getData(string filePath,int index,Mat& trainingImages, vector<int>& trainingLabels)
{
	vector<string> files;
	getFiles(filePath, files);
	int number = files.size();
	for (int i = 0; i < number; i++)
	{
		string imgpath = files[i].c_str();
		Mat  SrcImage = imread(imgpath);
		Mat gray;
		cvtColor(SrcImage, gray, CV_BGR2GRAY);
		//缩放
		Size dsize = Size(200, 200);
		Mat image2 = Mat(dsize, CV_32SC1);
		resize(SrcImage, image2, dsize);
		image2 = image2.reshape(1, 1);
		trainingImages.push_back(image2);
		trainingLabels.push_back(index);
	}
}