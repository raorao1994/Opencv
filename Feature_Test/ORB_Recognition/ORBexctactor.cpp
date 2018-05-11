#include "stdafx.h"
#include <iostream>     // cout
#include <algorithm>    // copy
#include <vector>       // vector
#include <iterator>     // back_inserter
#include "ORBexctactor.h"

using namespace std;
using namespace cv;

#define HALF_PATCH_SIZE 16
#define PATCH_SIZE 16
#define EDGE_THRESHOLD 5
static void computeOrientation(const Mat& image, vector<KeyPoint>& keypoints, const vector<int>& umax);
static float IC_Angle(const Mat& image, Point2f pt, const vector<int> & u_max);
static void computeOrbDescriptor(const KeyPoint& kpt, const Mat& img, const Point* pattern, uchar* desc);

ORBexctactor::ORBexctactor(int feature_num, float scale_factor, int levels_num, int default_fast_threshold, int min_fast_threshold)
{
	//1、在构造函数中，首先先初始化每层的尺度因子待用！
	// 这边将每层金字塔对应的尺度因子给出
	vec_scale_factor_.resize(levels_num_);
	vec_scale_factor_[0] = 1.0f;
	for (int i = 1; i < levels_num_; i++)
	{
		vec_scale_factor_[i] = vec_scale_factor_[i - 1] * scale_factor_;
	}

	//2、这边去掉了作者初始化构建尺度因子的倒数，平方，平方的倒数，因为后期调用不多，显得冗余去掉了。
	//接下来给每层分配待提取的特征数，具体通过等比数列求和的方式，求出每一层应该提取的特征数
	feature_num_per_level_.resize(levels_num_);
	float factor = 1.0f / scale_factor_;
	float desired_features_per_scale = features_num_*(1 - factor) /
		(1 - (float)pow((double)factor, (double)levels_num_));
	int sum_features = 0;
	for (int level = 0; level < levels_num_ - 1; level++)
	{
		feature_num_per_level_[level] = cvRound(desired_features_per_scale);
		sum_features += feature_num_per_level_[level];
		desired_features_per_scale *= factor;
	}
	feature_num_per_level_[levels_num_ - 1] = max(features_num_ - sum_features, 0);

	//3、接下来做一些初始化，用于计算特征的方向和描述
	// 复制训练的模板
	const int npoints = 512;
	const Point* pattern0 = (const Point*)bit_pattern_31_;
	copy(pattern0, pattern0 + npoints, back_inserter(pattern_));

	//用于计算特征方向时，每个v坐标对应最大的u坐标
	umax_.resize(HALF_PATCH_SIZE + 1);
	// 将v坐标划分为两部分进行计算，主要为了确保计算特征主方向的时候，x,y方向对称
	int v, v0, vmax = cvFloor(HALF_PATCH_SIZE * sqrt(2.f) / 2 + 1);
	int vmin = cvCeil(HALF_PATCH_SIZE * sqrt(2.f) / 2);
	// 通过勾股定理计算
	const double hp2 = HALF_PATCH_SIZE*HALF_PATCH_SIZE;
	for (v = 0; v <= vmax; ++v)
		umax_[v] = cvRound(sqrt(hp2 - v * v));

	// 确保对称，即保证是一个圆
	for (v = HALF_PATCH_SIZE, v0 = 0; v >= vmin; --v)
	{
		while (umax_[v0] == umax_[v0 + 1])
			++v0;
		umax_[v] = v0;
		++v0;
	}
}

ORBexctactor::~ORBexctactor()
{
}

void ORBexctactor::operator()(InputArray image, InputArray mask, vector<KeyPoint>& keypoints, OutputArray descriptors)
{
}

void ORBexctactor::computePyramid(Mat image)
{
}

