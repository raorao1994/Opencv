// KnowYouFace.cpp : 定义控制台应用程序的入口点。
//人脸识别
#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp> 
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\face.hpp>
#include <iostream>
using namespace std;
using namespace cv;
using namespace cv::face;
//设置常量
//人脸库
const char* facePath = "data/haarcascade_frontalface_alt.xml";
//人脸比对文件库
string csvPath = "Face/face.csv";
//人脸图像集合
vector<Mat> images;//两个容器images,labels来存放图像数据和对应的标签 
vector<int> labels;
//csv文件分隔符
char separator = ';';
//颜色列表，用于多人人脸识别时用不同颜色区分
const static Scalar colors[] = { 
CV_RGB(0,0,255),
CV_RGB(0,128,255),
CV_RGB(0,255,255),
CV_RGB(0,255,0),
CV_RGB(255,128,0),
CV_RGB(255,255,0),
CV_RGB(255,0,0),
CV_RGB(255,0,255) };
//建立用于存放人脸的向量容器
vector<Rect> faces, faces2;
//是否翻转图片进行人脸识别
bool tryflip = false;

//读取csv文件
int read_csv()
{
	ifstream file(csvPath.c_str(), ifstream::in);
	if (!file) {
		string error_message = "没有提供有效的输入文件，请检查给定的文件名。";
		CV_Error(CV_StsBadArg, error_message);
	}
	string line, path, classlabel;
	while (getline(file, line)) {
		stringstream liness(line);
		getline(liness, path, separator);
		getline(liness, classlabel);
		if (!path.empty() && !classlabel.empty()) {
			Mat img = imread(path, 0);
			images.push_back(imread(path, 0));
			labels.push_back(atoi(classlabel.c_str()));
		}
	}
	return 0;
}

int main()
{
	//1、读取视频，得到CvCapture
	VideoCapture cap(0);
	//2、加载训练库
	read_csv();
	//2.1、创建人脸识别模型
	//opencv的FaceRecogizer目前有三个类实现了他，特征脸和fisherface方法最少训练图像为两张，而LBP可以单张图像训练  
	Ptr<FaceRecognizer> model = createEigenFaceRecognizer(); //特征脸 最少训练图像为两张
	//Ptr<FaceRecognizer> model = createFisherFaceRecognizer();//FisherFace 最少训练图像为两张
	//Ptr<FaceRecognizer> model = createLBPHFaceRecognizer();//LBP的这个方法在单个人脸验证方面效果最好
	//进行训练  
	model->train(images, labels);
	//3、识别人脸
	//创建人脸识别对象
	CascadeClassifier cascade;
	//加载识别库
	cascade.load(facePath);
	//循环读取视频帧，进行人脸识别
	bool stop = true;
	while (true)
	{
		//创建图片对象
		Mat frame;
		//从视频中获取图像
		cap >> frame;
		//IplImage *pImage = cvLoadImage("F://SVN//CShap//trunk//Opencv//FaceRecognition//KnowYouFace//Face//s13//17.jpg", CV_LOAD_IMAGE_UNCHANGED);
		//frame= cv::cvarrToMat(pImage);
		//frame = images[0];
		//转成灰色
		//对图像进行缩放，增加识别速度
		int scale = 2;
		Mat gray, smallImg(cvRound(frame.rows / scale), cvRound(frame.cols / scale), CV_8UC1);
		cvtColor(frame, gray, CV_BGR2GRAY);
		//改变图像大小，使用双线性差值
		resize(gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR);
		//变换后的图像进行直方图均值化处理
		equalizeHist(smallImg, smallImg);
		
		cascade.detectMultiScale(smallImg, faces,
			1.1, 2, 0
			//|CV_HAAR_FIND_BIGGEST_OBJECT
			//|CV_HAAR_DO_ROUGH_SEARCH
			| CV_HAAR_SCALE_IMAGE
			,
			Size(30, 30));
		//如果使能，翻转图像继续检测
		if (tryflip)
		{
			flip(smallImg, smallImg, 1);
			cascade.detectMultiScale(smallImg, faces2,
				1.1, 2, 0
				//|CV_HAAR_FIND_BIGGEST_OBJECT
				//|CV_HAAR_DO_ROUGH_SEARCH
				| CV_HAAR_SCALE_IMAGE
				,
				Size(30, 30));
			for (vector<Rect>::const_iterator r = faces2.begin(); r != faces2.end(); r++)
			{
				faces.push_back(Rect(smallImg.cols - r->x - r->width, r->y, r->width, r->height));
			}
		}
		//遍历识别出来的人脸
		Mat Testface;
		Point center;
		for (size_t i = 0; i < faces.size(); i++)
		{
			Scalar color = colors[i % 8];
			if (faces[i].height > 0 && faces[i].width > 0)
			{
				//获取测试脸
				Testface= gray(faces[i]);
				center.x = cvRound((faces[i].x + faces[i].width*0.5)*scale);
				center.y = cvRound((faces[i].y + faces[i].height*0.5)*scale);
				int radius = cvRound((faces[i].width + faces[i].height)*0.25*scale);
				CvRect rr = cvRect(center.x - radius, center.y - radius, faces[i].width * 2, faces[i].height * 2);
				rectangle(frame, rr, color, 1, 8, 0);
				//开始人脸比对
				Mat face_resized;
				//获取测试图片大小
				int im_width = images[0].cols;
				int im_height = images[0].rows;
			    //将图片转为测试图片大小
				resize(Testface, face_resized, Size(im_width, im_height), 1.0, 1.0, INTER_CUBIC);
				int prediction= -1;//比对结果index
				double confidence = 0.0;//比对匹配度
				//人脸比对
				model->predict(face_resized, prediction, confidence);
				cout << "这是第" << prediction << "个  相识度为:" << confidence << endl;
				string text = format("%f, confidence = %f", prediction, confidence);
				int n = prediction;
				std::stringstream ss,ss1;
				std::string str,str1;
				ss << n;
				ss >> str;
				ss1 << confidence;
				ss1 >> str1;
				text = str+", confidence = "+ str1;
				//将文字添加到img
				putText(frame, text, Point(center.x - faces[i].width, center.y- faces[i].height), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0, 255, 0), 2.0);
			}
		}
		imshow("视频", frame);
		char key = waitKey(100);
		if (key == 'p')
			stop = false;
	}
	//4、人脸比对
    return 0;
}

