// ORB_Recognition.cpp : �������̨Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

using namespace std;
using namespace cv;

int score = 30;
void func();
void func1();
void func2();
void func3();
void func4();

void change_score(int, void*)
{
	
}

int main()
{
	//func();
	//func1();
	//func2();
	//func3();
	func4();
	waitKey(0);
	//system("pause");
    return 0;
}

void func()
{
	Mat img1 = imread("d:/phone.jpg", CV_LOAD_IMAGE_GRAYSCALE);

	//ʶ��������
	vector<KeyPoint> keypoints1, keypoints2;
	Ptr<ORB> orb = ORB::create();
	orb->detect(img1, keypoints1);

	//����
	Mat descriptors1, descriptors2;
	orb->compute(img1, keypoints1, descriptors1);


	VideoCapture cap(0);
	Mat frame, gary;
	//ƥ��
	BFMatcher matcher(NORM_HAMMING);
	vector<DMatch> matches;
	while (true)
	{
		cap >> frame;
		cvtColor(frame, gary, CV_RGB2GRAY);
		orb->detect(gary, keypoints2);
		if (keypoints2.empty())
			continue;
		orb->compute(gary, keypoints2, descriptors2);
		matcher.match(descriptors1, descriptors2, matches);
		//����
		Mat result;
		drawMatches(img1, keypoints1, gary, keypoints2, matches, result);
		imshow("result", result);
		waitKey(2);
	}
}

