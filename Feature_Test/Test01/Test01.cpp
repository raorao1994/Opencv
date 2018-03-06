// Test01.cpp : �������̨Ӧ�ó������ڵ㡣
//��һ��������Ŀ

#include "stdafx.h"
#include "opencv2/core.hpp"    
#include "opencv2/core/utility.hpp"    
#include "opencv2/core/ocl.hpp"    
#include "opencv2/imgcodecs.hpp"    
#include "opencv2/highgui.hpp"    
#include "opencv2/features2d.hpp"    
#include "opencv2/calib3d.hpp"    
#include "opencv2/imgproc.hpp"    
#include "opencv2/flann.hpp"    
#include "opencv2/xfeatures2d.hpp"
#include <opencv2/xfeatures2d/nonfree.hpp>
#include "opencv2/ml.hpp"

using namespace cv;
using namespace std;
using namespace cv::xfeatures2d;
using namespace cv::ml;
int main()
{
	//��1�������ز�ͼ
	//���������ͼ����Դ��MiddleBuryͼƬ��
	Mat a = imread("D:/9.jpg", 0);
	Mat b = imread("D:/12.jpg", 0);
	//������ʽ��OpenCV2�еĲ�һ��,����Ҫ���������ռ�xfreatures2d
	//����ʹ���ú��˻�����ʾSURFΪδ�����ı�ʶ��
	//��2��ʹ��SURF���Ӽ��ؼ���
	Ptr<SURF> surf;
	surf = SURF::create(800);

	//ʵ����һ������ƥ����
	BFMatcher matcher;
	Mat c, d;
	vector<KeyPoint>key1, key2;
	//DMatch����������ƥ��õ�һ����������࣬������������֮��������Ϣ
	//������ͼ�и�����m��������ͼ��������n��ƥ�䣬���DMatch�ͼ�¼������ƥ�䣬���һ���¼m��n��
	//���������ľ����������Ϣ����������ں���������ɸѡ
	vector<DMatch> matches;
	
	//��3������detect��������SURF�����ؼ��㣬������vector������
	//��4������������������������
	//����ͼ���������룬���������㣬���Mat������������������������
	surf->detectAndCompute(a, Mat(), key1, c);
	//���Mat����Ϊ������ĸ���������Ϊÿ�����������ĳߴ磬SURF��64��ά
	surf->detectAndCompute(b, Mat(), key2, d);
	//ƥ�䣬������Դ��������������������DMatch��������
	//��5��ʹ��BruteForce����ƥ��
	matcher.match(c, d, matches);

	//sort���������ݽ�����������
	//ɸѡƥ��㣬����match���������Եľ����С��������
	sort(matches.begin(), matches.end());
	vector< DMatch > good_matches;
	int ptsPairs = std::min(50, (int)(matches.size() * 0.15));
	cout << ptsPairs << endl;
	for (int i = 0; i < ptsPairs; i++)
	{
		good_matches.push_back(matches[i]);//������С��50��ѹ���µ�DMatch
	}
	//��6�����ƴ�����ͼ����ƥ����Ĺؼ���
	Mat outimg; //drawMatches�������ֱ�ӻ�������һ���ͼ
	drawMatches(a, key1, b, key2, good_matches, outimg, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //����ƥ���    


	imshow("����", outimg);
	cvWaitKey(0);
    return 0;
}

