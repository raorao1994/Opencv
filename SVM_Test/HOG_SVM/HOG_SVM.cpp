// HOG_SVM.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>  
#include <opencv2/opencv.hpp>  

using namespace cv;
using namespace std;

int main()
{
	Mat image = imread("1.jpg");
	// 1. ����HOG���� 
	//HOG���������������HOG�����ӵ�
	HOGDescriptor hog(Size(64, 128), Size(16, 16), Size(8, 8), Size(8, 8), 9);
	// 2. ����SVM������
	// �����Ѿ�ѵ���õ����˼�������
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());   
	// 3. �ڲ���ͼ���ϼ����������
	vector<cv::Rect> regions;
	hog.detectMultiScale(image, regions, 0, Size(8, 8), Size(32, 32), 1.05, 1);
	// ��ʾ  
	for (size_t i = 0; i < regions.size(); i++)
	{
		rectangle(image, regions[i], Scalar(0, 0, 255), 2);
	}
	imshow("HOG���˼��", image);
	waitKey();

	return 0;
}

