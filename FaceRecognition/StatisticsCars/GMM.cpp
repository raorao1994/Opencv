/*
��ʵ��BackgroundSubtractorMOG��˹��ϱ���/ǰ���ָ��
*/
#include "stdafx.h"
#include "GMM.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <iostream>

GMM::GMM()
{
}


GMM::~GMM()
{
}
//���캯��
//���ͨ���޲ι��캯����Ĭ��ѵ��ģ��Ϊ200
GMM::GMM(int END_FRAME)
{
	this->END_FRAME = END_FRAME;
}
//GMM�����ʼ����������
void GMM::GMM_Init(Mat img)
{
	//��ʼ��Ϊ1
	fit_num = Mat(img.size(), CV_8UC1, -1);
	gmask = Mat(img.size(), CV_8UC1, -1);
	/****initialization the three parameters ****/
	for (int j = 0; j < GMM_MAX_COMPONT; j++)
	{
		//CV_32FC1������������������
		w[j] = Mat(img.size(), CV_32FC1, 0.0);
		//Ϊʲô���︳ֵΪ0ʱ������ľ�һֱ������ʱ����֪��ԭ���ȸ�ֵ-1����ʵ�ڲ��洢��Ҳ��0
		u[j] = Mat(img.size(), CV_8UC1, -1);
		//float����
		sigma[j] = Mat(img.size(), CV_32FC1, 0.0);
	}
}
//GMM��һ֡��ʼ����������
void GMM::GMM_First_Frame(Mat img)
{
	//��ʼ֡���������������ʼֵ
	for (int m = 0; m < img.rows; m++)
		for (int n = 0; n < img.cols; n++)
		{
			w[0].at<float>(m, n) = 1.0;
			//if the pixvel is gray-clever,then we should use unsigned char,not the unsigned int
			// һ��Ҫע��������ת���������ò���Ԥ�ڵĽ��
			u[0].at<unsigned char>(m, n) = img.at<unsigned char>(m, n);
			sigma[0].at<float>(m, n) = this->defaultsigma;
			for (int k = 1; k < GMM_MAX_COMPONT; k++)
			{
				/****when assigment this,we must be very carefully****/
				w[k].at<float>(m, n) = 0.0;
				u[k].at<unsigned char>(m, n) = -1;
				//��ֹ��������ʱ�з�ĸΪ0�����
				sigma[k].at<float>(m, n) = this->defaultsigma;
			}
		}
}
//GMMѵ�����̺�������
void GMM::GMM_Train(Mat img)
{
	//for loop images all pix 
	for (int m = 0; m < img.rows; m++)
		for (int n = 0; n < img.cols; n++)
		{
			int k = 0;
			int nfit = 0;
			//����Ȩ�ء���ֵ�ͷ���
			for (; k < GMM_MAX_COMPONT; k++)
			{
				//��ȡ���ص���ƽ�����ص�Ĳ�ֵ����ȡ����ֵ ��ֹ���
				int delam = abs(img.at<unsigned char>(m, n) - u[k].at<unsigned char>(m, n));
				//����ƽ����
				long dis = delam*delam;
				//the present(��ǰ) pixpel is fit(�ʺ�) the component(�ɷ�)
				if (dis < edit_threshold_w*sigma[k].at<float>(m, n))
				{
					/****update the weight****/
					w[k].at<float>(m, n) = 
						w[k].at<float>(m, n) + GMM_LEARN_ALPHA*(1 - w[k].at<float>(m, n));
					/****update the average****/
					u[k].at<unsigned char>(m, n) = 
						u[k].at<unsigned char>(m, n) + (GMM_LEARN_ALPHA / w[k].at<float>(m, n))*delam;
					/****update the variance****/
					sigma[k].at<float>(m, n) = 
						sigma[k].at<float>(m, n) + 
						(GMM_LEARN_ALPHA / w[k].at<float>(m, n))*(dis - sigma[k].at<float>(m, n));
				}
				else {
					w[k].at<float>(m, n) = 
						w[k].at<float>(m, n) + GMM_LEARN_ALPHA*(0 - w[k].at<float>(m, n));
					nfit++;
				}
			}

			////ѵ�����̼����㷨
			//for(int bb=k+1;bb<GMM_MAX_COMPONT;bb++)
			//{
			//    w[bb].at<float>(m,n)=w[bb].at<float>(m,n)+GMM_LEARN_ALPHA*(0-w[bb].at<float>(m,n));
			//    nfit++;
			//}

			//��gmm������˹��������,�Ӵ�С����,��������Ϊw/sigma
			for (int kk = 0; kk < GMM_MAX_COMPONT; kk++)
			{
				for (int rr = kk; rr < GMM_MAX_COMPONT; rr++)
				{
					//������������gmm�ṹ�����������أ�
					if (w[rr].at<float>(m, n) / sigma[rr].at<float>(m, n) > 
						w[kk].at<float>(m, n) / sigma[kk].at<float>(m, n))
					{
						//Ȩֵ����
						temp_w = w[rr].at<float>(m, n);
						w[rr].at<float>(m, n) = w[kk].at<float>(m, n);
						w[kk].at<float>(m, n) = temp_w;

						//��ֵ����
						temp_u = u[rr].at<unsigned char>(m, n);
						u[rr].at<unsigned char>(m, n) = u[kk].at<unsigned char>(m, n);
						u[kk].at<unsigned char>(m, n) = temp_u;

						//�����
						temp_sigma = sigma[rr].at<float>(m, n);
						sigma[rr].at<float>(m, n) = sigma[kk].at<float>(m, n);
						sigma[kk].at<float>(m, n) = temp_sigma;
					}
				}
			}

			//****���û�����������ĸ�˹�������¿�ʼ��һ����˹�ֲ�****/
			//if there is no exit component fit,then start a new componen
			if (nfit == GMM_MAX_COMPONT && 0 == w[GMM_MAX_COMPONT - 1].at<float>(m, n))
			{
				//����дΪfor(int h=0;h<MAX_GMM_COMPONT&&((0==w[h].at<float>(m,n)));h++)��
				//��Ϊ��������h����ÿ�ζ���1
				for (int h = 0; h < GMM_MAX_COMPONT; h++)
				{
					if ((0 == w[h].at<float>(m, n)))
					{
						w[h].at<float>(m, n) = GMM_LEARN_ALPHA;//�������ĵĲ�������
						u[h].at<unsigned char>(m, n) = (unsigned char)img.at<unsigned char>(m, n);
						//the opencv library code is 15.0
						sigma[h].at<float>(m, n) = this->defaultsigma;
						for (int q = 0; q < GMM_MAX_COMPONT&&q != h; q++)
						{
							/****update the other unfit's weight,u and sigma remain unchanged****/
							//normalization the weight,let they sum to 1
							w[q].at<float>(m, n) *= 1 - GMM_LEARN_ALPHA;
						}
						break;//�ҵ���һ��Ȩֵ��Ϊ0�ļ���
					}
				}
			}
			//���GMM_MAX_COMPONT��������ֵ�������������ĸ�˹����Ȩֵ�����ĸ�˹��
			//Ȩֵ���䣬ֻ���¾�ֵ�ͷ���
			else if (nfit == GMM_MAX_COMPONT&&w[GMM_MAX_COMPONT - 1].at<float>(m, n) != 0)
			{
				u[GMM_MAX_COMPONT - 1].at<unsigned char>(m, n) = (unsigned char)img.at<unsigned char>(m, n);
				//the opencv library code is 15.0
				sigma[GMM_MAX_COMPONT - 1].at<float>(m, n) = 15.0;
			}
		}
}
//������ͼ��ÿ������GMMѡ����ʵĸ�����������
void GMM::GMM_Fit_Num(Mat img)
{
	//float sum_w=0.0;//���¸�ֵΪ0������һ���������ۻ�
	for (int m = 0; m < img.rows; m++)
		for (int n = 0; n < img.cols; n++)
		{
			//���¸�ֵΪ0������һ���������ۻ�
			float sum_w = 0.0;
			//chose the fittest number fit_num
			for (unsigned char a = 0; a < GMM_MAX_COMPONT; a++)
			{
				//cout<<w[a].at<float>(m,n)<<endl;
				sum_w += w[a].at<float>(m, n);
				//�������THRESHOD_SUMW=0.6�Ļ�����ô�õ��ĸ�˹��Ŀ��Ϊ1����Ϊÿ�����ض���һ��Ȩֵ�ӽ�1
				if (sum_w >= GMM_THRESHOD_SUMW)
				{
					fit_num.at<unsigned char>(m, n) = a + 1;
					break;
				}
			}
		}
}
//GMM���Ժ�����ʵ��
void GMM::GMM_Test(Mat img)
{
	for (int m = 0; m < img.rows; m++)
		for (int n = 0; n < img.cols; n++)
		{
			unsigned char a = 0;
			for (; a < fit_num.at<unsigned char>(m, n); a++)
			{
				//�����sigmaȡ���ŵĻ�����֦����ǰ���ĸ��ʻ���󣬲����˱���������Ч��Ҳ����Щ����2��ˣ����ÿ�����Ч��������
				//if(abs(img.at<unsigned char>(m,n)-u[a].at<unsigned char>(m,n))<(unsigned char)(2*(sigma[a].at<float>(m,n))))
				if (abs(img.at<unsigned char>(m, n) - u[a].at<unsigned char>(m, n)) < (unsigned char)(2.5*(sigma[a].at<float>(m, n))))
				{
					gmask.at<unsigned char>(m, n) = 1;//����
					break;
				}
			}
			if (a == fit_num.at<unsigned char>(m, n))
				gmask.at<unsigned char>(m, n) = 255;//ǰ��
		}
}
//��ͨ��ȥ�뺯������
void GMM::Find_Connected_Components(Mat img)
{
	//�߼���̬�仯
	morphologyEx(gmask, gmask, MORPH_OPEN, Mat());
	//morphologyEx(gmask,gmask,MORPH_CLOSE,Mat());
}
//ѵ��
void GMM::Train(Mat img)
{
	if (NUM_FRAME == 0)
	{
		GMM_Init(img);
		GMM_First_Frame(img);
		NUM_FRAME++;
	}
	else
	{
		GMM_Train(img);
		NUM_FRAME++;
	}
	if (NUM_FRAME == END_FRAME)
	{
		GMM_Fit_Num(img);
	}
}
//����,���ض�ֵͼ��
Mat GMM::Apply(Mat img)
{
	GMM_Test(img);
	Find_Connected_Components(img);
	return gmask;
}
