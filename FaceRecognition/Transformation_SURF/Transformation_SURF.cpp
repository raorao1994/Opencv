// Transformation_SURF.cpp : �������̨Ӧ�ó������ڵ㡣
//OpenCV����任 & SURF�����������ϼ�
#include "stdafx.h"
#include "opencv2/core/core.hpp"  
#include "opencv2/features2d/features2d.hpp"  
#include "opencv2/highgui/highgui.hpp"
//#include<opencv2/legacy/legacy.hpp>  
#include <opencv2/xfeatures2d/nonfree.hpp>
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  
#include <iostream>  
using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;

void Transformation();
void SURF_FUNC();
int main()
{
	//Transformation();
	SURF_FUNC();
	// �ȴ��û������ⰴ���˳�����  
	waitKey(0);
    return 0;
}
//����仯
static void Transformation()
{
	//��0���ı�console������ɫ  
	system("color 1A");

	//��1������׼��  
	//��������㣬��������������  
	Point2f srcTriangle[3];
	Point2f dstTriangle[3];
	//����һЩMat����  
	Mat rotMat(2, 3, CV_32FC1);
	Mat warpMat(2, 3, CV_32FC1);
	Mat srcImage, dstImage_warp, dstImage_warp_rotate;

	//��2������Դͼ����һЩ��ʼ��  
	srcImage = imread("d:/8.jpg", 1);
	if (!srcImage.data) { printf("��ȡͼƬ������ȷ��Ŀ¼���Ƿ���imread����ָ����ͼƬ����~�� \n"); return; }
	// ����Ŀ��ͼ��Ĵ�С��������Դͼ��һ��  
	dstImage_warp = Mat::zeros(srcImage.rows, srcImage.cols, srcImage.type());

	//��3������Դͼ���Ŀ��ͼ���ϵ�������Լ������任  
	srcTriangle[0] = Point2f(0, 0);
	srcTriangle[1] = Point2f(static_cast<float>(srcImage.cols - 1), 0);
	srcTriangle[2] = Point2f(0, static_cast<float>(srcImage.rows - 1));

	dstTriangle[0] = Point2f(static_cast<float>(srcImage.cols*0.0), static_cast<float>(srcImage.rows*0.33));
	dstTriangle[1] = Point2f(static_cast<float>(srcImage.cols*0.65), static_cast<float>(srcImage.rows*0.35));
	dstTriangle[2] = Point2f(static_cast<float>(srcImage.cols*0.15), static_cast<float>(srcImage.rows*0.6));

	//��4����÷���任  
	warpMat = getAffineTransform(srcTriangle, dstTriangle);

	//��5����Դͼ��Ӧ�øո���õķ���任  
	warpAffine(srcImage, dstImage_warp, warpMat, dstImage_warp.size());

	//��6����ͼ��������ź�����ת  
	// ������ͼ���е�˳ʱ����ת50����������Ϊ0.6����ת����  
	Point center = Point(dstImage_warp.cols / 2, dstImage_warp.rows / 2);
	double angle = -30.0;
	double scale = 0.8;
	// ͨ���������תϸ����Ϣ�����ת����  
	rotMat = getRotationMatrix2D(center, angle, scale);
	// ��ת�����ź��ͼ��  
	warpAffine(dstImage_warp, dstImage_warp_rotate, rotMat, dstImage_warp.size());


	//��7����ʾ���  
	imshow("����1", srcImage);
	imshow("����2", dstImage_warp);
	imshow("����3", dstImage_warp_rotate);
}
//SURFʶ��
static void SURF_FUNC()
{
	//��1�������ز�ͼ  
	Mat srcImage1 = imread("d:/9.jpg", 1);
	Mat srcImage2 = imread("d:/12.jpg", 1);
	if (!srcImage1.data || !srcImage2.data)
	{
		printf("��ȡͼƬ������ȷ��Ŀ¼���Ƿ���imread����ָ����ͼƬ����~�� \n"); return;
	}

	//��2��ʹ��SURF���Ӽ��ؼ���  
	int minHessian = 400;//SURF�㷨�е�hessian��ֵ
	Ptr<SURF> detector = SURF::create(minHessian);
	//SurfFeatureDetector detector(minHessian);//����һ��SurfFeatureDetector��SURF�� ������������    
	std::vector<KeyPoint> keyPoint1, keyPoints2;//vectorģ���࣬����������͵Ķ�̬����  

												//��3������detect��������SURF�����ؼ��㣬������vector������  
	detector->detect(srcImage1, keyPoint1);
	detector->detect(srcImage2, keyPoints2);

	//��4������������������������  
	//SurfDescriptorExtractor extractor;
	Ptr<SURF> extractor = SURF::create();
	Mat descriptors1, descriptors2;
	extractor->compute(srcImage1, keyPoint1, descriptors1);
	extractor->compute(srcImage2, keyPoints2, descriptors2);

	//��5��ʹ��BruteForce����ƥ��  
	// ʵ����һ��ƥ����  
	//BruteForceMatcher< L2<float> > matcher;
	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce");
	std::vector< DMatch > matches;
	//ƥ������ͼ�е������ӣ�descriptors��  
	matcher->match(descriptors1, descriptors2, matches);

	//��6�����ƴ�����ͼ����ƥ����Ĺؼ���  
	Mat imgMatches;
	drawMatches(srcImage1, keyPoint1, srcImage2, keyPoints2, matches, imgMatches);//���л���  
	//��7����ʾЧ��ͼ  
	imshow("ƥ��ͼ", imgMatches);
}


