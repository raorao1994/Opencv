// ImageROI.cpp : �������̨Ӧ�ó������ڵ㡣
//(Դ����)http://blog.csdn.net/poem_qianmo/article/details/20911629
// ROI����ͼ�����&����ͼ���� ȫ����

#include "stdafx.h"
#include <opencv2\opencv.hpp>

const char *path1 = "D://8.jpg";
const char *path11 = "D://10.jpg";
const char *path2 = "D://9.jpg";
using namespace cv;
//----------------------------------��ROI_AddImage( )������----------------------------------  
// ��������ROI_AddImage����  
//     ���������ø���Ȥ����ROIʵ��ͼ�����  
//----------------------------------------------------------------------------------------------  
bool ROI_AddImage1()
{

	//��1������ͼ��  
	Mat srcImage1 = imread(path1);
	Mat logoImage = imread(path2);
	if (!srcImage1.data) { printf("���ã���ȡsrcImage1����~�� \n"); return false; }
	if (!logoImage.data) { printf("���ã���ȡlogoImage����~�� \n"); return false; }

	//��2������һ��Mat���Ͳ������趨ROI����  
	Mat imageROI = srcImage1(Rect(100, 100, logoImage.cols, logoImage.rows));

	//��3��������ģ�������ǻҶ�ͼ��  
	Mat mask = imread(path2, 0);

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
bool LinearBlending1()
{
	//��0������һЩ�ֲ�����  
	double alphaValue = 0.5;
	double betaValue;
	Mat srcImage2, srcImage3, dstImage;

	//��1����ȡͼ�� ( ����ͼƬ��Ϊͬ�������ͺͳߴ� )  
	srcImage2 = imread(path1);
	srcImage3 = imread(path11);

	if (!srcImage2.data) { printf("���ã���ȡsrcImage2����~�� \n"); return false; }
	if (!srcImage3.data) { printf("���ã���ȡsrcImage3����~�� \n"); return false; }

	//��2����ͼ���ϼ�Ȩ����  
	betaValue = (1.0 - alphaValue);
	addWeighted(srcImage2, alphaValue, srcImage3, betaValue, 0.0, dstImage);

	//��3����������ʾԭͼ����  
	namedWindow("<2>���Ի��ʾ�����ڡ�ԭͼ�� byǳī", 1);
	imshow("<2>���Ի��ʾ�����ڡ�ԭͼ�� byǳī", srcImage2);

	namedWindow("<3>���Ի��ʾ�����ڡ�Ч��ͼ�� byǳī", 1);
	imshow("<3>���Ի��ʾ�����ڡ�Ч��ͼ�� byǳī", dstImage);

	return true;

}

bool ROI_LinearBlending1()
{

	//��1����ȡͼ��
	Mat srcImage4 = imread(path1, 1);
	Mat logoImage = imread(path2);

	if (!srcImage4.data) { printf("���ã���ȡsrcImage4����~�� \n"); return false; }
	if (!logoImage.data) { printf("���ã���ȡlogoImage����~�� \n"); return false; }

	//��2������һ��Mat���Ͳ������趨ROI����
	Mat imageROI;
	//����һ
	imageROI = srcImage4(Rect(100, 10, logoImage.cols, logoImage.rows));
	//������
	//imageROI=srcImage4(Range(250,250+logoImage.rows),Range(200,200+logoImage.cols));

	//��3����logo�ӵ�ԭͼ��
	addWeighted(imageROI, 0.5, logoImage, 0.3, 0., imageROI);

	//��4����ʾ���
	namedWindow("<4>��������ͼ����ʾ������ byǳī");
	imshow("<4>��������ͼ����ʾ������ byǳī", srcImage4);

	return true;
}
int main()
{
	//ROI_AddImage();
	//LinearBlending1();
	ROI_LinearBlending1();
	int i=waitKey(0);
    return 0;
}