void ORBexctactor::computeKeyPointsQuadTree(vector<vector<KeyPoint>>& all_keypoints)
{
	all_keypoints.resize(levels_num_);
	// 设置格子大小
	const float border_width = 30;

	for (int level = 0; level < levels_num_; ++level)
	{
		// 得到每一层图像进行特征检测区域上下两个坐标
		const int min_border_x = EDGE_THRESHOLD - 3;
		const int min_border_y = min_border_x;
		const int max_border_x = vec_image_pyramid_[level].cols - EDGE_THRESHOLD + 3;
		const int max_border_y = vec_image_pyramid_[level].rows - EDGE_THRESHOLD + 3;
		// 用于分配的关键点
		vector<KeyPoint> vec_to_distribute_keys;
		vec_to_distribute_keys.reserve(features_num_ * 10);

		const float width = (max_border_x - min_border_x);
		const float height = (max_border_y - min_border_y);
		// 将待检测区域划分为格子的行列数
		const int cols = width / border_width;
		const int rows = height / border_width;
		// 重新计算每个格子的大小
		const int width_cell = ceil(width / cols);
		const int height_cell = ceil(height / rows);
		// 在每个格子内进行fast特征检测
		for (int i = 0; i < rows; i++)
		{
			const float ini_y = min_border_y + i*height_cell;
			float max_y = ini_y + height_cell + 6;

			if (ini_y >= max_border_y - 3)
				continue;
			if (max_y > max_border_y)
				max_y = max_border_y;

			for (int j = 0; j < cols; j++)
			{
				const float ini_x = min_border_x + j*width_cell;
				float max_x = ini_x + width_cell + 6;
				if (ini_x >= max_border_x - 6)
					continue;
				if (max_x > max_border_x)
					max_x = max_border_x;

				vector<KeyPoint> vec_keys_cell;
				FAST(vec_image_pyramid_[level].rowRange(ini_y, max_y).colRange(ini_x, max_x),
					vec_keys_cell, default_fast_threshold_, true);
				// 如果检测到的fast特征为空，则降低阈值再进行检测
				if (vec_keys_cell.empty())
				{
					FAST(vec_image_pyramid_[level].rowRange(ini_y, max_y).colRange(ini_x, max_x),
						vec_keys_cell, min_fast_threshold_, true);
				}
				// 计算实际特征点的位置
				if (!vec_keys_cell.empty())
				{
					for (vector<KeyPoint>::iterator vit = vec_keys_cell.begin(); vit != vec_keys_cell.end(); vit++)
					{
						(*vit).pt.x += j*width_cell;
						(*vit).pt.y += i*height_cell;
						vec_to_distribute_keys.push_back(*vit);
					}
				}

			}
		}

		vector<KeyPoint> & keypoints = all_keypoints[level];
		keypoints.reserve(features_num_);
		// 将特征点进行四叉树划分
		keypoints = distributeQuadTree(vec_to_distribute_keys, min_border_x, max_border_x,
			min_border_y, max_border_y, feature_num_per_level_[level], level);

		const int scaled_patch_size = PATCH_SIZE*vec_scale_factor_[level];

		// 换算特征点真实位置（添加边界值），添加特征点的尺度信息
		const int nkps = keypoints.size();
		for (int i = 0; i < nkps; i++)
		{
			keypoints[i].pt.x += min_border_x;
			keypoints[i].pt.y += min_border_y;
			keypoints[i].octave = level;
			keypoints[i].size = scaled_patch_size;
		}
	}

	// 计算特征点的方向
	for (int level = 0; level < levels_num_; ++level)
		computeOrientation(vec_image_pyramid_[level], all_keypoints[level], umax_);
}

