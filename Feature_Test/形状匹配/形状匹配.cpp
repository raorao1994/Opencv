// 形状匹配.cpp : 定义控制台应用程序的入口点。
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

using namespace std;
using namespace cv;
/*
*Angle 角度计算
*cen 中间点
*first 起点
*second结点
*起点->中间点->结点
*/
double Angle(Point cen, Point first, Point second)
{

	const double M_PI = 3.1415926535897;
	double ma_x = first.x - cen.x;
	double ma_y = first.y - cen.y;
	double mb_x = second.x - cen.x;
	double mb_y = second.y - cen.y;
	double v1 = (ma_x * mb_x) + (ma_y * mb_y);
	double ma_val = sqrt(ma_x * ma_x + ma_y * ma_y);
	double mb_val = sqrt(mb_x * mb_x + mb_y * mb_y);
	double cosM = v1 / (ma_val * mb_val);
	double acosV = acos(cosM);
	double angleAMB = acosV * 180.0 / M_PI;

	//计算是否为大于180度的角
	if (cen.x == first.x)
	{
		//cen和first两点构成的直线垂直于y轴
		if (second.x > cen.x)
			return (360 - angleAMB);
		else
			return angleAMB;
	}
	else
	{
		double k = (cen.y - first.y) / (cen.x - first.x);
		double b = first.y - first.x*k;
		double _y = second.x*k + b;
		if (second.y >= _y)
			return angleAMB;
		else
			return (360 - angleAMB);
	}
}
//计算边缘的特征
vector<int> edge(vector<Point> edge)
{
	vector<Point> poly;
	//对图像轮廓点进行多边形拟合
	approxPolyDP(Mat(edge), poly, 1, true);//根据点集，拟合出多边形
	vector<int> orients;
	for (size_t i = 1; i < poly.size()-1; i++)
	{
		double direction =Angle(poly[i], poly[i - 1], poly[i + 1]);
		//从0，45，90，135得到最近的角度
		/*if ((direction>0 && direction < 22.5) || (direction >157.5 && direction < 202.5) || (direction>337.5 && direction<360))
			direction = 0;
		else if ((direction>22.5 && direction < 67.5) || (direction >202.5 && direction <247.5))
			direction = 45;
		else if ((direction >67.5 && direction < 112.5) || (direction>247.5 && direction<292.5))
			direction = 90;
		else if ((direction >112.5 && direction < 157.5) || (direction>292.5 && direction<337.5))
			direction = 135;
		else
			direction = 0;*/

		orients.push_back((int)direction);
	}
	return orients;
}
//特征比较
bool islike(vector<int> temp, vector<int> edge)
{
	int tempSize = temp.size();
	//判断模版边缘和待检测边缘长度是否一致
	if (edge.size() - tempSize > 0)
	{
		for (size_t i = 0; i < edge.size()- tempSize; i++)
		{
			int isok = 0;
			int index = 0;
			for (size_t j = i; j < tempSize; j++)
			{
				if (abs(edge[j] -temp[index])<15)
					isok++;
				index++;
			}
			if (isok > tempSize*0.6)
				return true;
		}
	}
	else if(abs(edge.size() - tempSize) < tempSize*0.1)
	{
		for (size_t i = 0; i < tempSize-edge.size(); i++)
		{
			int isok = 0;
			int index = 0;
			for (size_t j = i; j < edge.size(); j++)
			{
				//if (temp[j] == edge[index])
				if (abs(edge[j] - temp[index])<15)
					isok++;
				index++;
			}
			if (isok > tempSize*0.6)
				return true;
		}
	}
	return false;
}

int main()
{
	//1.查找模版图像的轮廓  
	Mat templateImg = imread("E:/TablewareRecognition/碗/单碗/8/1.jpg", CV_LOAD_IMAGE_COLOR);
	Mat copyImg1 = templateImg.clone();
	cvtColor(templateImg, templateImg, CV_BGR2GRAY);
	threshold(templateImg, templateImg, 100, 255, CV_THRESH_BINARY);//确保黑中找白  
	vector<vector<Point>> contours1;
	findContours(templateImg, contours1, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//最外层轮廓  
	vector<int> tempint;
	for (size_t i = 0; i < contours1.size(); i++)
	{
		double area = abs(contourArea(contours1[i]));
		if (area > 3600)
		{
			drawContours(copyImg1, contours1, i, Scalar(100, 255, 100), 4, 8);
			tempint = edge(contours1[0]);
		}
	}
	//2.查找待测试图像的轮廓
	string path = "E:/TablewareRecognition/碗/多碗-1/7.jpg";
	Mat testImg = imread(path, CV_LOAD_IMAGE_COLOR);
	Mat copyImg2 = testImg.clone();
	cvtColor(testImg, testImg, CV_BGR2GRAY);
	threshold(testImg, testImg, 100, 255, CV_THRESH_BINARY);//确保黑中找白  
	vector<vector<Point>> contours2;
	findContours(testImg, contours2, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//最外层轮廓 
	//drawContours(copyImg2, contours2, -1, Scalar(100, 255, 100), 4, 8);
	//3.形状匹配---比较两个形状或轮廓间的相似度  
	//遍历待测试图像的轮廓
	for (int i = 0; i < contours2.size(); i++)
	{
		double area=abs(contourArea(contours2[i]));
		if (area > 3600)
		{
			//vector<Point> poly;
			//approxPolyDP(Mat(contours2[i]), poly, 1, true);//根据点集，拟合出多边形
			//vector<vector<Point>>  pp; pp.push_back(poly);
			//drawContours(copyImg2, pp, -1, Scalar(100, 255, 100), 4, 8);

			vector<int> edgeint = edge(contours2[i]);
			bool b = islike(tempint, edgeint);
			if(b)
				drawContours(copyImg2, contours2, i, Scalar(100, 255, 100), 4, 8);
			cout << "相似：" << b << endl;
		}
	}
	imshow("模版", copyImg1);
	imshow("查询", copyImg2);
	waitKey(0);
	return 0;
}

