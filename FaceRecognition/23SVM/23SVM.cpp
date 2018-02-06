// 23SVM.cpp : 定义控制台应用程序的入口点。
// OpenCV--SVM多分类问题
#include "stdafx.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/ml.hpp>

using namespace cv;
using namespace cv::ml;

Vec3b getRandomColor() {
	RNG rng;
	return Vec3b(rng.next() % 255, rng.next() % 255, rng.next() % 255);
}

int main()
{
	// Data for visual representation
	int width = 512, height = 512;
	Mat image = Mat::zeros(height, width, CV_8UC3);

	// Set up training data
	int labels[4] = { 1, 2, 3, 4 };
	float trainingData[4][2] = { { 100, 10 },{ 10, 500 },{ 500, 10 },{ 500, 500 } };

	Mat trainingDataMat(4, 2, CV_32FC1, trainingData);
	Mat labelsMat(4, 1, CV_32SC1, labels);

	// Train the SVM
	//! [init]
	Ptr<SVM> svm = SVM::create();
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::POLY);
	svm->setDegree(1.0);
	svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));
	//! [init]
	//! [train]
	//    svm->train(trainingDataMat, ROW_SAMPLE, labelsMat);
	//    Ptr<TrainData> auto_train_data = TrainData::create(trainingDataMat, ROW_SAMPLE, labelsMat);
	//    svm->trainAuto(auto_train_data);
	svm->train(trainingDataMat, ROW_SAMPLE, labelsMat);
	//! [train]

	// Show the decision regions given by the SVM
	//! [show]
	Vec3b green(0, 255, 0), blue(255, 0, 0), red(0, 0, 255), yellow(0, 255, 255);

	for (int i = 0; i < image.rows; ++i) {
		for (int j = 0; j < image.cols; ++j) {
			Mat sampleMat = (Mat_<float>(1, 2) << j, i);
			float response = svm->predict(sampleMat);
			double ratio = 0.5;
			if (response == 1)
				image.at<Vec3b>(i, j) = green*ratio;
			else if (response == 2)
				image.at<Vec3b>(i, j) = blue*ratio;
			else if (response == 3) {
				image.at<Vec3b>(i, j) = red*ratio;
			}
			else if (response == 4) {
				image.at<Vec3b>(i, j) = yellow*ratio;
			}
		}

	}
	int thickness = -1;
	int lineType = 8;
	circle(image, Point(100, 10), 5, Scalar(0, 255, 0), thickness, lineType);
	circle(image, Point(10, 500), 5, Scalar(255, 0, 0), thickness, lineType);
	circle(image, Point(500, 10), 5, Scalar(0, 0, 255), thickness, lineType);
	circle(image, Point(500, 500), 5, Scalar(0, 255, 255), thickness, lineType);
	thickness = 2;
	lineType = 8;
	Mat sv = svm->getSupportVectors();
	std::cout << sv << std::endl;

	for (int i = 0; i < sv.rows; ++i) {
		const float* v = sv.ptr<float>(i);
		circle(image, Point((int)v[0], (int)v[1]), 6, CV_RGB(128, 128, 128), 2);
	}
	imwrite("result.png", image);        // save the image
	imshow("SVM Simple Example", image); // show it to the user
	waitKey(0);
    return 0;
}

