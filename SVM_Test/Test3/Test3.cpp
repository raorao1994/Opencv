// Test3.cpp : �������̨Ӧ�ó������ڵ㡣
//ʵ��SVM-���Բ��ɷֶࣨ������������
#include "stdafx.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/ml.hpp> 
#include <CTYPE.H>   


using namespace cv;
using namespace cv::ml;

// Number of training samples per class  
#define NTRAINING_SAMPLES   100
// Fraction of samples which compose the linear separable part 
#define FRAC_LINEAR_SEP     0.9f

int main()
{
	int size = 400; // height and widht of image    
	const int s = 1000; // number of data    
	int i, j, sv_num;
	IplImage* img;

	Ptr<SVM> svm=SVM::create();
	// Set up SVM's parameters 
	//	CV_PROP_RW int         svm_type;
	//	CV_PROP_RW int         kernel_type;
	//	CV_PROP_RW double      degree; // for poly
	//	CV_PROP_RW double      gamma;  // for poly/rbf/sigmoid
	//	CV_PROP_RW double      coef0;  // for poly/sigmoid
	//	CV_PROP_RW double      C;  // for CV_SVM_C_SVC,       CV_SVM_EPS_SVR and CV_SVM_NU_SVR
	//	CV_PROP_RW double      nu; // for CV_SVM_NU_SVC, CV_SVM_ONE_CLASS, and CV_SVM_NU_SVR
	//	CV_PROP_RW double      p; // for CV_SVM_EPS_SVR
	//	CvMat*      class_weights; // for CV_SVM_C_SVC
	//	CV_PROP_RW CvTermCriteria term_crit; // termination criteria
	//};
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::LINEAR);//LINEAR 
	svm->setDegree(10.0);
	svm->setGamma(8.0);
	svm->setCoef0(1.0);
	svm->setC(10.0);
	svm->setNu(0.5);
	svm->setP(0.1);
	svm->setTermCriteria(TermCriteria(TermCriteria::EPS, 1000, FLT_EPSILON));
	//param = CvSVMParams(CvSVM::C_SVC, CvSVM::RBF, 10.0, 8.0, 1.0, 10.0, 0.5, 0.1, NULL, criteria);
	CvTermCriteria criteria; // ֹͣ������׼    
	CvRNG rng = cvRNG();
	CvPoint pts[s]; // ����1000����    
	float data[s * 2]; // �������    
	int res[s]; // ������    

	CvMat data_mat, res_mat;
	//Mat data_mat, res_mat;
	CvScalar rcolor;

	const float* support;

	// ͼ������ĳ�ʼ��    
	img = cvCreateImage(cvSize(size, size), IPL_DEPTH_8U, 3);
	cvZero(img);

	// ѧϰ���ݵ�����    
	for (i = 0; i<s; ++i)
	{
		pts[i].x = cvRandInt(&rng) % size;
		pts[i].y = cvRandInt(&rng) % size;

		if (pts[i].y>50 * cos(pts[i].x*CV_PI / 100) + 200)
		{
			cvLine(img, cvPoint(pts[i].x - 2, pts[i].y - 2), cvPoint(pts[i].x + 2, pts[i].y + 2), CV_RGB(255, 0, 0));
			cvLine(img, cvPoint(pts[i].x + 2, pts[i].y - 2), cvPoint(pts[i].x - 2, pts[i].y + 2), CV_RGB(255, 0, 0));
			res[i] = 1;
		}
		else
		{
			if (pts[i].x>200)
			{
				cvLine(img, cvPoint(pts[i].x - 2, pts[i].y - 2), cvPoint(pts[i].x + 2, pts[i].y + 2), CV_RGB(0, 255, 0));
				cvLine(img, cvPoint(pts[i].x + 2, pts[i].y - 2), cvPoint(pts[i].x - 2, pts[i].y + 2), CV_RGB(0, 255, 0));
				res[i] = 2;
			}
			else
			{
				cvLine(img, cvPoint(pts[i].x - 2, pts[i].y - 2), cvPoint(pts[i].x + 2, pts[i].y + 2), CV_RGB(0, 0, 255));
				cvLine(img, cvPoint(pts[i].x + 2, pts[i].y - 2), cvPoint(pts[i].x - 2, pts[i].y + 2), CV_RGB(0, 0, 255));
				res[i] = 3;
			}
		}
	}

	// ѧϰ���ݵ���ʵ    
	cvNamedWindow("SVMѵ�������ռ估����", CV_WINDOW_AUTOSIZE);
	cvShowImage("SVMѵ�������ռ估����", img);
	cvWaitKey(0);

	// ѧϰ����������    
	for (i = 0; i<s; ++i)
	{
		data[i * 2] = float(pts[i].x) / size;
		data[i * 2 + 1] = float(pts[i].y) / size;
	}

	cvInitMatHeader(&data_mat, s, 2, CV_32FC1, data);
	cvInitMatHeader(&res_mat, s, 1, CV_32SC1, res);

	Mat data_mat1(data_mat.rows, data_mat.cols, CV_32FC1, data_mat.data.fl);
	Mat res_mat1(res_mat.rows, res_mat.cols, CV_32SC1, res_mat.data.fl);
	//data_mat1 = data_mat;
	svm->train(data_mat1, ROW_SAMPLE, res_mat1);
	svm->save("svm.xml");
	// ѧϰ�����ͼ    
	for (i = 0; i<size; i++)
	{
		for (j = 0; j<size; j++)
		{
			CvMat m;
			float ret = 0.0;
			float a[] = { float(j) / size,float(i) / size };
			cvInitMatHeader(&m, 1, 2, CV_32FC1, a);

			Mat m1(m.rows, m.cols, CV_32FC1, m.data.fl);
			ret = svm->predict(m1);

			switch ((int)ret)
			{
			case 1:
				rcolor = CV_RGB(100, 0, 0);
				break;
			case 2:
				rcolor = CV_RGB(0, 100, 0);
				break;
			case 3:
				rcolor = CV_RGB(0, 0, 100);
				break;
			}
			cvSet2D(img, i, j, rcolor);
		}
	}


	// Ϊ����ʾѧϰ�����ͨ��������ͼ��������������أ��������������з��࣬Ȼ��������������������ɫ�ȼ�����ɫ��ͼ    
	for (i = 0; i<s; ++i)
	{
		CvScalar rcolor;
		switch (res[i])
		{
		case 1:
			rcolor = CV_RGB(255, 0, 0);
			break;
		case 2:
			rcolor = CV_RGB(0, 255, 0);
			break;
		case 3:
			rcolor = CV_RGB(0, 0, 255);
			break;
		}
		cvLine(img, cvPoint(pts[i].x - 2, pts[i].y - 2), cvPoint(pts[i].x + 2, pts[i].y + 2), rcolor);
		cvLine(img, cvPoint(pts[i].x + 2, pts[i].y - 2), cvPoint(pts[i].x - 2, pts[i].y + 2), rcolor);
	}

	// ֧�������Ļ���  
	Mat sv = svm->getSupportVectors();
	sv_num = sv.rows;
	for (i = 0; i<sv_num; ++i)
	{
		support = sv.ptr<float>(i);
		cvCircle(img, cvPoint((int)(support[0] * size), (int)(support[i] * size)), 5, CV_RGB(200, 200, 200));
	}

	cvNamedWindow("SVM", CV_WINDOW_AUTOSIZE);
	cvShowImage("SVM��������֧������", img);
	cvWaitKey(0);
	cvDestroyWindow("SVM");
	cvReleaseImage(&img);
	return 0;
}

