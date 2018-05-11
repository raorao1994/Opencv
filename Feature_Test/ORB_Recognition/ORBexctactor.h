#pragma once
#include "stdafx.h"
#include<vector>
#include<iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

using namespace std;
using namespace cv;

class ORBexctactor
{
public:
	//���캯��
	ORBexctactor(int feature_num=500,float scale_factor=1.2f,int levels_num=8,
		int default_fast_threshold=20,int min_fast_threshold=7);
	//��������
	~ORBexctactor();
	/** brief ����ͼ���orb��������������orb�������䵽һ���Ĳ�������
	*  Ŀǰmask�����Ǳ����Եģ�û��ʵ��
	*/
	void operator()(InputArray image, InputArray mask, vector<KeyPoint>& keypoints, OutputArray descriptors);

protected:
	//brief ����ͼ�������
	void computePyramid(Mat image);
	//ͨ���Ĳ����ķ�ʽ����������
	void computeKeyPointsQuadTree(vector<vector<KeyPoint>>& all_keypoints);
	//brief ͨ���Ĳ����ķ�ʽ����������
	vector<KeyPoint> distributeQuadTree(const vector<KeyPoint>& vec_to_distribute_keys, const int &min_x,
		const int &max_x, const int &min_y, const int &max_y, const int &feature_num, const int &level);


public:
	//ͼ�������
	vector<Mat> vec_image_pyramid_;
protected:
	//���ڴ��ѵ����ģ��
	vector<Point> pattern_;
	//�����ȡ�������������
	int features_num_;
	//������ͼ��֮��ĳ߶Ȳ���
	float scale_factor_;
	//��˹�������Ĳ���
	int levels_num_;
	//Ĭ������fast�ǵ���ֵ20
	int default_fast_threshold_;
	//����fast�ǵ���ֵΪ9
	int min_fast_threshold_;
	//ÿ�������ĸ���
	vector<int> feature_num_per_level_;
	//���ڴ洢������������ʱ��ͼ��ÿ��v�����Ӧ����u����
	vector<int> umax_;
	//���ڴ洢ÿ��ĳ߶�����
	vector<float> vec_scale_factor_;
	//?
	int bit_pattern_31_;
};

