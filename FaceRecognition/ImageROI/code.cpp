//-----------------------------------������˵����----------------------------------------------
//  ��������:ROI����ͼ�����&����ͼ����
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


//-----------------------------------��ȫ�ֺ����������֡�--------------------------------------
//     ������ȫ�ֺ�������
//-----------------------------------------------------------------------------------------------
bool ROI_AddImage();
bool LinearBlending();
bool ROI_LinearBlending();

//-----------------------------------��main( )������--------------------------------------------
//     ����������̨Ӧ�ó������ں��������ǵĳ�������￪ʼ
//-----------------------------------------------------------------------------------------------
int main1()
{
	system("color 5E");

	if (ROI_AddImage() && LinearBlending() && ROI_LinearBlending())
	{
		cout << endl << "�š����ˣ��ó�������Ҫ��ͼ��~! : )";
	}

	waitKey(0);
	return 0;
}

//----------------------------------��ROI_AddImage( )������----------------------------------
// ��������ROI_AddImage����
//     ���������ø���Ȥ����ROIʵ��ͼ�����
//----------------------------------------------------------------------------------------------
bool ROI_AddImage()
{

	//��1������ͼ��
	Mat srcImage1 = imread("dota_pa.jpg");
	Mat logoImage = imread("dota_logo.jpg");
	if (!srcImage1.data) { printf("��ȡsrcImage1����~�� \n"); return false; }
	if (!logoImage.data) { printf("��ȡlogoImage����~�� \n"); return false; }

	//��2������һ��Mat���Ͳ������趨ROI����
	Mat imageROI = srcImage1(Rect(200, 250, logoImage.cols, logoImage.rows));

	//��3��������ģ�������ǻҶ�ͼ��
	Mat mask = imread("dota_logo.jpg", 0);

	//��4������Ĥ������ROI
	logoImage.copyTo(imageROI, mask);

	//��5����ʾ���
	namedWindow("<1>����ROIʵ��ͼ�����ʾ������");
	imshow("<1>����ROIʵ��ͼ�����ʾ������", srcImage1);

	return true;
}


//---------------------------------��LinearBlending����������-------------------------------------
// ��������LinearBlending����
// ����������cv::addWeighted��������ʵ��ͼ�����Ի��
//--------------------------------------------------------------------------------------------
bool LinearBlending()
{
	//��0������һЩ�ֲ�����
	double alphaValue = 0.5;
	double betaValue;
	Mat srcImage2, srcImage3, dstImage;

	//��1����ȡͼ�� ( ����ͼƬ��Ϊͬ�������ͺͳߴ� )
	srcImage2 = imread("mogu.jpg");
	srcImage3 = imread("rain.jpg");

	if (!srcImage2.data) { printf("��ȡsrcImage2����~�� \n"); return false; }
	if (!srcImage3.data) { printf("��ȡsrcImage3����~�� \n"); return false; }

	//��2������ͼ���ϼ�Ȩ����
	betaValue = (1.0 - alphaValue);
	addWeighted(srcImage2, alphaValue, srcImage3, betaValue, 0.0, dstImage);

	//��3����������ʾԭͼ����
	namedWindow("<2>���Ի��ʾ������", 1);
	imshow("<2>���Ի��ʾ������", srcImage2);

	namedWindow("<3>���Ի��ʾ������", 1);
	imshow("<3>���Ի��ʾ������", dstImage);

	return true;

}

//---------------------------------��ROI_LinearBlending������-------------------------------------
// ��������ROI_LinearBlending����
// ���������Ի��ʵ�ֺ���,ָ����������ͼ����.����cv::addWeighted����������϶���
//                     ����Ȥ����ROI��ʵ���Զ�����������Ի��
//--------------------------------------------------------------------------------------------
bool ROI_LinearBlending()
{

	//��1����ȡͼ��
	Mat srcImage4 = imread("dota_pa.jpg", 1);
	Mat logoImage = imread("dota_logo.jpg");

	if (!srcImage4.data) { printf("��ȡsrcImage4����~�� \n"); return false; }
	if (!logoImage.data) { printf("��ȡlogoImage����~�� \n"); return false; }

	//��2������һ��Mat���Ͳ������趨ROI����
	Mat imageROI;
	//����һ
	imageROI = srcImage4(Rect(200, 250, logoImage.cols, logoImage.rows));
	//������
	//imageROI=srcImage4(Range(250,250+logoImage.rows),Range(200,200+logoImage.cols));

	//��3����logo�ӵ�ԭͼ��
	addWeighted(imageROI, 0.5, logoImage, 0.3, 0., imageROI);

	//��4����ʾ���
	namedWindow("<4>��������ͼ����ʾ������");
	imshow("<4>��������ͼ����ʾ������", srcImage4);

	return true;
}