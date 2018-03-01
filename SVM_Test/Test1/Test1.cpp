// Test1.cpp : 定义控制台应用程序的入口点。
//实现SVM-线性可分二分类问题
#include "stdafx.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/ml.hpp> 
using namespace cv;
using namespace cv::ml;
int main()
{
	// Data for visual representation  
	int width = 512, height = 512;
	Mat image = Mat::zeros(height, width, CV_8UC3);

	// Set up training data  
	//float labels[4] = { 1.0, -1.0, -1.0, -1.0}; //样本数据  
	//Mat labelsMat(4, 1, CV_32SC1, labels);     //样本标签  
	//float trainingData[4][2] = { { 501, 300 },{ 255, 10 },{ 501, 255 },{ 10, 501 }}; //Mat结构特征数据  
	//Mat trainingDataMat(4, 2, CV_32FC1, trainingData);   //Mat结构标签  

	float labels[5] = { 1.0, -1.0, -1.0, -1.0,1.0 }; //样本数据  
	Mat labelsMat(5, 1, CV_32SC1, labels);     //样本标签  
	float trainingData[5][2] = { { 501, 300 },{ 255, 10 },{ 501, 255 },{ 10, 501 },{ 450,500 } }; //Mat结构特征数据  
	Mat trainingDataMat(5, 2, CV_32FC1, trainingData);   //Mat结构标签 
	// Train the SVM  
	//CvSVM SVM;
	Ptr<SVM> SVM = SVM::create();
	// Set up SVM's parameters 
	SVM->setType(SVM::C_SVC);
	SVM->setKernel(SVM::LINEAR);//LINEAR 
	SVM->setDegree(1.0);
	//SVM->setTermCriteria(TermCriteria(CV_TERMCRIT_ITER, 100, 1e-6));
	SVM->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));
	SVM->train(trainingDataMat, ROW_SAMPLE, labelsMat);

	Vec3b green(0, 255, 0), blue(255, 0, 0);
	// Show the decision regions given by the SVM  
	for (int i = 0; i < image.rows; ++i)
	{
		for (int j = 0; j < image.cols; ++j)
		{
			Mat sampleMat = (Mat_<float>(1, 2) << i, j);
			float response = SVM->predict(sampleMat);
			if (response > 1)
				image.at<Vec3b>(j, i) = green;
			else
				image.at<Vec3b>(j, i) = blue;
		}
	}
	imshow("样本区域分割", image);
	waitKey();

	// Show the training data  
	int thickness = -1;
	int lineType = 8;
	circle(image, Point(501, 300), 5, Scalar(0, 100, 0), thickness, lineType);
	circle(image, Point(255, 10), 5, Scalar(255, 100, 255), thickness, lineType);
	circle(image, Point(501, 255), 5, Scalar(100, 255, 255), thickness, lineType);
	circle(image, Point(10, 501), 5, Scalar(255, 200, 255), thickness, lineType);
	circle(image, Point(450, 500), 5, Scalar(0, 200, 0), thickness, lineType);

	// Show support vectors  
	thickness = 2;
	lineType = 8;
	Mat sv = SVM->getSupportVectors();
	int c = sv.rows;
	//int c = SVM.get_support_vector_count();
	for (int i = 0; i < c; ++i)
	{
		//const float* v = SVM.get_support_vector(i);
		const float* v = sv.ptr<float>(i);
		circle(image, Point((int)v[0], (int)v[1]), 6, Scalar(0, 0, 255), thickness, lineType);
	}
	imwrite("result.png", image);// save the image   

	imshow("SVM Simple Example", image); // show it to the user  
	waitKey(0);
}