vector<KeyPoint> ORBexctactor::distributeQuadTree(const vector<KeyPoint>& vec_to_distribute_keys, const int & min_x, const int & max_x, const int & min_y, const int & max_y, const int & feature_num, const int & level)
{
	//// 计算初始时有几个节点
	//const int init_node_num = round(static_cast<float>(max_x - min_x) / (max_y - min_y));
	//// 得到节点之间的间隔
	//const float interval_x = static_cast<float>(max_x - min_x) / init_node_num;

	//vector<ExtractorNode*> init_nodes;
	//init_nodes.resize(init_node_num);
	//// 划分之后包含的节点
	//list<ExtractorNode> list_nodes;
	//for (int i = 0; i < init_node_num; i++)
	//{
	//	ExtractorNode ni;
	//	ni.UL_ = Point2i(interval_x*static_cast<float>(i), 0);
	//	ni.UR_ = Point2i(interval_x*static_cast<float>(i + 1), 0);
	//	ni.BL_ = Point2i(ni.UL_.x, max_y - min_y);
	//	ni.BR_ = Point2i(ni.UR_.x, max_y - min_y);
	//	ni.vec_keys_.reserve(vec_to_distribute_keys.size());

	//	list_nodes.push_back(ni);
	//	init_nodes[i] = &list_nodes.back();
	//}

	////将点分配给子节点
	//for (size_t i = 0; i < vec_to_distribute_keys.size(); i++)
	//{
	//	const KeyPoint &kp = vec_to_distribute_keys[i];
	//	init_nodes[kp.pt.x / interval_x]->vec_keys_.push_back(kp);
	//}

	//list<ExtractorNode>::iterator lit = list_nodes.begin();

	//while (lit != list_nodes.end())
	//{
	//	// 如果只含一个特征点的时候，则不再划分
	//	if (lit->vec_keys_.size() == 1)
	//	{
	//		lit->is_no_more_ = true;
	//		lit++;
	//	}
	//	else if (lit->vec_keys_.empty())
	//		lit = list_nodes.erase(lit);
	//	else
	//		lit++;
	//}

	//bool is_finish = false;

	//int iteration = 0;

	//vector<pair<int, ExtractorNode*> > keys_size_and_node;//节点及对应包含的特征数
	//keys_size_and_node.reserve(list_nodes.size() * 4);

	//while (!is_finish)
	//{
	//	iteration++;
	//	// 初始节点个数，用于判断是否节点再一次进行了划分
	//	int prev_size = list_nodes.size();

	//	lit = list_nodes.begin();
	//	// 表示节点分解次数
	//	int to_expand_num = 0;

	//	keys_size_and_node.clear();

	//	while (lit != list_nodes.end())
	//	{
	//		if (lit->is_no_more_)
	//		{
	//			// 表面只有一个特征点，则不再划分
	//			lit++;
	//			continue;
	//		}
	//		else
	//		{
	//			// 如果超过一个特征点，则继续划分
	//			ExtractorNode n1, n2, n3, n4;
	//			lit->divideNode(n1, n2, n3, n4);

	//			// 对划分之后的节点进行判断，是否含有特征点，含有特征点则添加节点
	//			if (n1.vec_keys_.size() > 0)
	//			{
	//				list_nodes.push_front(n1);
	//				if (n1.vec_keys_.size() > 1)
	//				{
	//					to_expand_num++;
	//					keys_size_and_node.push_back(make_pair(n1.vec_keys_.size(), &list_nodes.front()));
	//					list_nodes.front().node_iter_ = list_nodes.begin();
	//				}
	//			}
	//			if (n2.vec_keys_.size() > 0)
	//			{
	//				list_nodes.push_front(n2);
	//				if (n2.vec_keys_.size() > 1)
	//				{
	//					to_expand_num++;
	//					keys_size_and_node.push_back(make_pair(n2.vec_keys_.size(), &list_nodes.front()));
	//					list_nodes.front().node_iter_ = list_nodes.begin();
	//				}
	//			}
	//			if (n3.vec_keys_.size() > 0)
	//			{
	//				list_nodes.push_front(n3);
	//				if (n3.vec_keys_.size() > 1)
	//				{
	//					to_expand_num++;
	//					keys_size_and_node.push_back(make_pair(n3.vec_keys_.size(), &list_nodes.front()));
	//					list_nodes.front().node_iter_ = list_nodes.begin();
	//				}
	//			}
	//			if (n4.vec_keys_.size() > 0)
	//			{
	//				list_nodes.push_front(n4);
	//				if (n4.vec_keys_.size() > 1)
	//				{
	//					to_expand_num++;
	//					keys_size_and_node.push_back(make_pair(n4.vec_keys_.size(), &list_nodes.front()));
	//					list_nodes.front().node_iter_ = list_nodes.begin();
	//				}
	//			}

	//			lit = list_nodes.erase(lit);
	//			continue;
	//		}
	//	}

	//	// 当节点个数大于需分配的特征数或者所有的节点只有一个特征点（节点不能划分）的时候，则结束。
	//	if ((int)list_nodes.size() >= feature_num || (int)list_nodes.size() == prev_size)
	//	{
	//		is_finish = true;
	//	}
	//	else if (((int)list_nodes.size() + to_expand_num * 3) > feature_num)//节点展开次数乘以3用于表明下一次的节点分解可能超过特征数，即为最后一次分解
	//	{
	//		while (!is_finish)
	//		{
	//			prev_size = list_nodes.size();

	//			vector<pair<int, ExtractorNode*> > prev_size_and_node = keys_size_and_node;
	//			keys_size_and_node.clear();

	//			sort(prev_size_and_node.begin(), prev_size_and_node.end());
	//			for (int j = prev_size_and_node.size() - 1; j >= 0; j--)
	//			{
	//				ExtractorNode n1, n2, n3, n4;
	//				prev_size_and_node[j].second->divideNode(n1, n2, n3, n4);

	//				// 划分之后进一步的判断
	//				if (n1.vec_keys_.size() > 0)
	//				{
	//					list_nodes.push_front(n1);
	//					if (n1.vec_keys_.size() > 1)
	//					{
	//						keys_size_and_node.push_back(make_pair(n1.vec_keys_.size(), &list_nodes.front()));
	//						list_nodes.front().node_iter_ = list_nodes.begin();
	//					}
	//				}
	//				if (n2.vec_keys_.size() > 0)
	//				{
	//					list_nodes.push_front(n2);
	//					if (n2.vec_keys_.size() > 1)
	//					{
	//						keys_size_and_node.push_back(make_pair(n2.vec_keys_.size(), &list_nodes.front()));
	//						list_nodes.front().node_iter_ = list_nodes.begin();
	//					}
	//				}
	//				if (n3.vec_keys_.size() > 0)
	//				{
	//					list_nodes.push_front(n3);
	//					if (n3.vec_keys_.size() > 1)
	//					{
	//						keys_size_and_node.push_back(make_pair(n3.vec_keys_.size(), &list_nodes.front()));
	//						list_nodes.front().node_iter_ = list_nodes.begin();
	//					}
	//				}
	//				if (n4.vec_keys_.size() > 0)
	//				{
	//					list_nodes.push_front(n4);
	//					if (n4.vec_keys_.size() > 1)
	//					{
	//						keys_size_and_node.push_back(make_pair(n4.vec_keys_.size(), &list_nodes.front()));
	//						list_nodes.front().node_iter_ = list_nodes.begin();
	//					}
	//				}

	//				list_nodes.erase(prev_size_and_node[j].second->node_iter_);

	//				if ((int)list_nodes.size() >= feature_num)
	//					break;
	//			}

	//			if ((int)list_nodes.size() >= feature_num || (int)list_nodes.size() == prev_size)
	//				is_finish = true;

	//		}
	//	}
	//}

	//// 保留每个节点下最好的特征点
	//vector<KeyPoint> result_keys;
	//result_keys.reserve(features_num_);
	//for (list<ExtractorNode>::iterator lit = list_nodes.begin(); lit != list_nodes.end(); lit++)
	//{
	//	vector<KeyPoint> &node_keys = lit->vec_keys_;
	//	KeyPoint* keypoint = &node_keys[0];
	//	float max_response = keypoint->response;

	//	for (size_t k = 1; k < node_keys.size(); k++)
	//	{
	//		if (node_keys[k].response > max_response)
	//		{
	//			keypoint = &node_keys[k];
	//			max_response = node_keys[k].response;
	//		}
	//	}

	//	result_keys.push_back(*keypoint);
	//}

	//return result_keys;

	vector<KeyPoint> result_keys;
	return result_keys;
}

