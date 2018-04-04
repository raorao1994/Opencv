// HIST.cpp : �������̨Ӧ�ó������ڵ㡣
// ֱ��ͼ�ļ�������ʾ
#include "stdafx.h"
#include <stdlib.h>  
#include <stdio.h> 
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;
//ֵΪ255�㵱ǰ�� ȫ�ֱ����ſ�ͨ����ͨ��Ա���ñ����ֵ
CvPoint Current_Point;

bool find_point(IplImage *img, char val);

int main()
{	
	//��ֵ����
	int threshold_type = CV_THRESH_BINARY;
	//��һ���������
	int Last_Area = 0; 
	//��ǰ�������
	int Current_Area = 0;
	//��ֵ
	double threshold = 65;
	//ֵΪ255�����һ��
	CvPoint Last_Point;
	//���������ͳ������
	CvConnectedComp comp;
	//Դͼ�� HSV��ʽͼ��
	IplImage *src1, *hsv, *Igray, *Ithreshold, *Itemp, *Iopen, *Imask;
	//��ʼ����һ��
	Last_Point = cvPoint(0, 0);
	//��ʼ����ǰ��
	Current_Point = cvPoint(0, 0);
	//����ͼ��
	if (!(src1 = cvLoadImage("handle.jpg")))
		return -1;

	//�˴�����ͼ������ӦΪ��ͨ��
	//if (!(Imask = cvLoadImage("D:\\Template\\OpenCV\\Template46_hue-saturation_Hist\\Debug\\cup2.jpg", CV_LOAD_IMAGE_GRAYSCALE)))
	//  return -2;

	hsv = cvCreateImage(cvGetSize(src1), src1->depth, src1->nChannels);
	Igray = cvCreateImage(cvGetSize(src1), src1->depth, 1);
	Ithreshold = cvCreateImage(cvGetSize(src1), src1->depth, 1);
	Itemp = cvCreateImage(cvGetSize(src1), src1->depth, 1);
	Iopen = cvCreateImage(cvGetSize(src1), src1->depth, 1);
	//������������ͼ����
	Imask = cvCreateImage(cvGetSize(src1), src1->depth, 1); 
	//Դͼ��->HSV��ʽͼ��
	cvCvtColor(src1, hsv, CV_BGR2HSV);
	//Դͼ��->�Ҷ�ͼ��
	cvCvtColor(src1, Igray, CV_BGR2GRAY);
	//��ֵ��ֵ��
	cvThreshold(Igray, Ithreshold, threshold, 255, threshold_type);
	//�����㣬ȥ��С�������������� NULL:3*3�ο���Ϊ���ĵĺ�
	cvMorphologyEx(Ithreshold, Iopen, Itemp, NULL, CV_MOP_OPEN, 1);

	cvNamedWindow("ԭͼ", 1);
	cvNamedWindow("�Ҷ�ͼ", 1);
	cvNamedWindow("��ֵ��", 1);
	cvNamedWindow("������", 1);
	cvNamedWindow("FLOOD_FILL", 1);

	cvShowImage("ԭͼ", src1);
	cvShowImage("�Ҷ�ͼ", Igray);
	cvShowImage("��ֵ��", Ithreshold);
	cvShowImage("������", Iopen);
	cvShowImage("FLOOD_FILL", Imask);

	//��ˮ��� �����������
	cvNamedWindow("FLOOD_FILL", 1);
	//����������������
	cvCopy(Iopen, Imask);

	do
	{
		//������ֵΪ255�����ص�
		if (find_point(Imask, 255))
		{

			cout << " X: " << Current_Point.x << " Y: " << Current_Point.y << endl;
			//��ֵΪ255�ĵ������ˮ��䣬ֵ100
			cvFloodFill(Imask, Current_Point, cvScalar(100), cvScalar(0), cvScalar(0),
				&comp, 8 | CV_FLOODFILL_FIXED_RANGE);
			//��ǰ�������
			Current_Area = comp.area;
			//��ǰ���������һ������һ������0
			if (Last_Area<Current_Area)
			{
				if (Last_Area>0)
					//��һ����ֵ0
					cvFloodFill(Imask, Last_Point, cvScalar(0), cvScalar(0), cvScalar(0),
						&comp, 8 | CV_FLOODFILL_FIXED_RANGE);
				cvShowImage("FLOOD_FILL", Imask);
				cvWaitKey(500);
				//��ǰ����ֵ����һ����
				Last_Area = Current_Area;
				//��ǰ�㸳ֵ����һ��
				Last_Point = Current_Point;
				//memcpy(&Last_Point, &Current_Point, sizeof(CvPoint)); //���󣬴˷��������޷�����ʹ������
			}
			//��ǰ����С�ڵ�����һ���򣬵�ǰ������0
			else                                            
			{
				if (Current_Area>0)
					//��ǰ����ֵ0
					cvFloodFill(Imask, Current_Point, cvScalar(0), cvScalar(0), cvScalar(0),
						&comp, 8 | CV_FLOODFILL_FIXED_RANGE);
				cvShowImage("FLOOD_FILL", Imask);
				cvWaitKey(500);
			}
		}
		//���ʣ����������ֵ255
		else
		{
			cvFloodFill(Imask, Last_Point, cvScalar(255), cvScalar(0), cvScalar(0), &comp, 8 | CV_FLOODFILL_FIXED_RANGE);
			cvShowImage("FLOOD_FILL", Imask);
			cvWaitKey(500);
			//��һ����ֵ0
			break;
		}
	} while (true);

	cvSaveImage("Imask.jpg", Imask);

	//ɫ��(hue) ���Ͷ�(saturation) ����(value)
	IplImage *h_plane = cvCreateImage(cvSize(hsv->width, hsv->height), IPL_DEPTH_8U, 1);
	IplImage *s_plane = cvCreateImage(cvSize(hsv->width, hsv->height), IPL_DEPTH_8U, 1);
	IplImage *v_plane = cvCreateImage(cvSize(hsv->width, hsv->height), IPL_DEPTH_8U, 1);
	//ɫ�౥�Ͷ�����
	IplImage *planes[] = { h_plane,s_plane };
	//ͼ��ָ�
	
	//cvCvtPixToPlane(hsv, h_plane, s_plane, v_plane, NULL);
	cvSplit(hsv, h_plane, s_plane, v_plane, NULL);

	int h_bins = 30, s_bins = 32;

	//����ֱ��ͼ
	CvHistogram *hist;
	//��Ӧά������bins����������
	int hist_size[] = { h_bins, s_bins };
	//Hͨ�����ַ�Χ ���Ͷ�0-180
	float h_ranges[] = { 0, 180 };
	//Sͨ�����ַ�Χ
	float s_ranges[] = { 0, 255 };
	//���ַ�Χ����, ****����bin��rangeֻҪ�����С�߽�
	float* ranges[] = { h_ranges, s_ranges };   
	//����ֱ��ͼ ��ά������Ӧά��bins�������ܼ�����ʽ�洢�����ַ�Χ���ԣ�����ֱ��ͼ��
	hist = cvCreateHist(2, hist_size, CV_HIST_ARRAY, ranges, 1);
	//����ֱ��ͼ��ͼ��ֱ��ͼ�ṹ�����ۼӣ����룩
	cvCalcHist(planes, hist, 0, Imask);
	//ֱ��ͼ��һ��
	cvNormalizeHist(hist, 1.0);         

	//���ƿ��ӻ�ֱ��ͼ
	int scale = 10;
	IplImage* hist_img = cvCreateImage(cvSize(h_bins*scale, s_bins*scale), 8, 3);   //300*320
	cvZero(hist_img);

	//��С�Ҷȿ����ͼ��
	float max_value = 0;
	//��ȡֱ��ͼ���ֵ
	cvGetMinMaxHistValue(hist, NULL, &max_value, NULL, NULL);

	for (int h = 0; h < h_bins; h++)
	{
		for (int s = 0; s < s_bins; s++)
		{
			//��ȡֱ��ͼ��Ӧbin�еĸ�����
			//float bin_val = cvQueryHistValue_2D(hist, h, s);
			float bin_val = cvGetReal2D(hist, h, s);
			//ӳ�䵽255�ռ䣬��һ��̫С���ѱ�
			int intensity = cvRound(bin_val * 255 / max_value); 
			cvRectangle(hist_img, cvPoint(h*scale, s*scale),
				cvPoint((h + 1)*scale - 1, (s + 1)*scale - 1),
				CV_RGB(intensity, intensity, intensity), CV_FILLED);
		}
	}

	cvNamedWindow("HIST_Image", 1);
	cvShowImage("HIST_Image", hist_img);

	cvWaitKey();

	cvReleaseHist(&hist);

	cvReleaseImage(&src1);
	cvReleaseImage(&hsv);
	cvReleaseImage(&Igray);
	cvReleaseImage(&Ithreshold);
	cvReleaseImage(&Itemp);
	cvReleaseImage(&Iopen);
	cvReleaseImage(&Imask);
	cvReleaseImage(&h_plane);
	cvReleaseImage(&s_plane);
	cvReleaseImage(&v_plane);
	cvReleaseImage(&hist_img);

	cvDestroyWindow("src1");
	cvDestroyWindow("HIST_Image");
	cvDestroyWindow("GRAY_Image");
	cvDestroyWindow("THRESHHOLD_Image");
	cvDestroyWindow("OPEN_Image");
	cvDestroyWindow("FLOOD_FILL");
	cvDestroyWindow("HIST_Image");
}

/******************����ͼ��ָ���㷨********************/
bool find_point(IplImage *img, char val)
{
	char* ptr = NULL;

	if (img->nChannels == 1)
	{
		ptr = img->imageData;
		if (ptr != NULL)
		{
			for (int i = 0; i < img->height; i++)       //����ָ����Ѱַ
			{
				ptr = (img->imageData + i*(img->widthStep));   //i �� j ��
				for (int j = 0; j < img->width; j++)    //����ָ����Ѱַ
				{
					if (ptr[j] == val)              //�ж�ĳ�������Ƿ�Ϊ255
					{
						Current_Point.x = j;        /********�ֲ������˷�ʽ �޷�ʵ�ָ�ֵ********/
						Current_Point.y = i;
						return true;
					}
				}
			}
		}
	}
	return false;
}