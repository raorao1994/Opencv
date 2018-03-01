// Test4-pic.cpp : �������̨Ӧ�ó������ڵ㡣
//SVMģ��ѵ��������OpenCVʵ�� -ͼƬ����
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

void getFiles(string path, vector<string>& files);
void getBubble(Mat& trainingImages, vector<int>& trainingLabels);
void getNoBubble(Mat& trainingImages, vector<int>& trainingLabels);

int main()
{
	//��ȡѵ������
	Mat classes;
	Mat trainingData;
	Mat trainingImages;
	vector<int> trainingLabels;
	getBubble(trainingImages, trainingLabels);
	getNoBubble(trainingImages, trainingLabels);
	Mat(trainingImages).copyTo(trainingData);
	trainingData.convertTo(trainingData, CV_32FC1);
	Mat(trainingLabels).copyTo(classes);
	//����SVMѵ��������
	Ptr<SVM> svm =SVM::create();
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::LINEAR);
	svm->setType(SVM::C_SVC);
	svm->setDegree(0);
	svm->setGamma(1);
	svm->setCoef0(0);
	svm->setC(1);
	svm->setNu(0);
	svm->setP(0);
	svm->setTermCriteria(TermCriteria(CV_TERMCRIT_ITER, 1000, 0.01));
	/*CvSVMParams SVM_params;
	SVM_params.svm_type = CvSVM::C_SVC;
	SVM_params.kernel_type = CvSVM::LINEAR;
	SVM_params.degree = 0;
	SVM_params.gamma = 1;
	SVM_params.coef0 = 0;
	SVM_params.C = 1;
	SVM_params.nu = 0;
	SVM_params.p = 0;
	SVM_params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 1000, 0.01);*/

	//ѵ��
	svm->train(trainingData, ROW_SAMPLE, classes);
	//����ģ��
	svm->save("svm.xml");
	cout << "ѵ�����ˣ�����" << endl;
	getchar();
	return 0;
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
void getBubble(Mat& trainingImages, vector<int>& trainingLabels)
{
	string filePath = "E:/Github/projectdata/data/train_images/airplanes";
	vector<string> files;
	getFiles(filePath, files);
	int number = files.size();
	for (int i = 0; i < number; i++)
	{
		string imgpath = files[i].c_str();
		Mat  SrcImage = imread(imgpath);
		//����
		Size dsize = Size(200, 200);
		Mat image2 = Mat(dsize, CV_32S);
		resize(SrcImage, image2, dsize);
		image2 = image2.reshape(1, 1);
		trainingImages.push_back(image2);
		trainingLabels.push_back(1);
	}
}
void getNoBubble(Mat& trainingImages, vector<int>& trainingLabels)
{
	string filePath = "E:/Github/projectdata/data/train_images/butterfly";
	vector<string> files;
	getFiles(filePath, files);
	int number = files.size();
	for (int i = 0; i < number; i++)
	{
		string imgpath = files[i].c_str();
		Mat  SrcImage = imread(imgpath);
		//����
		Size dsize = Size(200, 200);
		Mat image2 = Mat(dsize, CV_32S);
		resize(SrcImage, image2, dsize);
		image2 = image2.reshape(1, 1);
		trainingImages.push_back(image2);
		trainingLabels.push_back(1);
	}
}