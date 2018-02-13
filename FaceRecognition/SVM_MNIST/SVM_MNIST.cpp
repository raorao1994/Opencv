// SVM_MNIST.cpp : �������̨Ӧ�ó������ڵ㡣
//��SVMѵ��mnist����
/*
svm_type �C
ָ��SVM�����ͣ������ǿ��ܵ�ȡֵ��
CvSVM::C_SVC C��֧������������� n�����  (n \geq 2)���������쳣ֵ�ͷ�����C���в���ȫ���ࡣ
CvSVM::NU_SVC \nu��֧�������������n����Ȼ����ȫ����ķ�����������Ϊ \nu ȡ��C����ֵ�����䡾0��1���У�nuԽ�󣬾��߽߱�Խƽ������
CvSVM::ONE_CLASS �������������е�ѵ��������ȡ��ͬһ�����Ȼ��SVM������һ���ֽ����Էָ�����������ռ�����ռ������������������ռ�����ռ����
CvSVM::EPS_SVR \epsilon��֧�������ع����ѵ�����е�������������ϳ����ĳ�ƽ��ľ�����ҪС��p���쳣ֵ�ͷ�����C�����á�
CvSVM::NU_SVR \nu��֧�������ع���� \nu ������ p��

�ɴ� [LibSVM] ��ȡ����ϸ�ڡ�

kernel_type �C
SVM���ں����ͣ������ǿ��ܵ�ȡֵ��
CvSVM::LINEAR �����ںˡ�û���κ���ӳ������ά�ռ䣬�������֣���ع飩��ԭʼ�����ռ��б���ɣ���������ѡ��K(x_i, x_j) = x_i^T x_j.
CvSVM::POLY ����ʽ�ںˣ� K(x_i, x_j) = (\gamma x_i^T x_j + coef0)^{degree}, \gamma > 0.
CvSVM::RBF ���ھ���ĺ��������ڴ�����������һ���Ϻõ�ѡ�� K(x_i, x_j) = e^{-\gamma ||x_i - x_j||^2}, \gamma > 0.
CvSVM::SIGMOID Sigmoid�����ںˣ�K(x_i, x_j) = \tanh(\gamma x_i^T x_j + coef0).

degree �C �ں˺�����POLY���Ĳ���degree��

gamma �C �ں˺�����POLY/ RBF/ SIGMOID���Ĳ���\gamma��

coef0 �C �ں˺�����POLY/ SIGMOID���Ĳ���coef0��

Cvalue �C SVM���ͣ�C_SVC/ EPS_SVR/ NU_SVR���Ĳ���C��

nu �C SVM���ͣ�NU_SVC/ ONE_CLASS/ NU_SVR���Ĳ��� \nu��

p �C SVM���ͣ�EPS_SVR���Ĳ��� \epsilon��

class_weights �C C_SVC�еĿ�ѡȨ�أ�����ָ�����࣬����C�Ժ��� class\_weights_i * C��������ЩȨ��Ӱ�첻ͬ���Ĵ������ͷ��Ȩ��Խ��ĳһ������������ݵĳͷ����Խ��

term_crit �C SVM�ĵ���ѵ�����̵���ֹ���������������Լ�������������⡣������ָ���Ĺ����/��������������

*/
#include "stdafx.h"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/ml.hpp>
#include <string>
#include <iostream>
#include "mnist.h"

using namespace std;
using namespace cv;
using namespace cv::ml;

string trainImage = "mnist_dataset/train-images-idx3-ubyte";
string trainLabel = "mnist_dataset/train-labels-idx1-ubyte";
string testImage = "mnist_dataset/t10k-images-idx3-ubyte";
string testLabel = "mnist_dataset/t10k-labels-idx1-ubyte";
//string testImage = "mnist_dataset/train-images.idx3-ubyte";
//string testLabel = "mnist_dataset/train-labels.idx1-ubyte";

//��ʱ��
double cost_time_;
clock_t start_time_;
clock_t end_time_;


int main()
{
	//--------------------- 1. Set up training data ---------------------------------------
	Mat trainData;
	Mat labels;
	trainData = read_mnist_image(trainImage);
	labels = read_mnist_label(trainLabel);
	cout << trainData.rows << " " << trainData.cols << endl;
	cout << labels.rows << " " << labels.cols << endl;

	//------------------------ 2. Set up the support vector machines parameters --------------------
	Ptr<SVM> svm = SVM::create();
	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::RBF);
	//svm->setDegree(10.0);
	svm->setGamma(0.01);
	//svm->setCoef0(1.0);
	svm->setC(10.0);
	//svm->setNu(0.5);
	//svm->setP(0.1);
	svm->setTermCriteria(TermCriteria(CV_TERMCRIT_EPS, 1000, FLT_EPSILON));

	//------------------------ 3. Train the svm ----------------------------------------------------
	cout << "Starting training process" << endl;
	start_time_ = clock();
	svm->train(trainData, ROW_SAMPLE, labels);
	end_time_ = clock();
	cost_time_ = (end_time_ - start_time_) / CLOCKS_PER_SEC;
	cout << "Finished training process...cost " << cost_time_ << " seconds..." << endl;

	//------------------------ 4. save the svm ----------------------------------------------------
	svm->save("mnist_dataset/mnist_svm.xml");
	cout << "save as /mnist_dataset/mnist_svm.xml" << endl;
	//------------------------ 5. load the svm ----------------------------------------------------
	cout << "��ʼ����SVM�ļ�...\n";
	Ptr<SVM> svm1 = StatModel::load<SVM>("mnist_dataset/mnist_svm.xml");
	cout << "�ɹ�����SVM�ļ�...\n";


	//------------------------ 6. read the test dataset -------------------------------------------
	cout << "��ʼ�����������...\n";
	Mat testData;
	Mat tLabel;
	testData = read_mnist_image(testImage);
	tLabel = read_mnist_label(testLabel);
	cout << "�ɹ�����������ݣ�����\n";


	float count = 0;
	for (int i = 0; i < testData.rows; i++) {
		Mat sample = testData.row(i);
		float res = svm1->predict(sample);
		res = std::abs(res - tLabel.at<int>(i, 0)) <= FLT_EPSILON ? 1.f : 0.f;
		count += res;
	}
	cout << "��ȷ��ʶ����� count = " << count << endl;
	cout << "������Ϊ..." << (10000 - count + 0.0) / 10000 * 100.0 << "%....\n";

	system("pause");
	return 0;
}

