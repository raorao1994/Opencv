// MultiChannelBlending.cpp : �������̨Ӧ�ó������ڵ㡣
//http://blog.csdn.net/poem_qianmo/article/details/21176257
//-----------------------------------������˵����----------------------------------------------  
//  ��������:����OpenCV���Ž̳�֮�ġ�������ɫͨ��&��ͨ��ͼ����   ����Դ��  
// VS2010��   OpenCV�汾��2.4.8  
//     2014��3��13 �� Create by ǳī  
//  ͼƬ�زĳ�����dota2ԭ�� dota2logo   
//     ǳī��΢����@ǳī_ë����  
//------------------------------------------------------------------------------------------------  

//-----------------------------------��ͷ�ļ��������֡�---------------------------------------  
//     ����������������������ͷ�ļ�  
//----------------------------------------------------------------------------------------------                                                                                      
#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <iostream>  

//-----------------------------------�������ռ��������֡�---------------------------------------  
//     ����������������ʹ�õ������ռ�  
//-----------------------------------------------------------------------------------------------    
using namespace cv;
using namespace std;

const char *path1 = "d:\\8.jpg";
const char *path2 = "d:\\9.jpg";
//-----------------------------------��ȫ�ֺ����������֡�--------------------------------------  
//     ������ȫ�ֺ�������  
//-----------------------------------------------------------------------------------------------  
bool MultiChannelBlending();

//-----------------------------------��main( )������--------------------------------------------  
//     ����������̨Ӧ�ó������ں��������ǵĳ�������￪ʼ  
//-----------------------------------------------------------------------------------------------  
int main()
{
	system("color5E");

	if (MultiChannelBlending())
	{
		cout << endl << "�š����ˣ��ó�������Ҫ�Ļ��ֵͼ��~";
	}

	waitKey(0);
	return 0;
}


//-----------------------------��MultiChannelBlending( )������--------------------------------  
//     ��������ͨ����ϵ�ʵ�ֺ���  
//-----------------------------------------------------------------------------------------------  
bool MultiChannelBlending()
{
	//��0��������ر���  
	Mat srcImage;
	Mat logoImage;
	vector<Mat>channels;
	Mat  imageBlueChannel;

	//=================����ɫͨ�����֡�=================  
	//     ��������ͨ�����-��ɫ��������  
	//============================================  

	//��1������ͼƬ  
	logoImage = imread(path2, 0);
	srcImage = imread(path1);

	if (!logoImage.data) { printf("Oh��no����ȡlogoImage����~��\n"); return false; }
	if (!srcImage.data) { printf("Oh��no����ȡsrcImage����~��\n"); return false; }

	//��2����һ��3ͨ��ͼ��ת����3����ͨ��ͼ��  
	split(srcImage, channels);//����ɫ��ͨ��  

							  //��3����ԭͼ����ɫͨ�����÷��ظ�imageBlueChannel��ע�������ã��൱�����ߵȼۣ��޸�����һ����һ�����ű�  
	imageBlueChannel = channels.at(0);
	//��4����ԭͼ����ɫͨ���ģ�500,250�����괦���·���һ�������logoͼ���м�Ȩ���������õ��Ļ�Ͻ���浽imageBlueChannel��  
	addWeighted(imageBlueChannel(Rect(0, 0, logoImage.cols, logoImage.rows)), 1.0,
		logoImage, 0.5, 0, imageBlueChannel(Rect(0, 0, logoImage.cols, logoImage.rows)));

	//��5����������ͨ�����ºϲ���һ����ͨ��  
	merge(channels, srcImage);

	//��6����ʾЧ��ͼ  
	namedWindow("<1>��Ϸԭ��+logo��ɫͨ�� byǳī");
	imshow("<1>��Ϸԭ��+logo��ɫͨ�� byǳī", srcImage);


	//=================����ɫͨ�����֡�=================  
	//     ��������ͨ�����-��ɫ��������  
	//============================================  

	//��0��������ر���  
	Mat  imageGreenChannel;

	//��1�����¶���ͼƬ  
	logoImage = imread(path2, 0);
	srcImage = imread(path1);

	if (!logoImage.data) { printf("Oh��no����ȡlogoImage����~��\n"); return false; }
	if (!srcImage.data) { printf("Oh��no����ȡsrcImage����~��\n"); return false; }

	//��2����һ����ͨ��ͼ��ת����������ͨ��ͼ��  
	split(srcImage, channels);//����ɫ��ͨ��  

							  //��3����ԭͼ����ɫͨ�������÷��ظ�imageBlueChannel��ע�������ã��൱�����ߵȼۣ��޸�����һ����һ�����ű�  
	imageGreenChannel = channels.at(1);
	//��4����ԭͼ����ɫͨ���ģ�500,250�����괦���·���һ�������logoͼ���м�Ȩ���������õ��Ļ�Ͻ���浽imageGreenChannel��  
	addWeighted(imageGreenChannel(Rect(0, 0, logoImage.cols, logoImage.rows)), 1.0,
		logoImage, 0.5, 0., imageGreenChannel(Rect(0, 0, logoImage.cols, logoImage.rows)));

	//��5�������������ĵ�ͨ�����ºϲ���һ����ͨ��  
	merge(channels, srcImage);

	//��6����ʾЧ��ͼ  
	namedWindow("<2>��Ϸԭ��+logo��ɫͨ�� byǳī");
	imshow("<2>��Ϸԭ��+logo��ɫͨ�� byǳī", srcImage);



	//=================����ɫͨ�����֡�=================  
	//     ��������ͨ�����-��ɫ��������  
	//============================================  

	//��0��������ر���  
	Mat  imageRedChannel;

	//��1�����¶���ͼƬ  
	logoImage = imread(path2, 0);
	srcImage = imread(path1);

	if (!logoImage.data) { printf("Oh��no����ȡlogoImage����~��\n"); return false; }
	if (!srcImage.data) { printf("Oh��no����ȡsrcImage����~��\n"); return false; }

	//��2����һ����ͨ��ͼ��ת����������ͨ��ͼ��  
	split(srcImage, channels);//����ɫ��ͨ��  

							  //��3����ԭͼ�ĺ�ɫͨ�����÷��ظ�imageBlueChannel��ע�������ã��൱�����ߵȼۣ��޸�����һ����һ�����ű�  
	imageRedChannel = channels.at(2);
	//��4����ԭͼ�ĺ�ɫͨ���ģ�500,250�����괦���·���һ�������logoͼ���м�Ȩ���������õ��Ļ�Ͻ���浽imageRedChannel��  
	addWeighted(imageRedChannel(Rect(50, 20, logoImage.cols, logoImage.rows)), 1.0,
		logoImage, 0.5, 0., imageRedChannel(Rect(50, 20, logoImage.cols, logoImage.rows)));

	//��5�������������ĵ�ͨ�����ºϲ���һ����ͨ��  
	merge(channels, srcImage);

	//��6����ʾЧ��ͼ  
	namedWindow("<3>��Ϸԭ��+logo��ɫͨ�� byǳī");
	imshow("<3>��Ϸԭ��+logo��ɫͨ�� byǳī", srcImage);

	return true;
}

