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
	//1���ڹ��캯���У������ȳ�ʼ��ÿ��ĳ߶����Ӵ��ã�
	// ��߽�ÿ���������Ӧ�ĳ߶����Ӹ���
	vec_scale_factor_.resize(levels_num_);
	vec_scale_factor_[0] = 1.0f;
	for (int i = 1; i < levels_num_; i++)
	{
		vec_scale_factor_[i] = vec_scale_factor_[i - 1] * scale_factor_;
	}

	//2�����ȥ�������߳�ʼ�������߶����ӵĵ�����ƽ����ƽ���ĵ�������Ϊ���ڵ��ò��࣬�Ե�����ȥ���ˡ�
	//��������ÿ��������ȡ��������������ͨ���ȱ�������͵ķ�ʽ�����ÿһ��Ӧ����ȡ��������
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

	//3����������һЩ��ʼ�������ڼ��������ķ��������
	// ����ѵ����ģ��
	const int npoints = 512;
	const Point* pattern0 = (const Point*)bit_pattern_31_;
	copy(pattern0, pattern0 + npoints, back_inserter(pattern_));

	//���ڼ�����������ʱ��ÿ��v�����Ӧ����u����
	umax_.resize(HALF_PATCH_SIZE + 1);
	// ��v���껮��Ϊ�����ֽ��м��㣬��ҪΪ��ȷ�����������������ʱ��x,y����Գ�
	int v, v0, vmax = cvFloor(HALF_PATCH_SIZE * sqrt(2.f) / 2 + 1);
	int vmin = cvCeil(HALF_PATCH_SIZE * sqrt(2.f) / 2);
	// ͨ�����ɶ������
	const double hp2 = HALF_PATCH_SIZE*HALF_PATCH_SIZE;
	for (v = 0; v <= vmax; ++v)
		umax_[v] = cvRound(sqrt(hp2 - v * v));

	// ȷ���Գƣ�����֤��һ��Բ
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
	// ���ø��Ӵ�С
	const float border_width = 30;

	for (int level = 0; level < levels_num_; ++level)
	{
		// �õ�ÿһ��ͼ����������������������������
		const int min_border_x = EDGE_THRESHOLD - 3;
		const int min_border_y = min_border_x;
		const int max_border_x = vec_image_pyramid_[level].cols - EDGE_THRESHOLD + 3;
		const int max_border_y = vec_image_pyramid_[level].rows - EDGE_THRESHOLD + 3;
		// ���ڷ���Ĺؼ���
		vector<KeyPoint> vec_to_distribute_keys;
		vec_to_distribute_keys.reserve(features_num_ * 10);

		const float width = (max_border_x - min_border_x);
		const float height = (max_border_y - min_border_y);
		// ����������򻮷�Ϊ���ӵ�������
		const int cols = width / border_width;
		const int rows = height / border_width;
		// ���¼���ÿ�����ӵĴ�С
		const int width_cell = ceil(width / cols);
		const int height_cell = ceil(height / rows);
		// ��ÿ�������ڽ���fast�������
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
				// �����⵽��fast����Ϊ�գ��򽵵���ֵ�ٽ��м��
				if (vec_keys_cell.empty())
				{
					FAST(vec_image_pyramid_[level].rowRange(ini_y, max_y).colRange(ini_x, max_x),
						vec_keys_cell, min_fast_threshold_, true);
				}
				// ����ʵ���������λ��
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
		// ������������Ĳ�������
		keypoints = distributeQuadTree(vec_to_distribute_keys, min_border_x, max_border_x,
			min_border_y, max_border_y, feature_num_per_level_[level], level);

		const int scaled_patch_size = PATCH_SIZE*vec_scale_factor_[level];

		// ������������ʵλ�ã���ӱ߽�ֵ�������������ĳ߶���Ϣ
		const int nkps = keypoints.size();
		for (int i = 0; i < nkps; i++)
		{
			keypoints[i].pt.x += min_border_x;
			keypoints[i].pt.y += min_border_y;
			keypoints[i].octave = level;
			keypoints[i].size = scaled_patch_size;
		}
	}

	// ����������ķ���
	for (int level = 0; level < levels_num_; ++level)
		computeOrientation(vec_image_pyramid_[level], all_keypoints[level], umax_);
}

