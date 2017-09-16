//-----------------------------------��ͷ�ļ��������֡�---------------------------------------  
//      ����������������������ͷ�ļ�  
//----------------------------------------------------------------------------------------------   
#include "stdafx.h"
#include <opencv2/opencv.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  

//-----------------------------------�������ռ��������֡�--------------------------------------  
//      ����������������ʹ�õ������ռ�  
//-----------------------------------------------------------------------------------------------   
using namespace std;
using namespace cv;
//-----------------------------------��ȫ�ֱ����������֡�--------------------------------------  
//      ������ȫ�ֱ�������  
//-----------------------------------------------------------------------------------------------  
Mat g_srcImage, g_dstImage, g_midImage;//ԭʼͼ���м�ͼ��Ч��ͼ  
vector<Vec4i> g_lines;//����һ��ʸ���ṹg_lines���ڴ�ŵõ����߶�ʸ������  
					  //�������յ�TrackBarλ�ò���  
int g_nthreshold = 100;

//-----------------------------------��ȫ�ֺ����������֡�--------------------------------------  
//      ������ȫ�ֺ�������  
//-----------------------------------------------------------------------------------------------  
static void on_HoughLines(int, void*);//�ص�����  
//-----------------------------------��main( )������--------------------------------------------  
//      ����������̨Ӧ�ó������ں��������ǵĳ�������￪ʼ  
//-----------------------------------------------------------------------------------------------  
//-----------------------------------��main( )������--------------------------------------------  
//      ����������̨Ӧ�ó������ں��������ǵĳ�������￪ʼ  
//-----------------------------------------------------------------------------------------------  
int line()
{
	//�ı�console������ɫ  
	system("color 3F");

	//����ԭʼͼ��Mat��������     
	Mat g_srcImage = imread("d:/11.jpg");  //����Ŀ¼��Ӧ����һ����Ϊ1.jpg���ز�ͼ  

									   //��ʾԭʼͼ    
	imshow("��ԭʼͼ��", g_srcImage);

	//����������  
	namedWindow("��Ч��ͼ��", 1);
	createTrackbar("ֵ", "��Ч��ͼ��", &g_nthreshold, 200, on_HoughLines);

	//���б�Ե����ת��Ϊ�Ҷ�ͼ  
	Canny(g_srcImage, g_midImage, 50, 200, 3);//����һ��canny��Ե���  
	cvtColor(g_midImage, g_dstImage, CV_GRAY2BGR);//ת����Ե�����ͼΪ�Ҷ�ͼ  

	//����һ�λص�����������һ��HoughLinesP����  
	on_HoughLines(g_nthreshold, 0);
	HoughLinesP(g_midImage, g_lines, 1, CV_PI / 180, 80, 50, 10);
	//��ʾЧ��ͼ    
	imshow("��Ч��ͼ��", g_dstImage);
	waitKey(0);
	return 0;

}


//-----------------------------------��on_HoughLines( )������--------------------------------  
//      ����������ñ����/��ñ���㡿���ڵĻص�����  
//----------------------------------------------------------------------------------------------  
static void on_HoughLines(int, void*)
{
	//����ֲ���������ȫ�ֱ���  
	Mat dstImage = g_dstImage.clone();
	Mat midImage = g_midImage.clone();

	//����HoughLinesP����  
	vector<Vec4i> mylines;
	HoughLinesP(midImage, mylines, 1, CV_PI / 180, g_nthreshold + 1, 50, 10);

	//ѭ����������ÿһ���߶�  
	for (size_t i = 0; i < mylines.size(); i++)
	{
		Vec4i l = mylines[i];
		line(dstImage, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(23, 180, 55), 1, CV_AA);
	}
	//��ʾͼ��  
	imshow("��Ч��ͼ��", dstImage);
}

int circle()
{
	//��1������ԭʼͼ��Mat��������     
	Mat srcImage = imread("d:/11.jpg");  //����Ŀ¼��Ӧ����һ����Ϊ1.jpg���ز�ͼ  
	Mat midImage, dstImage;//��ʱ������Ŀ��ͼ�Ķ���  

						   //��2����ʾԭʼͼ  
	imshow("��ԭʼͼ��", srcImage);

	//��3��תΪ�Ҷ�ͼ������ͼ��ƽ��  
	cvtColor(srcImage, midImage, CV_BGR2GRAY);//ת����Ե�����ͼΪ�Ҷ�ͼ  
	GaussianBlur(midImage, midImage, Size(9, 9), 2, 2);

	//��4�����л���Բ�任  
	vector<Vec3f> circles;
	HoughCircles(midImage, circles, CV_HOUGH_GRADIENT, 1.5, 10, 200, 100, 0, 0);

	//��5��������ͼ�л��Ƴ�Բ  
	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		//����Բ��  
		circle(srcImage, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		//����Բ����  
		circle(srcImage, center, radius, Scalar(155, 50, 255), 3, 8, 0);
	}

	//��6����ʾЧ��ͼ    
	imshow("��Ч��ͼ��", srcImage);

	waitKey(0);

	return 0;
}