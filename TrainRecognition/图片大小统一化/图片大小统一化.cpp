// ͼƬ��Сͳһ��.cpp : �������̨Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <fstream>
#include <io.h>

using namespace std;
using namespace cv;
//�������
vector<string> g_files;
string g_Path = "E:/TablewareRecognition/��/����-1";
int g_Width=512;

//���巽��
void ConvertSize(string path);
int main()
{
	//�����ļ���������ͼƬ
	intptr_t hFile = 0;
	struct  _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(g_Path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if (!(fileinfo.attrib&_A_SUBDIR))
			{
				string imgPath= p.assign(g_Path).append("\\").append(fileinfo.name);
				g_files.push_back(imgPath);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	//���������ļ���ת��ͼƬ��ʽ
	for (size_t i = 0; i < g_files.size(); i++)
	{
		ConvertSize(g_files[i]);
	}
	cout << "������ϣ�����" << endl;
	waitKey();
    return 0;
}
//ת��ͼƬ��С
void ConvertSize(string path)
{
	Mat img = imread(path);
	int w = img.size().width;
	int h = img.size().height;
	double scale = g_Width*1.0 / w;

	int nw = w*scale;
	int nh = h*scale;

	Mat _img;
	Size size = Size(nw,nh);
	resize(img, _img, size);
	img.release();
	imwrite(path, _img);
	cout << path << "\t ת���ɹ�" << endl;
}