// Eigenfaces.cpp : 定义控制台应用程序的入口点。
// Eigenfaces特征脸createEigenFaceRecognizer()
#include "stdafx.h"
#include <iostream>
#include <opencv2\opencv.hpp>  
#include <opencv2\core\core.hpp>  
#include <opencv2\highgui\highgui.hpp> 
#include <opencv2\face.hpp>
using namespace std;
using namespace cv;
using namespace cv::face;

int facea()
{
	VideoCapture cap(0);    //打开默认摄像头  
	if (!cap.isOpened())
	{
		return -1;
	}
	Mat frame;
	Mat edges;
	Mat gray;

	CascadeClassifier cascade;
	bool stop = false;
	//训练好的文件名称，放置在可执行文件同目录下  
	const char *frontalface = "data/haarcascade_frontalface_alt.xml";
	cascade.load(frontalface);
	Ptr<cv::face::FaceRecognizer> modelPCA = cv::face::createEigenFaceRecognizer();
	const char *filepath2 = "Face/face.csv";
	modelPCA->load("MyFacePCAModel.xml");
	//modelPCA->load(filepath2);
	while (1)
	{
		cap >> frame;

		//建立用于存放人脸的向量容器  
		vector<Rect> faces(0);

		cvtColor(frame, gray, CV_BGR2GRAY);
		//改变图像大小，使用双线性差值  
		//resize(gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR);  
		//变换后的图像进行直方图均值化处理  
		equalizeHist(gray, gray);

		cascade.detectMultiScale(gray, faces,
			1.1, 2, 0
			//|CV_HAAR_FIND_BIGGEST_OBJECT  
			//|CV_HAAR_DO_ROUGH_SEARCH  
			| CV_HAAR_SCALE_IMAGE,
			Size(30, 30));

		Mat face;
		Point text_lb;

		for (size_t i = 0; i < faces.size(); i++)
		{
			if (faces[i].height > 0 && faces[i].width > 0)
			{
				face = gray(faces[i]);
				text_lb = Point(faces[i].x, faces[i].y);

				rectangle(frame, faces[i], Scalar(255, 0, 0), 1, 8, 0);
			}
		}

		Mat face_test;

		int predictPCA = 0;
		if (face.rows >= 120)
		{
			resize(face, face_test, Size(92, 112));

		}
		//Mat face_test_gray;  
		//cvtColor(face_test, face_test_gray, CV_BGR2GRAY);  

		if (!face_test.empty())
		{
			//测试图像应该是灰度图  
			predictPCA = modelPCA->predict(face_test);
		}

		cout << predictPCA << endl;
		if (predictPCA == 41)
		{
			string name = "LiuXiaoLong";
			putText(frame, name, text_lb, FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255));
		}

		imshow("face", frame);
		waitKey(200);
	}
	return 0;
}


int main1()
{
	//facea();
	cout << "usage:   <csv.ext> <output_folder> " << endl;
    return 0;
}

