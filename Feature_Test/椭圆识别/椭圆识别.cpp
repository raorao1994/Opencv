// ��Բʶ��.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv2/highgui/highgui.hpp"  
#include <iostream>  

using namespace cv;
using namespace std;

int main()
{
	string filename = "E:\\TablewareRecognition\\��\\����-2\\΢��ͼƬ_20180416144009.jpg";
	Mat src_image = imread(filename, 0);
	if (src_image.empty())
	{
		cout << "Couldn't open image!" << filename;
		return 0;
	}

	imshow("ԭͼ", src_image);

	//����  
	vector<vector<Point>> contours;

	//ʹ��canny������Ե  
	Mat edge_image;
	Canny(src_image, edge_image, 30, 70);
	imshow("canny��Ե", edge_image);

	//��Ե׷�٣�û�д洢��Ե����֯�ṹ  
	findContours(edge_image, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	Mat cimage = Mat::zeros(edge_image.size(), CV_8UC3);

	for (size_t i = 0; i < contours.size(); i++)
	{
		//��ϵĵ�����Ϊ6  
		size_t count = contours[i].size();
		if (count < 16)
			continue;
		//double area = abs(contourArea(contours[i]));
		//if (area < 3600|| area>170000)
		//	continue;
		//��Բ���  
		RotatedRect box = fitEllipse(contours[i]);

		//�������ȴ���30�����ų����������  
		if (MAX(box.size.width, box.size.height) > MIN(box.size.width, box.size.height) * 30)
			continue;
		if (MAX(box.size.width, box.size.height) <150)
			continue;

		//����׷�ٳ�������  
		drawContours(cimage, contours, (int)i, Scalar::all(255), 1, 8);

		//������ϵ���Բ  
		ellipse(cimage, box, Scalar(0, 0, 255), 1, CV_AA);
	}
	imshow("��Ͻ��", cimage);

	waitKey();
	return 0;
}

