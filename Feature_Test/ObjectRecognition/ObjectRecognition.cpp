// ObjectRecognition.cpp : �������̨Ӧ�ó������ڵ㡣
//����ͼ���Ե��ͼ��ʶ���㷨

#include "stdafx.h"
#include <stdio.h> 
#include <iostream>
#include <time.h>
#include "GeoMatch.h"
#include "CommandParser.h"

using namespace std;

int main(int argc, char *argv[])
{
	void WrongUsage();

	CommandParser cp(argc, argv); // object to parse command line

	GeoMatch GM;				// object to implent geometric matching	
	int lowThreshold = 10;		//deafult value
	int highThreashold = 100;	//deafult value

	double minScore = 0.7;		//deafult value
	double greediness = 0.8;		//deafult value

	double total_time = 0;
	double score = 0;
	CvPoint result;

	//Load Template image 
	char *param;
	param = cp.GetParameter("-t");
	param = "Template.jpg";
	if (param == NULL)
	{
		cout << "ERROR: ģ��ͼ��Ϊ��";
		WrongUsage();
		return -1;
	}

	IplImage* templateImage = cvLoadImage(param, -1);
	if (templateImage == NULL)
	{
		cout << "\nERROR: ģ��ͼ�����ʧ��\n" << param;
		return 0;
	}

	param = cp.GetParameter("-s");
	param = "Search3.jpg";
	if (param == NULL)
	{
		cout << "ERROR: ������ͼ��Ϊ��";
		WrongUsage();
		return -1;
	}
	//Load Search Image
	IplImage* searchImage = cvLoadImage(param, -1);
	if (searchImage == NULL)
	{
		cout << "\nERROR: ������ͼ�����ʧ��" << param;
		return 0;
	}

	param = cp.GetParameter("-l"); //get Low threshold
	param = "10";
	if (param != NULL)
		lowThreshold = atoi(param);

	param = cp.GetParameter("-h");
	param = "100";
	if (param != NULL)
		highThreashold = atoi(param);//get high threshold

	param = cp.GetParameter("-m"); // get minimum score
	param = "0.7";
	if (param != NULL)
		minScore = atof(param);

	param = cp.GetParameter("-g");//get greediness
	param = "0.5";
	if (param != NULL)
		greediness = atof(param);

	CvSize templateSize = cvSize(templateImage->width, templateImage->height);
	IplImage* grayTemplateImg = cvCreateImage(templateSize, IPL_DEPTH_8U, 1);

	// Convert color image to gray image.
	if (templateImage->nChannels == 3)
	{
		cvCvtColor(templateImage, grayTemplateImg, CV_RGB2GRAY);
	}
	else
	{
		cvCopy(templateImage, grayTemplateImg);
	}
	cout << "\n ���ڱ�Ե��ģ��ƥ�����\n";
	cout << " ------------------------------------\n";

	if (!GM.CreateGeoMatchModel(grayTemplateImg, lowThreshold, highThreashold))
	{
		cout << "ERROR: �޷�����ʶ��ģ��";
		return 0;
	}
	GM.DrawContours(templateImage, CV_RGB(255, 0, 0), 1);
	cout << " ʶ��ģ�ʹ����ɹ�" << "����ֵ = " << lowThreshold << " ����ֵ = " << highThreashold << endl;
	CvSize searchSize = cvSize(searchImage->width, searchImage->height);
	IplImage* graySearchImg = cvCreateImage(searchSize, IPL_DEPTH_8U, 1);

	// Convert color image to gray image. 
	if (searchImage->nChannels == 3)
		cvCvtColor(searchImage, graySearchImg, CV_RGB2GRAY);
	else
	{
		cvCopy(searchImage, graySearchImg);
	}
	cout << " �ҵ�����" << " Minumum���� = " << minScore << " Greediness = " << greediness << "\n\n";
	cout << " ------------------------------------\n";
	clock_t start_time1 = clock();
	score = GM.FindGeoMatchModel(graySearchImg, minScore, greediness, &result);
	clock_t finish_time1 = clock();
	total_time = (double)(finish_time1 - start_time1) / CLOCKS_PER_SEC;

	if (score>minScore) // if score is atleast 0.4
	{
		cout << " ʶ�𵽵�ͼ��λ��Ϊ�� [" << result.x << ", " << result.y << "]\n Score = " << score << "\n ʶ��ʱ�� = " << total_time * 1000 << "ms";
		GM.DrawContours(searchImage, result, CV_RGB(0, 255, 0), 1);
	}
	else
		cout << " ����δ�ҵ�";

	cout << "\n ------------------------------------\n\n";
	cout << "\n �밴�����ⰸ��������һ��!";

	//Display result
	cvNamedWindow("Template", CV_WINDOW_AUTOSIZE);
	cvShowImage("Template", templateImage);
	cvNamedWindow("Search Image", CV_WINDOW_AUTOSIZE);
	cvShowImage("Search Image", searchImage);
	// wait for both windows to be closed before releasing images
	cvWaitKey(0);
	cvDestroyWindow("Search Image");
	cvDestroyWindow("Template");
	cvReleaseImage(&searchImage);
	cvReleaseImage(&graySearchImg);
	cvReleaseImage(&templateImage);
	cvReleaseImage(&grayTemplateImg);

	return 1;
}
void WrongUsage()
{
	cout << "\n ���ڱ�Ե��ģ��ƥ�����\n";
	cout << " ------------------------------------\n";
	cout << "\n�������:\n\n";
	cout << "     -t ģ��ͼ���� (image to be searched)\n\n";
	cout << "     -h ����ֵ (High threshold for creating template model)\n\n";
	cout << "     -l ����ֵ (Low threshold for creating template model)\n\n";
	cout << "     -s ����ͼ������ (image we are trying to find)\n\n";
	cout << "     -m Minumum���� (���������������ͷ��� [0.0 to 1.0])\n\n";
	cout << "     -g ̰�� (��ֹ����heuistic���� [0.0 to 1.0] )\n\n";

	cout << "Example: GeoMatch -t Template.jpg -h 100 -l 10 -s Search1.jpg -m 0.7 -g 0.5 \n\n";
}
