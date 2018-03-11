// ���ڱ�Ե������ʶ��.cpp : �������̨Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

//����ȫ�ֱ���
//��ȡ������
Ptr<SurfFeatureDetector> surfDetector = SurfFeatureDetector::create(800);
//������������Ϊ�±ߵ�������ƥ����׼��    
Ptr<SurfDescriptorExtractor> SurfDescriptor = SurfDescriptorExtractor::create();

//��������
vector<KeyPoint> GetTempleKetPoint(string path, OutputArray ContourImg);
Mat GetContourImg(string path);

int main()
{
	//1������ģ��ͼ��,���ҳ�ͼ���Ե������
	Mat TempImg;
	vector<KeyPoint> TempKeyPoint= GetTempleKetPoint("t1.jpg", TempImg);
	//2�����ش�ʶ��ͼ�񣬻�ȡͼ���Եͼ��
	Mat SrarchImg;
	vector<KeyPoint> SrarchKeyPoint = GetTempleKetPoint("1.jpg",SrarchImg);
	//3������ģ��ʶ�������㣬�ڱ�Եͼ����ʶ���Ӧ�㣬
	//3.1���������������
	Mat imageDesc1, imageDesc2;
	SurfDescriptor->compute(TempImg, TempKeyPoint, imageDesc1);
	SurfDescriptor->compute(SrarchImg, SrarchKeyPoint, imageDesc2);
	//������ƥ�䲢��ʾƥ����       
	FlannBasedMatcher matcher;
	vector<DMatch> matchePoints;
	matcher.match(imageDesc1, imageDesc2, matchePoints, Mat());

	//�������������  
	cout << "����������ǰ���룺" << endl;
	for (int i = 0; i<matchePoints.size(); i++) //��������㰴��������ǰ����  
	{
		cout << matchePoints[i].distance << endl;
	}
	sort(matchePoints.begin(), matchePoints.end()); //�������С��������
	cout << "�������������룺" << endl;
	for (int i = 0; i<matchePoints.size(); i++)
	{
		cout << matchePoints[i].distance << endl;
	}

	//��ȡǿ�����㣬��ȡ����ǰN��������ƥ����
	vector<DMatch> goodMatchePoints;
	for (int i = 0; i<10; i++)
	{
		goodMatchePoints.push_back(matchePoints[i]);
	}

	//��������ƥ���  
	Mat imageOutput;
	drawMatches(TempImg, TempKeyPoint, SrarchImg, SrarchKeyPoint, goodMatchePoints, imageOutput, Scalar::all(-1),
		Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	imshow("imageOutput", imageOutput);
	waitKey(0);
    return 0;
}
//��ȡģ��������
vector<KeyPoint> GetTempleKetPoint(string path, OutputArray ContourImg)
{
	//1.1��ȡͼƬ��Եͼ��
	Mat ContoursImg = GetContourImg(path);
	ContourImg.create(ContoursImg.size(), ContoursImg.type());
	//Mat dst = ContourImg.getMat();
	ContoursImg.copyTo(ContourImg);
	//1.2���ģ��������
	vector<KeyPoint> KeyPoint;
	surfDetector->detect(ContoursImg, KeyPoint);
	drawKeypoints(ContoursImg, KeyPoint, ContoursImg, Scalar::all(-1));
	//�ͷ�ContoursImg�ڴ���Դ
	ContoursImg.release();
	return KeyPoint;
}
//��ȡͼƬ��Եͼ��
Mat GetContourImg(string path)
{
	Mat TempleImgSrc = imread(path, CV_8UC1);
	Mat TempleImg;
	if (TempleImgSrc.channels() == 3)
	{
		cvtColor(TempleImgSrc, TempleImg, CV_BGR2GRAY);
	}
	else
	{
		TempleImgSrc.copyTo(TempleImg);
	}
	//�ͷ�ԭͼ�ڴ���Դ
	TempleImgSrc.release();
	//1.1��ģ��ͼ����б�Ե��ֵ������
	Mat ThresholdImg;
	threshold(TempleImg, ThresholdImg, 100, 255, THRESH_BINARY | THRESH_OTSU);
	//�ͷ�ģ���ڴ���Դ
	TempleImg.release();
	//1.2�Զ�ֵͼ����б�Ե��ȡ����
	vector<vector<Point>> contours;
	findContours(ThresholdImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	Mat ContoursImg = Mat::zeros(ThresholdImg.size(), CV_8UC1);
	drawContours(ContoursImg, contours, -1, Scalar(255), 4);
	return ContoursImg;
}
