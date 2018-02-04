// DynamicTargetMonitoring.cpp : �������̨Ӧ�ó������ڵ㡣
//����BackgroundSubtractorMOG2ʵ�ֶ�̬Ŀ����
#include "stdafx.h"
#include "opencv2/opencv.hpp"    
#include <vector>  

using namespace cv;
using namespace std;

const int Train = 200;
int main(int argc, char *argv[])
{
	char* videoFilename = "E:\\SVN\\OpenCV\\trunk\\Data\\video1.avi";
	Ptr<BackgroundSubtractorMOG2> mog = createBackgroundSubtractorMOG2(200, 25, false);
	//bgsubtractor->setVarThreshold(20);  
	Mat foreGround;
	Mat backGround;
	int trainCounter = 0;
	bool dynamicDetect = true;

	namedWindow("src");
	namedWindow("foreground");

	VideoCapture cap(videoFilename);//��Ĭ�ϵ�����ͷ      
	//VideoCapture cap(0);//��Ĭ�ϵ�����ͷ      
	if (!cap.isOpened())
	{
		return -1;
	}
	
	Mat src;
	bool stop = false;
	while (!stop)
	{
		cap >> src;
		if (dynamicDetect)
		{
			//mog->apply(backGround, foreGround, 0.005);
			mog->apply(src, foreGround, 0.005);
			//ͼ������� 
			//��ֵ�˲�
			medianBlur(foreGround, foreGround, 3);
			dilate(foreGround, foreGround, Mat(), Point(-1, -1), 3);
			erode(foreGround, foreGround, Mat(), Point(-1, -1), 6);
			dilate(foreGround, foreGround, Mat(), Point(-1, -1), 3);
			imshow("foreground", foreGround);
			if (trainCounter < Train)//ѵ���ڼ����ý��Ϊ��׼ȷ�������Ӧ��Ϊ����  
			{
				Mat findc;
				foreGround.copyTo(findc);
				vector<vector<Point>> contours;
				//Ѱ������
				cv::findContours(findc, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
				//targets.clear();  
				const int maxArea = 800;
				size_t s = contours.size();
				for (size_t i = 0; i < s; i++)
				{
					//contourArea����ȸ������
					double area = abs(contourArea(contours[i]));
					if (area > maxArea)
					{
						Rect mr = boundingRect(Mat(contours[i]));
						rectangle(src, mr, Scalar(0, 0, 255), 2, 8, 0);
						//targets.push_back(mr);  
					}
				}
				//string text;                    
				char text[50];
				sprintf_s(text, "background training -%d- ...", trainCounter);
				putText(src, text, Point(50, 50), 3, 1, Scalar(0, 255, 255), 2, 8, false);
				//delete[] text;  
			}
			else
			{
				//detects.clear();  
				Mat findc;
				foreGround.copyTo(findc);
				vector<vector<Point>> contours;
				cv::findContours(findc, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
				const int maxArea = 500;
				size_t s = contours.size();
				RNG rng;
					for (size_t i = 0; i < s; i++)
					{
						double area = abs(contourArea(contours[i]));
						if (area > maxArea)
						{
							Scalar sca_color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
							Rect mr = boundingRect(Mat(contours[i]));
							rectangle(src, mr, sca_color, 2, 8, 0);
							//���ԶԶ�̬Ŀ�������Ӧ����  
						}
					}
			}
			trainCounter++;
		}
		imshow("src", src);
		cout << "��" << trainCounter<<"����ͼƬ"<<endl;
		if (waitKey(30) == 27) //Esc���˳�      
		{
			stop = true;
		}
	}
	return 0;
}

