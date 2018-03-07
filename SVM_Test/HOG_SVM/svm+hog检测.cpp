#include "stdafx.h"
#include <iostream>  
#include <fstream>  
#include <opencv2/opencv.hpp>
#include <string>

#define PosSamNO 3000    //正样本个数  
#define NegSamNO 3000    //负样本个数  
#define HardExampleNO 1000   //难例个数

void svm_hog_detect()
{
	//HOG检测器，用来计算HOG描述子的  
	//检测窗口(48,48),块尺寸(16,16),块步长(8,8),cell尺寸(8,8),直方图bin个数9  
	cv::HOGDescriptor hog(cv::Size(48, 48), cv::Size(16, 16), cv::Size(8, 8), cv::Size(8, 8), 9);

	//HOG描述子的维数，由图片大小、检测窗口大小、块大小、细胞单元中直方图bin个数决定 
	int DescriptorDim;

	//从XML文件读取训练好的SVM模型
	cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::load("SVM_HOG_C_H0.xml");

	if (svm->empty())
	{
		std::cout << "load svm detector failed!!!" << std::endl;
		return;
	}

	//特征向量的维数，即HOG描述子的维数  
	DescriptorDim = svm->getVarCount();

	//获取svecsmat，元素类型为float
	cv::Mat svecsmat = svm->getSupportVectors();
	//特征向量维数
	int svdim = svm->getVarCount();
	int numofsv = svecsmat.rows;

	//alphamat和svindex必须初始化，否则getDecisionFunction()函数会报错
	cv::Mat alphamat = cv::Mat::zeros(numofsv, svdim, CV_32F);
	cv::Mat svindex = cv::Mat::zeros(1, numofsv, CV_64F);

	cv::Mat Result;
	double rho = svm->getDecisionFunction(0, alphamat, svindex);
	//将alphamat元素的数据类型重新转成CV_32F
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

	/**************读入视频进行HOG检测******************/

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

	//用于保存检测结果
	out.open("test result.mp4", CV_FOURCC('D', 'I', 'V', 'X'), 25.0, cv::Size(width / 2, height / 2), true);

	for (int i = 0; i < frame_num; ++i)
	{
		capture >> frame;

		cv::resize(frame, frame, cv::Size(width / 2, height / 2));

		//目标矩形框数组 
		std::vector<cv::Rect> found, found_1, found_filtered;
		//对图片进行多尺度检测
		hog.detectMultiScale(frame, found, 0, cv::Size(8, 8), cv::Size(16, 16), 1.2, 2);

		for (int i = 0; i<found.size(); i++)
		{
			if (found[i].x > 0 && found[i].y > 0 && (found[i].x + found[i].width)< frame.cols
				&& (found[i].y + found[i].height)< frame.rows)
				found_1.push_back(found[i]);
		}

		//找出所有没有嵌套的矩形框r,并放入found_filtered中,如果有嵌套的话,
		//则取外面最大的那个矩形框放入found_filtered中  
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

		//画矩形框，因为hog检测出的矩形框比实际目标框要稍微大些,所以这里需要做一些调整，可根据实际情况调整  
		for (int i = 0; i<found_filtered.size(); i++)
		{
			cv::Rect r = found_filtered[i];
			//将检测矩形框缩小后绘制，根据实际情况调整
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

		//保存检测结果
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