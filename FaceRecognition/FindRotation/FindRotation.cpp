// FindRotation.cpp : �������̨Ӧ�ó������ڵ㡣
// OpenCV������⣬����������ת�Ƕ�


#include "stdafx.h"
#include <iostream>  
#include <vector>  
#include <opencv2/opencv.hpp>   
#include <opencv2/core/core.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#define PI 3.1415926  
using namespace std;
using namespace cv;

int hough_line(Mat src)
{
	//��1������ԭʼͼ��Mat��������     
	Mat srcImage = src;//imread("1.jpg");  //����Ŀ¼��Ӧ����һ����Ϊ1.jpg���ز�ͼ  
	Mat midImage, dstImage;//��ʱ������Ŀ��ͼ�Ķ���  
	//��2�����б�Ե����ת��Ϊ�Ҷ�ͼ  
	Canny(srcImage, midImage, 50, 200, 3);//����һ��canny��Ե���  
	cvtColor(midImage, dstImage, CV_GRAY2BGR);//ת����Ե�����ͼΪ�Ҷ�ͼ  
	//��3�����л����߱任  
	vector<Vec4i> lines;//����һ��ʸ���ṹlines���ڴ�ŵõ����߶�ʸ������  
	HoughLinesP(midImage, lines, 1, CV_PI / 180, 80, 50, 10);
	//��4��������ͼ�л��Ƴ�ÿ���߶�  
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(dstImage, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(186, 88, 255), 1, CV_AA);
	}
	//��5����ʾԭʼͼ    
	imshow("��ԭʼͼ��", srcImage);
	//��6����Ե�����ͼ   
	imshow("����Ե�����ͼ��", midImage);
	//��7����ʾЧ��ͼ    
	imshow("��Ч��ͼ��", dstImage);
	//waitKey(0);
	return 0;
}

int main()
{
	// Read input binary image
	char *image_name = "test.png";//test.png
	Mat image = imread(image_name, 0);
	if (!image.data)
		return 0;
	imshow("ԭͼ1", image);
	// ���ļ��м���ԭͼ
	IplImage *pSrcImage = cvLoadImage(image_name, CV_LOAD_IMAGE_UNCHANGED);
	// תΪ2ֵͼ
	cvThreshold(pSrcImage, pSrcImage, 200, 255, THRESH_BINARY_INV);
	image = cvarrToMat(pSrcImage);
	imwrite("binary.jpg", image);
	cvtColor(image, image, CV_BGR2GRAY);
	// Get the contours(����) of the connected components������Ҫ�ء�������
	vector<vector<Point>> contours;
	findContours(image,
		contours, // a vector of contours 
		CV_RETR_EXTERNAL, // retrieve the external contours
		CV_CHAIN_APPROX_NONE); // retrieve all pixels of each contours
	// Print contours' length
	cout << "Contours: " << contours.size() << endl;
	vector<vector<Point>>::const_iterator itContours = contours.begin();
	for (; itContours != contours.end(); ++itContours)
	{
		cout << "Size: " << itContours->size() << endl;
	}
	// draw black contours on white image
	Mat result(image.size(), CV_8U, Scalar(255));
	drawContours(result, contours,
		-1, // draw all contours
		Scalar(0), // in black
		2); // with a thickness of 2
	imshow("����2", result);
	// Eliminate too short or too long contours
	int cmin = 100;  // minimum contour length
	int cmax = 1000; // maximum contour length
	vector<vector<Point>>::const_iterator itc = contours.begin();
	while (itc != contours.end()) {
		if (itc->size() < cmin || itc->size() > cmax)
			//����
			itc = contours.erase(itc);
		else
			++itc;
	}
	// draw contours on the original(ԭʼ) image
	Mat original = imread(image_name);
	drawContours(original, contours,
		-1, // draw all contours
		Scalar(255, 255, 0), // in white
		2); // with a thickness of 2
	imshow("ԭʼ����3", original);
	// Let's now draw black contours on white image
	result.setTo(Scalar(255));
	drawContours(result, contours,
		-1, // draw all contours
		Scalar(0), // in black
		1); // with a thickness of 1
	image = imread("binary.jpg", 0);
	// testing the bounding box
	//����任����ֱ�߼�⣬�˴�ʹ�õ���probabilistic Hough transform��HoughLinesP��������standard Hough transform��HoughLines��  
	Mat result_line(image.size(), CV_8U, Scalar(255));
	result_line = result.clone();
	//hough_line(result_line);
	vector<Vec4i> lines;
	HoughLinesP(result_line, lines, 1, CV_PI / 180, 80, 50, 10);
	for (int i = 0; i < lines.size(); i++)
	{
		line(result_line, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(0, 0, 0), 2, 8, 0);
	}
	imshow("����4", result_line);
	//��ȡ��Ե����С���ο�
	vector<vector<Point>>::const_iterator itc_rec= contours.begin();
	while (itc_rec!=contours.end())
	{
	  Rect r0= boundingRect(Mat(*(itc_rec)));
	  rectangle(result,r0,Scalar(0),2);
	      ++itc_rec;
	}
	imshow("һЩͼ������5",result);
	CvBox2D	End_Rage2D;
	CvPoint2D32f rectpoint[4];
	//�����ڴ�ռ�
	CvMemStorage *storage = cvCreateMemStorage(0);
	//CvSeq���� ��ż�⵽��ͼ��������Ե���е�����ֵ������ֵ�����Ľṹ����������ʽ
	CvSeq* contour;
	//��̵ؼ�����ɫ(��)�����а�ɫ(����)����������ⲿ��(��ѡ)�ڲ��߽硣
	//�⺯����ѡ�������в���
	//���contour��Ϊ�գ���ʾ�ҵ�һ����������������д��ֻ��ʾһ������
	//���Ϊfor(; contour; contour = contour->h_next) �Ϳ���ͬʱ��ʾ�������
	IplImage* img_i = cvLoadImage(image_name, CV_LOAD_IMAGE_GRAYSCALE);
	IplImage* img_temp = cvCreateImage(cvGetSize(img_i), 8, 1);
	cvThreshold(img_i, img_i, 128, 255, CV_THRESH_BINARY);
	cvCopy(img_i, img_temp);
	cvFindContours(img_temp, storage, &contour, sizeof(CvContour),
		CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
	for (; contour; contour = contour->h_next)
	{
		End_Rage2D = cvMinAreaRect2(contour);
		//����cvMinAreaRect2��������õ���С��Χ����  �����ѵó���������ĽǶȣ����,�߶ȣ����е����������CvBox2D���͵Ľṹ���У�  
		//��Ҫ��������������
		for (int i = 0; i < 4; i++)
		{  
			line(result, cvPointFrom32f(rectpoint[i]), 
				cvPointFrom32f(rectpoint[(i + 1) % 4]), Scalar(125), 2);
		}
		cvBoxPoints(End_Rage2D, rectpoint);
		cout << " angle:\n" << (float)End_Rage2D.angle << endl;      //����������ת�Ƕ�
	}
	imshow("���6", result);
	waitKey();
	return 0;
}

