// ConsoleApplication1.cpp : �������̨Ӧ�ó������ڵ㡣
//��һ��

#include "stdafx.h"
#include <opencv2\opencv.hpp>

int first()
{
	const char *pstrImageName = "D://8.jpg";
	const char *pstrWindowsTitle = "OpenCV��һ������";

	//���ļ��ж�ȡͼ��  
	IplImage *pImage = cvLoadImage(pstrImageName, CV_LOAD_IMAGE_UNCHANGED);

	//��������  
	cvNamedWindow(pstrWindowsTitle, CV_WINDOW_AUTOSIZE);

	//��ָ����������ʾͼ��  
	cvShowImage(pstrWindowsTitle, pImage);

	//�ȴ������¼�  
	cvWaitKey();
	//����ƶ�����
	cvDestroyWindow(pstrWindowsTitle);
	//���pImage�����ڴ�
	cvReleaseImage(&pImage);
	return 0;
}


//��һ�Σ�����opencv����ͼ��
int main()
{
	first();
	//cv::Mat�൱��cv.Mat��˼��cv�����ռ��µ�Mat��
	//������ͷ������using namespace cv;ʡȥ���cv�����ռ�
	cv::Mat img = cv::imread("D://8.jpg");//�滻�����ͼƬ·��

	cv::imshow("test", img);

	cv::waitKey();

	return 0;
}

