// 04���ڿ�ķ���ͶӰ.cpp : �������̨Ӧ�ó������ڵ㡣
// ���ڿ�ķ���ͶӰ��cvCalcArrBackProjectPatch��

#include "stdafx.h"
#include <stdlib.h>  
#include <stdio.h> 
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;

int main()
{
	//Դͼ�� HSV��ʽͼ��
	IplImage* Ihand_model, *Ihand_test, *Ihand_mask, *hsv1, *hsv2, *hsv3, *hsv4, *Icup_model, *Icup_test, *Icup_mask;

	//δ����ķ�ɫģ��ͼ��
	if (!(Ihand_model = cvLoadImage("../data/2.png")))
		return -1;
	//Ѱ�����Ʒ���ͶӰ�Ĳ���ͼ��
	if (!(Ihand_test = cvLoadImage("../data/3.png")))
		return -2;
	//���ڴ����ɫģ��ͼ������룬�˴�����ͼ������ӦΪ��ͨ��
	if (!(Ihand_mask = cvLoadImage("../data/2.png",CV_LOAD_IMAGE_GRAYSCALE)))
		return -3;
	//δ����ı�����ɫģ��ͼ��
	if (!(Icup_model = cvLoadImage("../data/1.jpg")))
		return -4;
	//Ѱ�ұ��ӷ���ͶӰ�Ĳ���ͼ��
	if (!(Icup_test = cvLoadImage("../data/2.jpg")))
		return -5;
	//���ڴ�������ɫģ��ͼ������� �ɻ�ȡ���ӷ���ͶӰ���С
	if (!(Icup_mask = cvLoadImage("../data/1.jpg",CV_LOAD_IMAGE_GRAYSCALE)))
		return -6;

	int hand_patch_width = 5;                       //���Ʒ���ͶӰ����
	int hand_patch_height = 5;                      //���Ʒ���ͶӰ��߶�

	int cup_patch_width = Icup_model->width;        //���ӷ���ͶӰ����
	int cup_patch_height = Icup_model->height;      //���ӷ���ͶӰ��߶�

	cvXorS(Ihand_mask, cvScalar(255), Ihand_mask);          //����ͼ��λ����󷴣�����������������Ϊ����
	cvXorS(Icup_mask, cvScalar(255), Icup_mask);            //����ͼ��λ����󷴣��Ա�������������Ϊ����

	cvSet(Ihand_model, cvScalarAll(0), Ihand_mask);         //���������������ڣ����ɷ�ɫģ��ͼ��
	cvSet(Icup_model, cvScalarAll(0), Icup_mask);           //���������������ڣ����ɷ�ɫģ��ͼ��

	hsv1 = cvCreateImage(cvGetSize(Ihand_model), Ihand_model->depth, Ihand_model->nChannels);   //HSV
	hsv2 = cvCreateImage(cvGetSize(Ihand_test), Ihand_test->depth, Ihand_test->nChannels);      //HSV
	hsv3 = cvCreateImage(cvGetSize(Icup_model), Icup_model->depth, Icup_model->nChannels);      //HSV
	hsv4 = cvCreateImage(cvGetSize(Icup_test), Icup_test->depth, Icup_test->nChannels);         //HSV

	cvCvtColor(Ihand_model, hsv1, CV_BGR2HSV);      //Դͼ��->HSV��ʽͼ��
	cvCvtColor(Ihand_test, hsv2, CV_BGR2HSV);       //Դͼ��->HSV��ʽͼ��
	cvCvtColor(Icup_model, hsv3, CV_BGR2HSV);       //Դͼ��->HSV��ʽͼ��
	cvCvtColor(Icup_test, hsv4, CV_BGR2HSV);        //Դͼ��->HSV��ʽͼ��

													//����ͶӰͼ�� ��С:����ͼ��-��Ĵ�С ����������
	IplImage *Iback_projection_patch_hand = cvCreateImage(
		cvSize(Ihand_test->width - hand_patch_width + 1, Ihand_test->height - hand_patch_height + 1),
		IPL_DEPTH_32F, 1);
	IplImage *Iback_projection_patch_cup = cvCreateImage(
		cvSize(Icup_test->width - cup_patch_width + 1, Icup_test->height - cup_patch_height + 1),
		IPL_DEPTH_32F, 1);

	//ɫ��(hue) ���Ͷ�(saturation) ����(value) ������ͨ��ͼ��
	IplImage *h_plane_1 = cvCreateImage(cvSize(hsv1->width, hsv1->height), IPL_DEPTH_8U, 1);
	IplImage *s_plane_1 = cvCreateImage(cvSize(hsv1->width, hsv1->height), IPL_DEPTH_8U, 1);
	IplImage *v_plane_1 = cvCreateImage(cvSize(hsv1->width, hsv1->height), IPL_DEPTH_8U, 1);

	IplImage *h_plane_2 = cvCreateImage(cvSize(hsv2->width, hsv2->height), IPL_DEPTH_8U, 1);
	IplImage *s_plane_2 = cvCreateImage(cvSize(hsv2->width, hsv2->height), IPL_DEPTH_8U, 1);
	IplImage *v_plane_2 = cvCreateImage(cvSize(hsv2->width, hsv2->height), IPL_DEPTH_8U, 1);

	IplImage *h_plane_3 = cvCreateImage(cvSize(hsv3->width, hsv3->height), IPL_DEPTH_8U, 1);
	IplImage *s_plane_3 = cvCreateImage(cvSize(hsv3->width, hsv3->height), IPL_DEPTH_8U, 1);
	IplImage *v_plane_3 = cvCreateImage(cvSize(hsv3->width, hsv3->height), IPL_DEPTH_8U, 1);

	IplImage *h_plane_4 = cvCreateImage(cvSize(hsv4->width, hsv4->height), IPL_DEPTH_8U, 1);
	IplImage *s_plane_4 = cvCreateImage(cvSize(hsv4->width, hsv4->height), IPL_DEPTH_8U, 1);
	IplImage *v_plane_4 = cvCreateImage(cvSize(hsv4->width, hsv4->height), IPL_DEPTH_8U, 1);

	IplImage *planes1[] = { h_plane_1, s_plane_1 };                     //ɫ�౥�Ͷ�����
	IplImage *planes2[] = { h_plane_2, s_plane_2 };                     //ɫ�౥�Ͷ�����
	IplImage *planes3[] = { h_plane_3, s_plane_3 };                     //ɫ�౥�Ͷ�����
	IplImage *planes4[] = { h_plane_4, s_plane_4 };                     //ɫ�౥�Ͷ�����

	cvSplit(hsv1, h_plane_1, s_plane_1, v_plane_1, NULL);       //ͼ��ָ�
	cvSplit(hsv2, h_plane_2, s_plane_2, v_plane_2, NULL);       //ͼ��ָ�
	cvSplit(hsv3, h_plane_3, s_plane_3, v_plane_3, NULL);
	cvSplit(hsv4, h_plane_4, s_plane_4, v_plane_4, NULL);

	int h_bins = 30, s_bins = 32;               //hάbins�ĸ�����sάbins�ĸ���               

												//����ģ��Ͳ���ֱ��ͼ
	CvHistogram *hist_model_hand, *hist_test_hand, *hist_model_cup, *hist_test_cup;

	int hist_size[] = { h_bins, s_bins };       //��Ӧά������bins����������
	float h_ranges[] = { 0, 180 };              //Hͨ�����ַ�Χ ���Ͷ�0-180
	float s_ranges[] = { 0, 255 };              //Sͨ�����ַ�Χ
	float* ranges[] = { h_ranges, s_ranges };   //���ַ�Χ����, ****����bin��rangeֻҪ�����С�߽�

	//����ֱ��ͼ ��ά������Ӧά��bins�������ܼ�����ʽ�洢�����ַ�Χ���ԣ�����ֱ��ͼ��
	hist_model_hand = cvCreateHist(2, hist_size, CV_HIST_ARRAY, ranges, 1);
	hist_test_hand = cvCreateHist(2, hist_size, CV_HIST_ARRAY, ranges, 1);
	hist_model_cup = cvCreateHist(2, hist_size, CV_HIST_ARRAY, ranges, 1);
	hist_test_cup = cvCreateHist(2, hist_size, CV_HIST_ARRAY, ranges, 1);
	//����ֱ��ͼ
	cvCalcHist(planes1, hist_model_hand, 0, 0);     //����ֱ��ͼ��ͼ��ֱ��ͼ�ṹ�����ۼӣ����룩��ɫֱ��ͼ
	cvCalcHist(planes2, hist_test_hand, 0, 0);      //����ֱ��ͼ��ͼ��ֱ��ͼ�ṹ�����ۼӣ����룩����ֱ��ͼ
	cvCalcHist(planes3, hist_model_cup, 0, 0);      //����ֱ��ͼ��ͼ��ֱ��ͼ�ṹ�����ۼӣ����룩��ɫֱ��ͼ
	cvCalcHist(planes4, hist_test_cup, 0, 0);       //����ֱ��ͼ��ͼ��ֱ��ͼ�ṹ�����ۼӣ����룩����ֱ��ͼ
													//ֱ��ͼ��һ��
	cvNormalizeHist(hist_model_hand, 1.0);          //ֱ��ͼ��һ��
	cvNormalizeHist(hist_test_hand, 1.0);           //ֱ��ͼ��һ��
	cvNormalizeHist(hist_model_cup, 1.0);           //ֱ��ͼ��һ��
	cvNormalizeHist(hist_test_cup, 1.0);            //ֱ��ͼ��һ��

													//���ƿ��ӻ�ֱ��ͼ
	int scale = 10;     //ֱ��ͼ��ɫֵͼ����ʾ����
	IplImage* Ihist_model_hand = cvCreateImage(cvSize(h_bins*scale, s_bins*scale), 8, 3);   //300*320
	IplImage* Ihist_test_hand = cvCreateImage(cvSize(h_bins*scale, s_bins*scale), 8, 3);    //300*320
	IplImage* Ihist_model_cup = cvCreateImage(cvSize(h_bins*scale, s_bins*scale), 8, 3);    //300*320
	IplImage* Ihist_test_cup = cvCreateImage(cvSize(h_bins*scale, s_bins*scale), 8, 3);     //300*320

																							//ֱ��ͼ��ɫֵͼ������
	cvZero(Ihist_model_hand);
	cvZero(Ihist_test_hand);
	cvZero(Ihist_model_cup);
	cvZero(Ihist_test_cup);

	//��С�Ҷȿ����ͼ��
	float max_value_model_hand = 0; //ֱ��ͼ�����ֵ��Ϊӳ����׼��
	float max_value_test_hand = 0;
	float max_value_model_cup = 0;  //ֱ��ͼ�����ֵ��Ϊӳ����׼��
	float max_value_test_cup = 0;

	cvGetMinMaxHistValue(hist_model_hand, NULL, &max_value_model_hand, NULL, NULL); //ֱ��ͼ���ֵ
	cvGetMinMaxHistValue(hist_test_hand, NULL, &max_value_test_hand, NULL, NULL);   //ֱ��ͼ���ֵ
	cvGetMinMaxHistValue(hist_model_cup, NULL, &max_value_model_cup, NULL, NULL);   //ֱ��ͼ���ֵ
	cvGetMinMaxHistValue(hist_test_cup, NULL, &max_value_test_cup, NULL, NULL);     //ֱ��ͼ���ֵ

	for (int h = 0; h < h_bins; h++)
	{
		for (int s = 0; s < s_bins; s++)
		{
			float bin_val_model_hand = cvGetReal2D(hist_model_hand->bins, h, s);  //bin�еĸ�����
			float bin_val_test_hand = cvGetReal2D(hist_test_hand->bins, h, s);    //bin�еĸ�����
			float bin_val_model_cup = cvGetReal2D(hist_model_cup->bins, h, s);    //bin�еĸ�����
			float bin_val_test_cup = cvGetReal2D(hist_test_cup->bins, h, s);      //bin�еĸ�����

			int intensity1 = cvRound(bin_val_model_hand * 255 / max_value_model_hand);  //ӳ�䵽255�ռ�
			int intensity2 = cvRound(bin_val_test_hand * 255 / max_value_test_hand);    //��һ��̫С
			int intensity3 = cvRound(bin_val_model_cup * 255 / max_value_model_cup);    //ӳ�䵽255�ռ�
			int intensity4 = cvRound(bin_val_test_cup * 255 / max_value_test_cup);      //��һ��̫С

			cvRectangle(Ihist_model_hand, cvPoint(h*scale, s*scale),            //����С�Ҷȿ����ͼ��
				cvPoint((h + 1)*scale - 1, (s + 1)*scale - 1),
				CV_RGB(intensity1, intensity1, intensity1), CV_FILLED);
			cvRectangle(Ihist_test_hand, cvPoint(h*scale, s*scale),
				cvPoint((h + 1)*scale - 1, (s + 1)*scale - 1),
				CV_RGB(intensity2, intensity2, intensity2), CV_FILLED);
			cvRectangle(Ihist_model_cup, cvPoint(h*scale, s*scale),         //����С�Ҷȿ����ͼ��
				cvPoint((h + 1)*scale - 1, (s + 1)*scale - 1),
				CV_RGB(intensity3, intensity3, intensity3), CV_FILLED);
			cvRectangle(Ihist_test_cup, cvPoint(h*scale, s*scale),
				cvPoint((h + 1)*scale - 1, (s + 1)*scale - 1),
				CV_RGB(intensity4, intensity4, intensity4), CV_FILLED);
		}
	}

	CvSize hand_patch_size = cvSize(hand_patch_width, hand_patch_height);   //���Ʒ���ͶӰ��ߴ�
	CvSize cup_patch_size = cvSize(cup_patch_width, cup_patch_height);      //���ӷ���ͶӰ��ߴ�

	//���������� ��������С��Ŀ�� ����ͼ������ ��ķ���ͶӰͼ�� ���С ģ��ֱ��ͼ �ȽϷ�ʽ����أ� ��һ��ˮƽ
	cvCalcArrBackProjectPatch((CvArr **)planes2, Iback_projection_patch_hand, hand_patch_size, hist_model_hand, CV_COMP_CORREL, 1);
	//��Ŀ������ �������ڵ���Ŀ�� ����ͼ������ ��ķ���ͶӰͼ�� ���С ģ��ֱ��ͼ �ȽϷ�ʽ����أ� ��һ��ˮƽ
	cvCalcArrBackProjectPatch((CvArr **)planes4, Iback_projection_patch_cup, cup_patch_size, hist_model_cup, CV_COMP_CORREL, 1);

	cvNamedWindow("Mask_Hand", 1);
	cvNamedWindow("Model_Hand", 1);
	cvNamedWindow("Test_Hand", 1);
	cvNamedWindow("HIST_Model_Hand", 1);
	cvNamedWindow("HIST_Test_Hand", 1);
	cvNamedWindow("BACK_Projection_Hand", 1);

	cvNamedWindow("Mask_Cup", 1);
	cvNamedWindow("Model_Cup", 1);
	cvNamedWindow("Test_Cup", 1);
	cvNamedWindow("HIST_Model_Cup", 1);
	cvNamedWindow("HIST_Test_Cup", 1);
	cvNamedWindow("BACK_Projection_Cup", 1);

	cvShowImage("Mask_Hand", Ihand_mask);
	cvShowImage("Model_Hand", Ihand_model);
	cvShowImage("Test_Hand", Ihand_test);
	cvShowImage("HIST_Model_Hand", Ihist_model_hand);
	cvShowImage("HIST_Test_Hand", Ihist_test_hand);
	cvShowImage("BACK_Projection_Hand", Iback_projection_patch_hand);

	cvShowImage("Mask_Cup", Icup_mask);
	cvShowImage("Model_Cup", Icup_model);
	cvShowImage("Test_Cup", Icup_test);
	cvShowImage("HIST_Model_Cup", Ihist_model_cup);
	cvShowImage("HIST_Test_Cup", Ihist_test_cup);
	cvShowImage("BACK_Projection_Cup", Iback_projection_patch_cup);

	cvWaitKey(0);
	//system("pause");

	cvReleaseHist(&hist_model_hand);
	cvReleaseHist(&hist_test_hand);
	cvReleaseHist(&hist_model_cup);
	cvReleaseHist(&hist_test_cup);

	cvReleaseImage(&Ihand_model);
	cvReleaseImage(&Ihand_test);
	cvReleaseImage(&Ihand_mask);
	cvReleaseImage(&hsv1);
	cvReleaseImage(&hsv2);
	cvReleaseImage(&hsv3);
	cvReleaseImage(&hsv4);
	cvReleaseImage(&Icup_model);
	cvReleaseImage(&Icup_test);
	cvReleaseImage(&Icup_mask);
	cvReleaseImage(&h_plane_1);
	cvReleaseImage(&s_plane_1);
	cvReleaseImage(&v_plane_1);
	cvReleaseImage(&h_plane_2);
	cvReleaseImage(&s_plane_2);
	cvReleaseImage(&v_plane_2);
	cvReleaseImage(&h_plane_3);
	cvReleaseImage(&s_plane_3);
	cvReleaseImage(&v_plane_3);
	cvReleaseImage(&h_plane_4);
	cvReleaseImage(&s_plane_4);
	cvReleaseImage(&v_plane_4);
	cvReleaseImage(&Ihist_model_hand);
	cvReleaseImage(&Ihist_test_hand);
	cvReleaseImage(&Ihist_model_cup);
	cvReleaseImage(&Ihist_test_cup);
	cvReleaseImage(&Iback_projection_patch_hand);
	cvReleaseImage(&Iback_projection_patch_cup);

	cvDestroyAllWindows();
}

