// Test01.cpp : 定义控制台应用程序的入口点。
//第一个测试项目

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
	//【1】载入素材图
	//左右相机的图像来源于MiddleBury图片库
	Mat a = imread("D:/9.jpg", 0);
	Mat b = imread("D:/12.jpg", 0);
	//创建方式和OpenCV2中的不一样,并且要加上命名空间xfreatures2d
	//否则即使配置好了还是显示SURF为未声明的标识符
	//【2】使用SURF算子检测关键点
	Ptr<SURF> surf;
	surf = SURF::create(800);

	//实例化一个暴力匹配器
	BFMatcher matcher;
	Mat c, d;
	vector<KeyPoint>key1, key2;
	//DMatch是用来描述匹配好的一对特征点的类，包含这两个点之间的相关信息
	//比如左图有个特征m，它和右图的特征点n最匹配，这个DMatch就记录它俩最匹配，并且还记录m和n的
	//特征向量的距离和其他信息，这个距离在后面用来做筛选
	vector<DMatch> matches;
	
	//【3】调用detect函数检测出SURF特征关键点，保存在vector容器中
	//【4】计算描述符（特征向量）
	//输入图像，输入掩码，输入特征点，输出Mat，存放所有特征点的描述向量
	surf->detectAndCompute(a, Mat(), key1, c);
	//这个Mat行数为特征点的个数，列数为每个特征向量的尺寸，SURF是64（维
	surf->detectAndCompute(b, Mat(), key2, d);
	//匹配，数据来源是特征向量，结果存放在DMatch类型里面
	//【5】使用BruteForce进行匹配
	matcher.match(c, d, matches);

	//sort函数对数据进行升序排列
	//筛选匹配点，根据match里面特征对的距离从小到大排序
	sort(matches.begin(), matches.end());
	vector< DMatch > good_matches;
	int ptsPairs = std::min(50, (int)(matches.size() * 0.15));
	cout << ptsPairs << endl;
	for (int i = 0; i < ptsPairs; i++)
	{
		good_matches.push_back(matches[i]);//距离最小的50个压入新的DMatch
	}
	//【6】绘制从两个图像中匹配出的关键点
	Mat outimg; //drawMatches这个函数直接画出摆在一起的图
	drawMatches(a, key1, b, key2, good_matches, outimg, Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);  //绘制匹配点    


	imshow("桌面", outimg);
	cvWaitKey(0);
    return 0;
}

