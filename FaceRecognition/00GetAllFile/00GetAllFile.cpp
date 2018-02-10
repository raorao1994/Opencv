// 00GetAllFile.cpp : 定义控制台应用程序的入口点。
// 获取文件目录下的所有子目录和文件名

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
	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			//如果不是,加入列表  
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
	string DATA_DIR = "E://电子书";
	vector<string> files;
	//测试
	char * DistAll = "AllFiles.txt";
	getFiles(DATA_DIR, files);//所有文件与文件夹的路径都输出
	ofstream ofn(DistAll);  //输出文件流
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