void func1()
{
	//1������ģ��ͼƬ
	Mat temple = imread("d:/img/71.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	imshow("ԭͼ",temple);
	// ����trackbar  
	createTrackbar("����ֵ��", "ԭͼ", &score, 120, change_score);
	//2����ȡͼƬ��Ե��Ϣ
	Mat edge_image;
	Canny(temple, edge_image, 30, 70);
	//imshow("��Եͼ��",edge_image);
	//3����ֵ��
	Mat threshold_img;
	threshold(edge_image, threshold_img, 100, 255, THRESH_BINARY);
	//3����ȡ��Ե
	vector<vector<Point>> contours;
	findContours(threshold_img, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
	//4����Ե����
	Mat threshold_img_new = Mat::zeros(threshold_img.size(), threshold_img.type());
	for (size_t i = 0; i < contours.size(); i++)
	{
		if (contours[i].size() <= 3)
			continue;
		vector<Point> _contour = contours[i];
		int salient_point = 0;
		//4.1�������Ե͹�����
		for (size_t j = 1; j < _contour.size()-2; j++)
		{
			if ((abs(_contour[j - 1].x + _contour[j + 1].x - 2 * _contour[j].x) +
				abs(_contour[j - 1].y + _contour[j + 1].y - 2 * _contour[j].y)) > 3)
				salient_point++;
		}
		//4.2���жϱ�Ե�Ƿ�Ϊ����
		if (salient_point >= _contour.size()*0.35)
			drawContours(threshold_img_new, contours, i, Scalar(255),3);
	}
	//imshow("������ı�Ե",threshold_img_new);
	//5����ȡ�����㣬ʶ��������
	vector<KeyPoint> keypoints1, keypoints2;
	Ptr<ORB> orb = ORB::create();
	orb->detect(temple, keypoints1);
	//6������
	Mat descriptors1, descriptors2;
	orb->compute(temple, keypoints1, descriptors1);
	//7����ȡ����ͷ
	VideoCapture cap(0);
	Mat frame, gary;
	//ƥ��
	BFMatcher matcher(NORM_HAMMING);
	vector<DMatch> matches;
	while (true)
	{
		cap >> frame;
		cvtColor(frame, gary, CV_RGB2GRAY);
		orb->detect(gary, keypoints2);
		if (keypoints2.empty())
			continue;
		orb->compute(gary, keypoints2, descriptors2);
		matcher.match(descriptors1, descriptors2, matches);
		//����ʶ��ƥ���
		vector<DMatch> _matches;
		for (size_t i = 0; i < matches.size(); i++)
		{
			if (matches[i].distance < score)
				_matches.push_back(matches[i]);
		}
		//����
		Mat result;
		drawMatches(temple, keypoints1, gary, keypoints2, _matches, result);
		imshow("result", result);
		waitKey(2);
	}
}

void func2()
{
	Mat img_1 = imread("d:/phone.jpg");
	Mat img_2 = imread("d:/phone.bmp");

	if (!img_1.data || !img_2.data)
	{
		cout << "error reading images " << endl;
	}

	vector<Point2f> recognized;
	vector<Point2f> scene;

	recognized.resize(500);
	scene.resize(500);

	Mat d_srcL, d_srcR;

	Mat img_matches, des_L, des_R;
	//ORB�㷨��Ŀ������ǻҶ�ͼ��  
	cvtColor(img_1, d_srcL, COLOR_BGR2GRAY);//CPU���ORB�㷨Դ�����Դ�������ͼ��ҶȻ����˲���ʡ��  
	cvtColor(img_2, d_srcR, COLOR_BGR2GRAY);

	Ptr<ORB> d_orb = ORB::create();

	Mat d_descriptorsL, d_descriptorsR;

	vector<KeyPoint> keyPoints_1, keyPoints_2;

	//���ùؼ�����ƥ�䷽ʽΪNORM_L2��������ʹ�� FLANNBASED = 1, BRUTEFORCE = 2, BRUTEFORCE_L1 = 3, BRUTEFORCE_HAMMING = 4, BRUTEFORCE_HAMMINGLUT = 5, BRUTEFORCE_SL2 = 6   
	Ptr<DescriptorMatcher> d_matcher = DescriptorMatcher::create(NORM_L2);

	std::vector<DMatch> matches;//��ͨƥ��  
	std::vector<DMatch> good_matches;//ͨ��keyPoint֮�����ɸѡƥ��ȸߵ�ƥ����  

	d_orb->detectAndCompute(d_srcL, Mat(), keyPoints_1, d_descriptorsL);

	d_orb->detectAndCompute(d_srcR, Mat(), keyPoints_2, d_descriptorsR);

	d_matcher->match(d_descriptorsL, d_descriptorsR, matches);

	int sz = matches.size();
	double max_dist = 0; double min_dist = 100;

	for (int i = 0; i < sz; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	cout << "\n-- Max dist : " << max_dist << endl;
	cout << "\n-- Min dist : " << min_dist << endl;

	for (int i = 0; i < sz; i++)
	{
		if (matches[i].distance < 0.6*max_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}
	//��ȡ����ƥ�������ڴ���ͼƬ�ϵĵ㼯��ȷ��ƥ��Ĵ��λ��  
	for (size_t i = 0; i < good_matches.size(); ++i)
	{
		scene.push_back(keyPoints_2[good_matches[i].trainIdx].pt);
	}

	for (unsigned int j = 0; j < scene.size(); j++)
		cv::circle(img_2, scene[j], 2, cv::Scalar(0, 255, 0), 2);
	//������ͨƥ����  
	Mat ShowMatches;
	drawMatches(img_1, keyPoints_1, img_2, keyPoints_2, matches, ShowMatches);
	imshow("matches", ShowMatches);
	imwrite("matches.png", ShowMatches);
	//��������ƥ����  
	Mat ShowGoodMatches;
	drawMatches(img_1, keyPoints_1, img_2, keyPoints_2, good_matches, ShowGoodMatches);
	imshow("good_matches", ShowGoodMatches);
	imwrite("good_matches.png", ShowGoodMatches);
	//��������ƥ�������ڴ���ͼƬ�ϵĵ㼯  
	imshow("MatchPoints_in_img_2", img_2);
	imwrite("MatchPoints_in_img_2.png", img_2);
	waitKey(0);
}

void func3()
{
	//ֱ�Ӽ���ģ��ͼ��Ϊ�Ҷ�ͼ��
	Mat img = imread("d:/img/phone1.bmp",CV_LOAD_IMAGE_GRAYSCALE);

	vector<KeyPoint> keypoint1, keypoint2;

	Mat describe1, describe2;

	Ptr<ORB> _orb = ORB::create();
	//���ùؼ�����ƥ�䷽ʽΪNORM_L2��������ʹ�� FLANNBASED = 1, BRUTEFORCE = 2, BRUTEFORCE_L1 = 3, 
	//BRUTEFORCE_HAMMING = 4, BRUTEFORCE_HAMMINGLUT = 5, BRUTEFORCE_SL2 = 6   
	Ptr<DescriptorMatcher> _descriptor = DescriptorMatcher::create(NORM_L2);
	_orb->detectAndCompute(img, Mat(), keypoint1, describe1);

	Mat iframe, gray;
	VideoCapture cap(0);
	vector<DMatch> matchs;
	vector<vector<DMatch>> _matchs;
	while (true)
	{
		cap >> iframe;
		cvtColor(iframe, gray, CV_BGR2GRAY);
		_orb->detectAndCompute(gray, Mat(), keypoint2, describe2);
		if (keypoint2.size() < 10)
		{
			imshow("matches", gray);
			continue;
		}
		//_descriptor->match(describe1, describe2, matchs);
		//int matchsSize = matchs.size();
		//double maxScore = 0, minScore = 100;
		//for (size_t i = 0; i < matchsSize; i++)
		//{
		//	double score = matchs[i].distance;
		//	if (score > maxScore)maxScore = score;
		//	if (score < minScore)minScore = score;
		//}
		//cout << "��Сֵ" << minScore;
		//cout << "���ֵ" << maxScore << endl;
		//vector<DMatch> goodmatch;
		//for (size_t i = 0; i < matchsSize; i++)
		//{
		//	if (matchs[i].distance < maxScore*0.6&&matchs[i].distance<50)
		//		goodmatch.push_back(matchs[i]);
		//}
		////������ͨƥ����  
		//Mat ShowMatches;
		//drawMatches(img, keypoint1, gray, keypoint2, goodmatch, ShowMatches);
		//imshow("matches", ShowMatches);

		//knnƥ��
		vector<DMatch> kmatches;
		_descriptor->knnMatch(describe1, describe2, _matchs, 2);
		const float minRatio = 1.f / 1.5f;
		for (size_t i = 0; i < _matchs.size(); i++) {
			const DMatch& bestMatch = _matchs[i][0];
			const DMatch& betterMatch = _matchs[i][1];
			float  distanceRatio = bestMatch.distance / betterMatch.distance;
			if (distanceRatio < minRatio)
				kmatches.push_back(bestMatch);
		}
		//������ͨƥ����  
		Mat ShowMatches;
		try {
			drawMatches(img, keypoint1, gray, keypoint2, kmatches, ShowMatches);
			imshow("matches", ShowMatches);
		}
		catch (exception ex) {}
		

		waitKey(2);
	}
}

void func4()
{
	//ֱ�Ӽ���ģ��ͼ��Ϊ�Ҷ�ͼ��
	Mat img = imread("d:/img/phone1.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	vector<KeyPoint> keypoint1, keypoint2;
	Mat describe1, describe2;
	Ptr<ORB> _orb = ORB::create();
	//���ùؼ�����ƥ�䷽ʽΪNORM_L2��������ʹ�� FLANNBASED = 1, BRUTEFORCE = 2, BRUTEFORCE_L1 = 3, 
	//BRUTEFORCE_HAMMING = 4, BRUTEFORCE_HAMMINGLUT = 5, BRUTEFORCE_SL2 = 6   
	Ptr<DescriptorMatcher> _descriptor = DescriptorMatcher::create(NORM_L2);
	_orb->detectAndCompute(img, Mat(), keypoint1, describe1);
	Mat iframe, gray;
	VideoCapture cap(0);
	vector<DMatch> matchs;
	vector<vector<DMatch>> _matchs;
	while (true)
	{
		cap >> iframe;
		cvtColor(iframe, gray, CV_BGR2GRAY);
		_orb->detectAndCompute(gray, Mat(), keypoint2, describe2);
		if (keypoint2.size() < 10)
		{
			imshow("matches", gray);
			continue;
		}
		//ƥ��
		Mat ShowMatches;
		drawKeypoints(gray, keypoint2, ShowMatches);
		imshow("matches", ShowMatches);
		waitKey(2);
	}
}