vector<KeyPoint> ORBexctactor::distributeQuadTree(const vector<KeyPoint>& vec_to_distribute_keys, const int & min_x, const int & max_x, const int & min_y, const int & max_y, const int & feature_num, const int & level)
{
	//// �����ʼʱ�м����ڵ�
	//const int init_node_num = round(static_cast<float>(max_x - min_x) / (max_y - min_y));
	//// �õ��ڵ�֮��ļ��
	//const float interval_x = static_cast<float>(max_x - min_x) / init_node_num;

	//vector<ExtractorNode*> init_nodes;
	//init_nodes.resize(init_node_num);
	//// ����֮������Ľڵ�
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

	////���������ӽڵ�
	//for (size_t i = 0; i < vec_to_distribute_keys.size(); i++)
	//{
	//	const KeyPoint &kp = vec_to_distribute_keys[i];
	//	init_nodes[kp.pt.x / interval_x]->vec_keys_.push_back(kp);
	//}

	//list<ExtractorNode>::iterator lit = list_nodes.begin();

	//while (lit != list_nodes.end())
	//{
	//	// ���ֻ��һ���������ʱ�����ٻ���
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

	//vector<pair<int, ExtractorNode*> > keys_size_and_node;//�ڵ㼰��Ӧ������������
	//keys_size_and_node.reserve(list_nodes.size() * 4);

	//while (!is_finish)
	//{
	//	iteration++;
	//	// ��ʼ�ڵ�����������ж��Ƿ�ڵ���һ�ν����˻���
	//	int prev_size = list_nodes.size();

	//	lit = list_nodes.begin();
	//	// ��ʾ�ڵ�ֽ����
	//	int to_expand_num = 0;

	//	keys_size_and_node.clear();

	//	while (lit != list_nodes.end())
	//	{
	//		if (lit->is_no_more_)
	//		{
	//			// ����ֻ��һ�������㣬���ٻ���
	//			lit++;
	//			continue;
	//		}
	//		else
	//		{
	//			// �������һ�������㣬���������
	//			ExtractorNode n1, n2, n3, n4;
	//			lit->divideNode(n1, n2, n3, n4);

	//			// �Ի���֮��Ľڵ�����жϣ��Ƿ��������㣬��������������ӽڵ�
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

	//	// ���ڵ���������������������������еĽڵ�ֻ��һ�������㣨�ڵ㲻�ܻ��֣���ʱ���������
	//	if ((int)list_nodes.size() >= feature_num || (int)list_nodes.size() == prev_size)
	//	{
	//		is_finish = true;
	//	}
	//	else if (((int)list_nodes.size() + to_expand_num * 3) > feature_num)//�ڵ�չ����������3���ڱ�����һ�εĽڵ�ֽ���ܳ�������������Ϊ���һ�ηֽ�
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

	//				// ����֮���һ�����ж�
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

	//// ����ÿ���ڵ�����õ�������
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

//����������֮�󣬼��������ķ��򣬼�������������Ϊ�˱�֤����������ת��������ԡ�
static void computeOrientation(const Mat& image, vector<KeyPoint>& keypoints, const vector<int>& umax)
{
	for (vector<KeyPoint>::iterator keypoint = keypoints.begin(),
		keypoint_end = keypoints.end(); keypoint != keypoint_end; ++keypoint)
	{
		keypoint->angle = IC_Angle(image, keypoint->pt, umax);
	}
}
// �Ҷ����ķ����������㷽��
static float IC_Angle(const Mat& image, Point2f pt, const vector<int> & u_max)
{
	int m_01 = 0, m_10 = 0;
	// �õ�����λ��
	const uchar* center = &image.at<uchar>(cvRound(pt.y), cvRound(pt.x));

	// �� v=0 ��һ�е�������
	for (int u = -HALF_PATCH_SIZE; u <= HALF_PATCH_SIZE; ++u)
		m_10 += u * center[u];

	// ���Ҫע��ͼ���step��һ����ͼ��Ŀ��
	int step = (int)image.step1();

	for (int v = 1; v <= HALF_PATCH_SIZE; ++v)
	{
		// ���º�����������ͬʱ����
		int v_sum = 0;
		int d = u_max[v];
		for (int u = -d; u <= d; ++u)
		{
			int val_plus = center[u + v*step], val_minus = center[u - v*step];
			v_sum += (val_plus - val_minus);//�������µ�ʱ�����з��ŵģ���������Ǽ�
			m_10 += u * (val_plus + val_minus);//��߼�������u�Ѿ�ȷ�����˷���
		}
		m_01 += v * v_sum;
	}

	return fastAtan2((float)m_01, (float)m_10);
}
//��������������
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