//特征检测好了之后，计算特征的方向，计算特征方向是为了保证特征具有旋转不变的特性。
static void computeOrientation(const Mat& image, vector<KeyPoint>& keypoints, const vector<int>& umax)
{
	for (vector<KeyPoint>::iterator keypoint = keypoints.begin(),
		keypoint_end = keypoints.end(); keypoint != keypoint_end; ++keypoint)
	{
		keypoint->angle = IC_Angle(image, keypoint->pt, umax);
	}
}
// 灰度质心法计算特征点方向
static float IC_Angle(const Mat& image, Point2f pt, const vector<int> & u_max)
{
	int m_01 = 0, m_10 = 0;
	// 得到中心位置
	const uchar* center = &image.at<uchar>(cvRound(pt.y), cvRound(pt.x));

	// 对 v=0 这一行单独计算
	for (int u = -HALF_PATCH_SIZE; u <= HALF_PATCH_SIZE; ++u)
		m_10 += u * center[u];

	// 这边要注意图像的step不一定是图像的宽度
	int step = (int)image.step1();

	for (int v = 1; v <= HALF_PATCH_SIZE; ++v)
	{
		// 上下和左右两条线同时计算
		int v_sum = 0;
		int d = u_max[v];
		for (int u = -d; u <= d; ++u)
		{
			int val_plus = center[u + v*step], val_minus = center[u - v*step];
			v_sum += (val_plus - val_minus);//计算上下的时候是有符号的，所以这边是减
			m_10 += u * (val_plus + val_minus);//这边加是由于u已经确定好了符号
		}
		m_01 += v * v_sum;
	}

	return fastAtan2((float)m_01, (float)m_10);
}
//计算特征描述子
const float factor_pi = (float)(CV_PI / 180.f);
static void computeOrbDescriptor(const KeyPoint& kpt, const Mat& img, const Point* pattern, uchar* desc)
{
	float angle = (float)kpt.angle*factor_pi;
	float a = (float)cos(angle), b = (float)sin(angle);

	const uchar* center = &img.at<uchar>(cvRound(kpt.pt.y), cvRound(kpt.pt.x));
	const int step = (int)img.step;
#define GET_VALUE(idx) \
        center[cvRound(pattern[idx].x*b + pattern[idx].y*a)*step + \
               cvRound(pattern[idx].x*a - pattern[idx].y*b)]


	for (int i = 0; i < 32; ++i, pattern += 16)
	{
		int t0, t1, val;
		t0 = GET_VALUE(0); t1 = GET_VALUE(1);
		val = t0 < t1;
		t0 = GET_VALUE(2); t1 = GET_VALUE(3);
		val |= (t0 < t1) << 1;
		t0 = GET_VALUE(4); t1 = GET_VALUE(5);
		val |= (t0 < t1) << 2;
		t0 = GET_VALUE(6); t1 = GET_VALUE(7);
		val |= (t0 < t1) << 3;
		t0 = GET_VALUE(8); t1 = GET_VALUE(9);
		val |= (t0 < t1) << 4;
		t0 = GET_VALUE(10); t1 = GET_VALUE(11);
		val |= (t0 < t1) << 5;
		t0 = GET_VALUE(12); t1 = GET_VALUE(13);
		val |= (t0 < t1) << 6;
		t0 = GET_VALUE(14); t1 = GET_VALUE(15);
		val |= (t0 < t1) << 7;

		desc[i] = (uchar)val;
	}

#undef GET_VALUE
}
