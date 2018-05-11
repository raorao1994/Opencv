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
	//构造函数
	ORBexctactor(int feature_num=500,float scale_factor=1.2f,int levels_num=8,
		int default_fast_threshold=20,int min_fast_threshold=7);
	//析构函数
	~ORBexctactor();
	/** brief 计算图像的orb特征及描述，将orb特征分配到一个四叉树当中
	*  目前mask参数是被忽略的，没有实现
	*/
	void operator()(InputArray image, InputArray mask, vector<KeyPoint>& keypoints, OutputArray descriptors);

protected:
	//brief 计算图像金字塔
	void computePyramid(Mat image);
	//通过四叉树的方式分配特征点
	void computeKeyPointsQuadTree(vector<vector<KeyPoint>>& all_keypoints);
	//brief 通过四叉树的方式分配特征点
	vector<KeyPoint> distributeQuadTree(const vector<KeyPoint>& vec_to_distribute_keys, const int &min_x,
		const int &max_x, const int &min_y, const int &max_y, const int &feature_num, const int &level);


public:
	//图像金字塔
	vector<Mat> vec_image_pyramid_;
protected:
	//用于存放训练的模版
	vector<Point> pattern_;
	//最多提取的特征点的数量
	int features_num_;
	//金字塔图像之间的尺度参数
	float scale_factor_;
	//高斯金字塔的层数
	int levels_num_;
	//默认设置fast角点阈值20
	int default_fast_threshold_;
	//设置fast角点阈值为9
	int min_fast_threshold_;
	//每层特征的个数
	vector<int> feature_num_per_level_;
	//用于存储计算特征方向时，图像每个v坐标对应最大的u坐标
	vector<int> umax_;
	//用于存储每层的尺度因子
	vector<float> vec_scale_factor_;
	//?
	int bit_pattern_31_;
};

