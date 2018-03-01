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

int main()
{
	int result = 0;
	char * filePath = "E:/Github/projectdata/data/test_image";
	vector<string> files;
	getFiles(filePath, files);
	int number = files.size();
	cout << "ѵ����������" << number << endl;
	string modelpath = "svm.xml";
	//Ptr<SVM> svm = SVM::create();
	Ptr<SVM> svm = SVM::load(modelpath.c_str());
	//svm->clear();
	FileStorage svm_fs(modelpath, FileStorage::READ);
	if (svm_fs.isOpened())
	{
		//svm->load(modelpath.c_str());
	}
	for (int i = 0; i < number; i++)
	{
		Mat inMat = imread(files[i].c_str());
		Mat gray;
		cvtColor(inMat, gray, CV_BGR2GRAY);
		//����
		Size dsize = Size(200, 200);
		Mat image2 = Mat(dsize, CV_32SC1);
		resize(gray, image2, dsize);
		image2 = image2.reshape(1, 1);
		image2.convertTo(image2, CV_32FC1);
		int response = (int)svm->predict(image2);
		cout << "ѵ��ͼƬ" << files[i].c_str() << "ѵ�����" << response << endl;
		if (response == 1)
		{
			result++;
		}
	}
	cout << result << endl;
	getchar();
	return  0;
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