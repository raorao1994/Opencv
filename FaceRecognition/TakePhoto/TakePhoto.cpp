// TakePhoto.cpp : �������̨Ӧ�ó������ڵ㡣
//����
#include "stdafx.h"
#include<opencv2\opencv.hpp>  
using namespace std;
using namespace cv;

#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <iostream>  
//���գ�����ͼƬ
int takephoto() 
{
	VideoCapture cap(0);
	int i = 1;
	while (1)
	{
		Mat frame;
		char key = waitKey(100);
		cap >> frame;
		imshow("frame", frame);
		//string filename = format("D://Picture/FaceBase/%d.bmp", i);
		string filename = format("D://Picture//FaceBase//%d.jpg", i);
		cout << key << endl;
		switch (key)
		{
		case'p':
			i++;
			imwrite(filename, frame);
			imshow("photo", frame);
			waitKey(500);
			destroyWindow("photo");
			break;
		default:
			break;
		}
	}
	return 0;
}
//��Ƶ��Ե��ȡ
int video()
{
	VideoCapture cap(0);
	if (!cap.isOpened())
	{
		return -1;
	}
	Mat frame;
	Mat edges;

	bool stop = false;
	cvNamedWindow("��Ƶ", CV_WINDOW_FULLSCREEN);//CV_WINDOW_FULLSCREEN
	while (!stop)
	{
		cap >> frame;
		cvtColor(frame, edges, CV_BGR2GRAY);
		GaussianBlur(edges, edges, Size(7, 7), 1.5, 1.5);
		Canny(edges, edges, 0, 30, 3);
		//ͼƬ����
		//IplImage *img_v = &IplImage(edges);
		//cvResize(img_v, img_v, CV_INTER_NN);
		//resize(edges, edges, Size(), 1.5, 1.5);
		imshow("��Ƶ", edges);
		//cvShowImage("��Ƶ", img_v);
		char key = waitKey(100);
		if (key=='p')
			stop = true;
	}
	return 0;
}

int video2()
{
	//����Ƶ�ļ�����ʵ���ǽ���һ��VideoCapture�ṹ  
	const char* path = "E:/";//D:/videos/PetsD2TeC2.avi
	VideoCapture capture(path);
	//����Ƿ�������:�ɹ���ʱ��isOpened����ture  
	if (!capture.isOpened())
		cout << "fail to open!" << endl;
	//��ȡ����֡��  
	long totalFrameNumber = capture.get(CV_CAP_PROP_FRAME_COUNT);
	cout << "������Ƶ��" << totalFrameNumber << "֡" << endl;


	//���ÿ�ʼ֡()  
	long frameToStart = 300;
	capture.set(CV_CAP_PROP_POS_FRAMES, frameToStart);
	cout << "�ӵ�" << frameToStart << "֡��ʼ��" << endl;


	//���ý���֡  
	int frameToStop = 400;

	if (frameToStop < frameToStart)
	{
		cout << "����֡С�ڿ�ʼ֡��������󣬼����˳���" << endl;
		return -1;
	}
	else
	{
		cout << "����֡Ϊ����" << frameToStop << "֡" << endl;
	}


	//��ȡ֡��  
	double rate = capture.get(CV_CAP_PROP_FPS);
	cout << "֡��Ϊ:" << rate << endl;



	//����һ���������ƶ�ȡ��Ƶѭ�������ı���  
	bool stop = false;
	//����ÿһ֡��ͼ��  
	Mat frame;
	//��ʾÿһ֡�Ĵ���  
	namedWindow("Extracted frame");
	//��֡��ļ��ʱ��:  
	//int delay = 1000/rate;  
	int delay = 1000 / rate;


	//����whileѭ����ȡ֡  
	//currentFrame����ѭ�����п��ƶ�ȡ��ָ����֡��ѭ�������ı���  
	long currentFrame = frameToStart;


	//�˲����ĺ�  
	int kernel_size = 3;
	Mat kernel = Mat::ones(kernel_size, kernel_size, CV_32F) / (float)(kernel_size*kernel_size);

	while (!stop)
	{
		//��ȡ��һ֡  
		if (!capture.read(frame))
		{
			cout << "��ȡ��Ƶʧ��" << endl;
			return -1;
		}

		//������˲�����  
		imshow("Extracted frame", frame);
		filter2D(frame, frame, -1, kernel);

		imshow("after filter", frame);
		cout << "���ڶ�ȡ��" << currentFrame << "֡" << endl;
		//waitKey(int delay=0)��delay �� 0ʱ����Զ�ȴ�����delay>0ʱ��ȴ�delay����  
		//��ʱ�����ǰû�а�������ʱ������ֵΪ-1�����򷵻ذ���  


		int c = waitKey(delay);
		//����ESC���ߵ���ָ���Ľ���֡���˳���ȡ��Ƶ  
		if ((char)c == 27 || currentFrame > frameToStop)
		{
			stop = true;
		}
		//���°������ͣ���ڵ�ǰ֡���ȴ���һ�ΰ���  
		if (c >= 0)
		{
			waitKey(0);
		}
		currentFrame++;

	}
	//�ر���Ƶ�ļ�  
	capture.release();
	waitKey(0);
	return 0;
}

int video3()
{
	cvNamedWindow("Example1", CV_WINDOW_AUTOSIZE);
	CvCapture* capture; //��ʼ��һ��CvCapture�ṹ��ָ��
	capture = cvCaptureFromCAM(0);//�������Ϊ1���������ͷ�ж������ݣ�������һ��CvCapture��ָ��
	assert(capture != NULL); //���ԣ�assert��ʹ�ã����capture�Ƿ�Ϊ��ָ�룬Ϊ��ʱ�����˳�������ӡ������Ϣ
	IplImage* frame;
	while (1)
	{
		frame = cvQueryFrame(capture);//���ڽ���һ֡��Ƶ�ļ������ڴ棨ʵ�������͸���CvCapture�ṹ�У�������һ����Ӧ��ǰ֡��ָ��
		if (!frame)
			break;
		cvShowImage("Example1", frame);
		char c = cvWaitKey(33);
		if (c == 27) break; //����ESC���˳�ѭ������������ֹͣ
	}
	cvReleaseCapture(&capture);//�ͷ��ڴ�
	cvDestroyWindow("Example1");
	return 2;
}

int main()
{
	takephoto();
	//video();
	//video2();
	//video3();
	return 0;
}

