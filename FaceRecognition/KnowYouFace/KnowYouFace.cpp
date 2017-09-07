// KnowYouFace.cpp : �������̨Ӧ�ó������ڵ㡣
//����ʶ��
#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp> 
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\face.hpp>
#include <iostream>
using namespace std;
using namespace cv;
using namespace cv::face;
//���ó���
//������
const char* facePath = "data/haarcascade_frontalface_alt.xml";
//�����ȶ��ļ���
string csvPath = "Face/face.csv";
//����ͼ�񼯺�
vector<Mat> images;//��������images,labels�����ͼ�����ݺͶ�Ӧ�ı�ǩ 
vector<int> labels;
//csv�ļ��ָ���
char separator = ';';
//��ɫ�б����ڶ�������ʶ��ʱ�ò�ͬ��ɫ����
const static Scalar colors[] = { 
CV_RGB(0,0,255),
CV_RGB(0,128,255),
CV_RGB(0,255,255),
CV_RGB(0,255,0),
CV_RGB(255,128,0),
CV_RGB(255,255,0),
CV_RGB(255,0,0),
CV_RGB(255,0,255) };
//�������ڴ����������������
vector<Rect> faces, faces2;
//�Ƿ�תͼƬ��������ʶ��
bool tryflip = false;

//��ȡcsv�ļ�
int read_csv()
{
	ifstream file(csvPath.c_str(), ifstream::in);
	if (!file) {
		string error_message = "û���ṩ��Ч�������ļ�������������ļ�����";
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
	//1����ȡ��Ƶ���õ�CvCapture
	VideoCapture cap(0);
	//2������ѵ����
	read_csv();
	//2.1����������ʶ��ģ��
	//opencv��FaceRecogizerĿǰ��������ʵ����������������fisherface��������ѵ��ͼ��Ϊ���ţ���LBP���Ե���ͼ��ѵ��  
	Ptr<FaceRecognizer> model = createEigenFaceRecognizer(); //������ ����ѵ��ͼ��Ϊ����
	//Ptr<FaceRecognizer> model = createFisherFaceRecognizer();//FisherFace ����ѵ��ͼ��Ϊ����
	//Ptr<FaceRecognizer> model = createLBPHFaceRecognizer();//LBP����������ڵ���������֤����Ч�����
	//����ѵ��  
	model->train(images, labels);
	//3��ʶ������
	//��������ʶ�����
	CascadeClassifier cascade;
	//����ʶ���
	cascade.load(facePath);
	//ѭ����ȡ��Ƶ֡����������ʶ��
	bool stop = true;
	while (true)
	{
		//����ͼƬ����
		Mat frame;
		//����Ƶ�л�ȡͼ��
		cap >> frame;
		//IplImage *pImage = cvLoadImage("F://SVN//CShap//trunk//Opencv//FaceRecognition//KnowYouFace//Face//s13//17.jpg", CV_LOAD_IMAGE_UNCHANGED);
		//frame= cv::cvarrToMat(pImage);
		//frame = images[0];
		//ת�ɻ�ɫ
		//��ͼ��������ţ�����ʶ���ٶ�
		int scale = 2;
		Mat gray, smallImg(cvRound(frame.rows / scale), cvRound(frame.cols / scale), CV_8UC1);
		cvtColor(frame, gray, CV_BGR2GRAY);
		//�ı�ͼ���С��ʹ��˫���Բ�ֵ
		resize(gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR);
		//�任���ͼ�����ֱ��ͼ��ֵ������
		equalizeHist(smallImg, smallImg);
		
		cascade.detectMultiScale(smallImg, faces,
			1.1, 2, 0
			//|CV_HAAR_FIND_BIGGEST_OBJECT
			//|CV_HAAR_DO_ROUGH_SEARCH
			| CV_HAAR_SCALE_IMAGE
			,
			Size(30, 30));
		//���ʹ�ܣ���תͼ��������
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
		//����ʶ�����������
		Mat Testface;
		Point center;
		for (size_t i = 0; i < faces.size(); i++)
		{
			Scalar color = colors[i % 8];
			if (faces[i].height > 0 && faces[i].width > 0)
			{
				//��ȡ������
				Testface= gray(faces[i]);
				center.x = cvRound((faces[i].x + faces[i].width*0.5)*scale);
				center.y = cvRound((faces[i].y + faces[i].height*0.5)*scale);
				int radius = cvRound((faces[i].width + faces[i].height)*0.25*scale);
				CvRect rr = cvRect(center.x - radius, center.y - radius, faces[i].width * 2, faces[i].height * 2);
				rectangle(frame, rr, color, 1, 8, 0);
				//��ʼ�����ȶ�
				Mat face_resized;
				//��ȡ����ͼƬ��С
				int im_width = images[0].cols;
				int im_height = images[0].rows;
			    //��ͼƬתΪ����ͼƬ��С
				resize(Testface, face_resized, Size(im_width, im_height), 1.0, 1.0, INTER_CUBIC);
				int prediction= -1;//�ȶԽ��index
				double confidence = 0.0;//�ȶ�ƥ���
				//�����ȶ�
				model->predict(face_resized, prediction, confidence);
				cout << "���ǵ�" << prediction << "��  ��ʶ��Ϊ:" << confidence << endl;
				string text = format("%f, confidence = %f", prediction, confidence);
				int n = prediction;
				std::stringstream ss,ss1;
				std::string str,str1;
				ss << n;
				ss >> str;
				ss1 << confidence;
				ss1 >> str1;
				text = str+", confidence = "+ str1;
				//��������ӵ�img
				putText(frame, text, Point(center.x - faces[i].width, center.y- faces[i].height), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0, 255, 0), 2.0);
			}
		}
		imshow("��Ƶ", frame);
		char key = waitKey(100);
		if (key == 'p')
			stop = false;
	}
	//4�������ȶ�
    return 0;
}

