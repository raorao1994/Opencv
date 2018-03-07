#include "stdafx.h"
#include <iostream>  
#include <fstream>  
#include <opencv2/opencv.hpp>
#include <string>

#define PosSamNO 3000    //����������  
#define NegSamNO 3000    //����������  
#define HardExampleNO 1000   //��������


void train_svm_hog()
{
	//HOG���������������HOG�����ӵ�
	//��ⴰ��(48,48),��ߴ�(16,16),�鲽��(8,8),cell�ߴ�(8,8),ֱ��ͼbin����9 
	cv::HOGDescriptor hog(cv::Size(48, 48), cv::Size(16, 16), cv::Size(8, 8), cv::Size(8, 8), 9);
	int DescriptorDim;//HOG�����ӵ�ά������ͼƬ��С����ⴰ�ڴ�С�����С��ϸ����Ԫ��ֱ��ͼbin��������  

					  //����SVM����
	cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::create();
	svm->setType(cv::ml::SVM::Types::C_SVC);
	svm->setKernel(cv::ml::SVM::KernelTypes::LINEAR);
	svm->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 1e-6));

	std::string ImgName;

	//������ͼƬ���ļ��б�
	std::ifstream finPos("positive_samples.txt");
	//������ͼƬ���ļ��б�
	std::ifstream finNeg("negative_samples.txt");

	//����ѵ������������������ɵľ��������������������ĸ�������������HOG������ά�� 
	cv::Mat sampleFeatureMat;
	//ѵ����������������������������������ĸ�������������1��1��ʾ��Ŀ�꣬-1��ʾ��Ŀ�� 
	cv::Mat sampleLabelMat;


	//���ζ�ȡ������ͼƬ������HOG������  
	for (int num = 0; num < PosSamNO && getline(finPos, ImgName); num++)
	{
		std::cout << "Processing��" << ImgName << std::endl;
		cv::Mat image = cv::imread(ImgName);

		//HOG���������� 
		std::vector<float> descriptors;
		//����HOG�����ӣ���ⴰ���ƶ�����(8,8)
		hog.compute(image, descriptors, cv::Size(8, 8));

		//�����һ������ʱ��ʼ�����������������������Ϊֻ��֪��������������ά�����ܳ�ʼ��������������  
		if (0 == num)
		{
			//HOG�����ӵ�ά�� 
			DescriptorDim = descriptors.size();
			//��ʼ������ѵ������������������ɵľ��������������������ĸ�������������HOG������ά��sampleFeatureMat  
			sampleFeatureMat = cv::Mat::zeros(PosSamNO + NegSamNO + HardExampleNO, DescriptorDim, CV_32FC1);
			//��ʼ��ѵ����������������������������������ĸ�������������1 
			sampleLabelMat = cv::Mat::zeros(PosSamNO + NegSamNO + HardExampleNO, 1, CV_32SC1);
		}

		//������õ�HOG�����Ӹ��Ƶ�������������sampleFeatureMat  
		for (int i = 0; i < DescriptorDim; i++)
		{
			//��num�����������������еĵ�i��Ԫ�� 
			sampleFeatureMat.at<float>(num, i) = descriptors[i];
		}
		//���������Ϊ1����Ŀ��	 
		sampleLabelMat.at<float>(num, 0) = 1;
	}

	//���ζ�ȡ������ͼƬ������HOG������  
	for (int num = 0; num < NegSamNO && getline(finNeg, ImgName); num++)
	{
		std::cout << "Processing��" << ImgName << std::endl;
		cv::Mat src = cv::imread(ImgName);
		cv::resize(src, src, cv::Size(48, 48));
		//HOG����������
		std::vector<float> descriptors;
		//����HOG�����ӣ���ⴰ���ƶ�����(8,8) 
		hog.compute(src, descriptors, cv::Size(8, 8));
		std::cout << "descriptor dimention��" << descriptors.size() << std::endl;
		//������õ�HOG�����Ӹ��Ƶ�������������sampleFeatureMat  
		for (int i = 0; i < DescriptorDim; i++)
		{
			//��PosSamNO+num�����������������еĵ�i��Ԫ��
			sampleFeatureMat.at<float>(num + PosSamNO, i) = descriptors[i];
		}

		//���������Ϊ-1����Ŀ��
		sampleLabelMat.at<float>(num + PosSamNO, 0) = -1;
	}

	//����HardExample������  
	if (HardExampleNO > 0)
	{
		//HardExample���������ļ��б� 
		std::ifstream finHardExample("hard_samples_d.txt");
		//���ζ�ȡHardExample������ͼƬ������HOG������  
		for (int num = 0; num < HardExampleNO && getline(finHardExample, ImgName); num++)
		{
			std::cout << "Processing��" << ImgName << std::endl;

			cv::Mat src = cv::imread(ImgName, cv::IMREAD_GRAYSCALE);
			cv::resize(src, src, cv::Size(48, 48));
			//HOG����������  
			std::vector<float> descriptors;
			//����HOG�����ӣ���ⴰ���ƶ�����(8,8) 
			hog.compute(src, descriptors, cv::Size(8, 8));

			//������õ�HOG�����Ӹ��Ƶ�������������sampleFeatureMat  
			for (int i = 0; i < DescriptorDim; i++)
			{
				//��PosSamNO+num�����������������еĵ�i��Ԫ��
				sampleFeatureMat.at<float>(num + PosSamNO + NegSamNO, i) = descriptors[i];
			}
			//���������Ϊ-1����Ŀ�� 
			sampleLabelMat.at<float>(num + PosSamNO + NegSamNO, 0) = -1;
		}
	}

	//ѵ��SVM������  
	//������ֹ��������������1000�λ����С��FLT_EPSILONʱֹͣ����  
	std::cout << "��ʼѵ��SVM������" << std::endl;
	cv::Ptr<cv::ml::TrainData> td = cv::ml::TrainData::create(sampleFeatureMat, cv::ml::SampleTypes::ROW_SAMPLE, sampleLabelMat);
	//ѵ��������  
	svm->train(td);
	std::cout << "ѵ�����" << std::endl;
	//��ѵ���õ�SVMģ�ͱ���Ϊxml�ļ�
	svm->save("SVM_HOG.xml");

	return;
}
