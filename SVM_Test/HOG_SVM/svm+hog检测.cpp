#include "stdafx.h"
#include <iostream>  
#include <fstream>  
#include <opencv2/opencv.hpp>
#include <string>

#define PosSamNO 3000    //����������  
#define NegSamNO 3000    //����������  
#define HardExampleNO 1000   //��������

void svm_hog_detect()
{
	//HOG���������������HOG�����ӵ�  
	//��ⴰ��(48,48),��ߴ�(16,16),�鲽��(8,8),cell�ߴ�(8,8),ֱ��ͼbin����9  
	cv::HOGDescriptor hog(cv::Size(48, 48), cv::Size(16, 16), cv::Size(8, 8), cv::Size(8, 8), 9);

	//HOG�����ӵ�ά������ͼƬ��С����ⴰ�ڴ�С�����С��ϸ����Ԫ��ֱ��ͼbin�������� 
	int DescriptorDim;

	//��XML�ļ���ȡѵ���õ�SVMģ��
	cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::load("SVM_HOG_C_H0.xml");

	if (svm->empty())
	{
		std::cout << "load svm detector failed!!!" << std::endl;
		return;
	}

	//����������ά������HOG�����ӵ�ά��  
	DescriptorDim = svm->getVarCount();

	//��ȡsvecsmat��Ԫ������Ϊfloat
	cv::Mat svecsmat = svm->getSupportVectors();
	//��������ά��
	int svdim = svm->getVarCount();
	int numofsv = svecsmat.rows;

	//alphamat��svindex�����ʼ��������getDecisionFunction()�����ᱨ��
	cv::Mat alphamat = cv::Mat::zeros(numofsv, svdim, CV_32F);
	cv::Mat svindex = cv::Mat::zeros(1, numofsv, CV_64F);

	cv::Mat Result;
	double rho = svm->getDecisionFunction(0, alphamat, svindex);
	//��alphamatԪ�ص�������������ת��CV_32F
	alphamat.convertTo(alphamat, CV_32F);
	Result = -1 * alphamat * svecsmat;

	std::vector<float> vec;
	for (int i = 0; i < svdim; ++i)
	{
		vec.push_back(Result.at<float>(0, i));
	}
	vec.push_back(rho);

	//saving HOGDetectorForOpenCV.txt
	std::ofstream fout("HOGDetectorForOpenCV.txt");
	for (int i = 0; i < vec.size(); ++i)
	{
		fout << vec[i] << std::endl;
	}

	hog.setSVMDetector(vec);

	/**************������Ƶ����HOG���******************/

	cv::VideoCapture capture("video.avi");

	if (!capture.isOpened())
	{
		std::cout << "Read video Failed !" << std::endl;
		return;
	}

	cv::Mat frame;

	int frame_num = capture.get(cv::CAP_PROP_FRAME_COUNT);
	std::cout << "total frame number is: " << frame_num << std::endl;


	int width = capture.get(cv::CAP_PROP_FRAME_WIDTH);
	int height = capture.get(cv::CAP_PROP_FRAME_HEIGHT);

	cv::VideoWriter out;

	//���ڱ�������
	out.open("test result.mp4", CV_FOURCC('D', 'I', 'V', 'X'), 25.0, cv::Size(width / 2, height / 2), true);

	for (int i = 0; i < frame_num; ++i)
	{
		capture >> frame;

		cv::resize(frame, frame, cv::Size(width / 2, height / 2));

		//Ŀ����ο����� 
		std::vector<cv::Rect> found, found_1, found_filtered;
		//��ͼƬ���ж�߶ȼ��
		hog.detectMultiScale(frame, found, 0, cv::Size(8, 8), cv::Size(16, 16), 1.2, 2);

		for (int i = 0; i<found.size(); i++)
		{
			if (found[i].x > 0 && found[i].y > 0 && (found[i].x + found[i].width)< frame.cols
				&& (found[i].y + found[i].height)< frame.rows)
				found_1.push_back(found[i]);
		}

		//�ҳ�����û��Ƕ�׵ľ��ο�r,������found_filtered��,�����Ƕ�׵Ļ�,
		//��ȡ���������Ǹ����ο����found_filtered��  
		for (int i = 0; i < found_1.size(); i++)
		{
			cv::Rect r = found_1[i];
			int j = 0;
			for (; j < found_1.size(); j++)
				if (j != i && (r & found_1[j]) == found_1[j])
					break;
			if (j == found_1.size())
				found_filtered.push_back(r);
		}

		//�����ο���Ϊhog�����ľ��ο��ʵ��Ŀ���Ҫ��΢��Щ,����������Ҫ��һЩ�������ɸ���ʵ���������  
		for (int i = 0; i<found_filtered.size(); i++)
		{
			cv::Rect r = found_filtered[i];
			//�������ο���С����ƣ�����ʵ���������
			r.x += cvRound(r.width*0.1);
			r.width = cvRound(r.width*0.8);
			r.y += cvRound(r.height*0.1);
			r.height = cvRound(r.height*0.8);
		}

		for (int i = 0; i<found_filtered.size(); i++)
		{
			cv::Rect r = found_filtered[i];

			cv::rectangle(frame, r.tl(), r.br(), cv::Scalar(0, 0, 255), 2);

		}
		cv::imshow("detect result", frame);

		//��������
		out << frame;

		if (cv::waitKey(30) == 'q')
		{
			break;
		}
	}
	capture.release();
	out.release();
	return;
}