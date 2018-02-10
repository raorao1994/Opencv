// 00GetAllFile.cpp : �������̨Ӧ�ó������ڵ㡣
// ��ȡ�ļ�Ŀ¼�µ�������Ŀ¼���ļ���

#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include<io.h>

using namespace std;

void getFiles(string path, vector<string>& files)
{
	//�ļ����  
	long   hFile = 0;
	//�ļ���Ϣ  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�����Ŀ¼,����֮  
			//�������,�����б�  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

int main()
{
	string DATA_DIR = "E://������";
	vector<string> files;
	//����
	char * DistAll = "AllFiles.txt";
	getFiles(DATA_DIR, files);//�����ļ����ļ��е�·�������
	ofstream ofn(DistAll);  //����ļ���
	int size = files.size();
	int  FaiNum = 0;
	ofn << size << endl;
	for (int i = 0; i<size; i++)
	{
		ofn << files[i] << endl;
	}
	ofn.close();
	return 0;
    return 0;
}

