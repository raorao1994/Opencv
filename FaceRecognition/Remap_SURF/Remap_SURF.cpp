// Remap_SURF.cpp : �������̨Ӧ�ó������ڵ㡣
//��������
#include "stdafx.h"

#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv2/core/core.hpp"  
#include "opencv2/features2d/features2d.hpp" 
#include "opencv2/xfeatures2d/nonfree.hpp"
//#include "opencv2/nonfree/nonfree.hpp"  
#include <iostream>


using namespace cv;
using namespace cv::xfeatures2d;
void shine_upon();
void SURF_FUNC();

int main()
{
	//shine_upon();
	SURF_FUNC();
	waitKey(0);
    return 0;
}
//ӳ��
void shine_upon()
{
	//��������
	Mat srcImage, dstImage;
	Mat map_x, map_y;
	//����ԭʼͼ
	srcImage = imread("d:/8.jpg", 1);
	if (!srcImage.data) {
		printf("read image erro");
	}
	imshow("True Image", srcImage);
	dstImage.create(srcImage.size(), srcImage.type());
	map_x.create(srcImage.size(), CV_32SC1);
	map_y.create(srcImage.size(), CV_32SC1);
	//������ӳ�����
	//˫��ѭ��������ÿһ�����ص㣬�ı�map_x & map_y��ֵ  
	char key = '3';
	for (int j = 0; j < srcImage.rows; j++)
	{
		for (int i = 0; i < srcImage.cols; i++)
		{
			switch (key)
			{
			case '1': // ���̡�1�������£����е�һ����ӳ�����  
				if (i > srcImage.cols*0.25 && i < srcImage.cols*0.75 && j > srcImage.rows*0.25 && j < srcImage.rows*0.75)
				{
					map_x.at<float>(j, i) = static_cast<float>(2 * (i - srcImage.cols*0.25) + 0.5);
					map_y.at<float>(j, i) = static_cast<float>(2 * (j - srcImage.rows*0.25) + 0.5);
				}
				else
				{
					map_x.at<float>(j, i) = 0;
					map_y.at<float>(j, i) = 0;
				}
				break;
			case '2':// ���̡�2�������£����еڶ�����ӳ�����  
				map_x.at<float>(j, i) = static_cast<float>(i);
				map_y.at<float>(j, i) = static_cast<float>(srcImage.rows - j);
				break;
			case '3':// ���̡�3�������£����е�������ӳ�����  
				map_x.at<float>(j, i) = static_cast<float>(srcImage.cols - i);
				map_y.at<float>(j, i) = static_cast<float>(j);
				break;
			case '4':// ���̡�4�������£����е�������ӳ�����  
				map_x.at<float>(j, i) = static_cast<float>(srcImage.cols - i);
				map_y.at<float>(j, i) = static_cast<float>(srcImage.rows - j);
				break;
			}
		}
	}
	//������ӳ�����  
	remap(srcImage, dstImage, map_x, map_y, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));
	imshow("remap image", dstImage);
}	 
//������
void SURF_FUNC()
{
	//��1������ԴͼƬ����ʾ  
	Mat srcImage1 = imread("d:/8.jpg", 1);
	Mat srcImage2 = imread("d:/11.jpg", 1);
	if (!srcImage1.data || !srcImage2.data)//����Ƿ��ȡ�ɹ�  
	{
		printf("��ȡͼƬ������ȷ��Ŀ¼���Ƿ���imread����ָ�����Ƶ�ͼƬ����~�� \n"); return ;
	}
	imshow("ԭʼͼ1", srcImage1);
	imshow("ԭʼͼ2", srcImage2);

	//��2��������Ҫ�õ��ı�������  
	int minHessian = 400;//����SURF�е�hessian��ֵ������������

	std::vector<KeyPoint> keypoints_1, keypoints_2;
	//Opencv3.0���� #include "opencv2/xfeatures2d/nonfree.hpp"
	Ptr<SURF> detector = SURF::create(minHessian);
	detector->detect(srcImage1, keypoints_1);
	detector->detect(srcImage1, keypoints_2);
	//Opencv3.0���� "opencv2/nonfree/nonfree.hpp"  
	//SurfFeatureDetector detector(minHessian);//����һ��SurfFeatureDetector��SURF�� ������������  
	//std::vector<KeyPoint> keypoints_1, keypoints_2;//vectorģ�������ܹ�����������͵Ķ�̬���飬�ܹ����Ӻ�ѹ������  

	//��3������detect��������SURF�����ؼ��㣬������vector������  
	//detector.detect(srcImage1, keypoints_1);
	//detector.detect(srcImage2, keypoints_2);

	//��4�����������ؼ���  
	Mat img_keypoints_1; Mat img_keypoints_2;
	drawKeypoints(srcImage1, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
	drawKeypoints(srcImage2, keypoints_2, img_keypoints_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

	//��5����ʾЧ��ͼ  
	imshow("��������Ч��ͼ1", img_keypoints_1);
	imshow("��������Ч��ͼ2", img_keypoints_2);
}

