/*
自实现BackgroundSubtractorMOG高斯混合背景/前景分割方法
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
//构造函数
//如果通过无参构造函数，默认训练模型为200
GMM::GMM(int END_FRAME)
{
	this->END_FRAME = END_FRAME;
}
//GMM整体初始化函数声明
void GMM::GMM_Init(Mat img)
{
	//初始化为1
	fit_num = Mat(img.size(), CV_8UC1, -1);
	gmask = Mat(img.size(), CV_8UC1, -1);
	/****initialization the three parameters ****/
	for (int j = 0; j < GMM_MAX_COMPONT; j++)
	{
		//CV_32FC1本身体现了正负符号
		w[j] = Mat(img.size(), CV_32FC1, 0.0);
		//为什么这里赋值为0时，后面的就一直出错？暂时还不知道原因，先赋值-1，其实内部存储的也是0
		u[j] = Mat(img.size(), CV_8UC1, -1);
		//float类型
		sigma[j] = Mat(img.size(), CV_32FC1, 0.0);
	}
}
//GMM第一帧初始化函数声明
void GMM::GMM_First_Frame(Mat img)
{
	//初始帧，各项参数均赋初始值
	for (int m = 0; m < img.rows; m++)
		for (int n = 0; n < img.cols; n++)
		{
			w[0].at<float>(m, n) = 1.0;
			//if the pixvel is gray-clever,then we should use unsigned char,not the unsigned int
			// 一定要注意其类型转换，否则会得不得预期的结果
			u[0].at<unsigned char>(m, n) = img.at<unsigned char>(m, n);
			sigma[0].at<float>(m, n) = this->defaultsigma;
			for (int k = 1; k < GMM_MAX_COMPONT; k++)
			{
				/****when assigment this,we must be very carefully****/
				w[k].at<float>(m, n) = 0.0;
				u[k].at<unsigned char>(m, n) = -1;
				//防止后面排序时有分母为0的情况
				sigma[k].at<float>(m, n) = this->defaultsigma;
			}
		}
}
//GMM训练过程函数声明
void GMM::GMM_Train(Mat img)
{
	//for loop images all pix 
	for (int m = 0; m < img.rows; m++)
		for (int n = 0; n < img.cols; n++)
		{
			int k = 0;
			int nfit = 0;
			//更新权重、均值和方差
			for (; k < GMM_MAX_COMPONT; k++)
			{
				//获取像素点与平均像素点的差值，并取绝对值 防止溢出
				int delam = abs(img.at<unsigned char>(m, n) - u[k].at<unsigned char>(m, n));
				//计算平方差
				long dis = delam*delam;
				//the present(当前) pixpel is fit(适合) the component(成分)
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

			////训练过程加速算法
			//for(int bb=k+1;bb<GMM_MAX_COMPONT;bb++)
			//{
			//    w[bb].at<float>(m,n)=w[bb].at<float>(m,n)+GMM_LEARN_ALPHA*(0-w[bb].at<float>(m,n));
			//    nfit++;
			//}

			//对gmm各个高斯进行排序,从大到小排序,排序依据为w/sigma
			for (int kk = 0; kk < GMM_MAX_COMPONT; kk++)
			{
				for (int rr = kk; rr < GMM_MAX_COMPONT; rr++)
				{
					//怎样才能做到gmm结构体整体排序呢？
					if (w[rr].at<float>(m, n) / sigma[rr].at<float>(m, n) > 
						w[kk].at<float>(m, n) / sigma[kk].at<float>(m, n))
					{
						//权值交换
						temp_w = w[rr].at<float>(m, n);
						w[rr].at<float>(m, n) = w[kk].at<float>(m, n);
						w[kk].at<float>(m, n) = temp_w;

						//均值交换
						temp_u = u[rr].at<unsigned char>(m, n);
						u[rr].at<unsigned char>(m, n) = u[kk].at<unsigned char>(m, n);
						u[kk].at<unsigned char>(m, n) = temp_u;

						//方差交换
						temp_sigma = sigma[rr].at<float>(m, n);
						sigma[rr].at<float>(m, n) = sigma[kk].at<float>(m, n);
						sigma[kk].at<float>(m, n) = temp_sigma;
					}
				}
			}

			//****如果没有满足条件的高斯，则重新开始算一个高斯分布****/
			//if there is no exit component fit,then start a new componen
			if (nfit == GMM_MAX_COMPONT && 0 == w[GMM_MAX_COMPONT - 1].at<float>(m, n))
			{
				//不能写为for(int h=0;h<MAX_GMM_COMPONT&&((0==w[h].at<float>(m,n)));h++)，
				//因为这样明显h不会每次都加1
				for (int h = 0; h < GMM_MAX_COMPONT; h++)
				{
					if ((0 == w[h].at<float>(m, n)))
					{
						w[h].at<float>(m, n) = GMM_LEARN_ALPHA;//按照论文的参数来的
						u[h].at<unsigned char>(m, n) = (unsigned char)img.at<unsigned char>(m, n);
						//the opencv library code is 15.0
						sigma[h].at<float>(m, n) = this->defaultsigma;
						for (int q = 0; q < GMM_MAX_COMPONT&&q != h; q++)
						{
							/****update the other unfit's weight,u and sigma remain unchanged****/
							//normalization the weight,let they sum to 1
							w[q].at<float>(m, n) *= 1 - GMM_LEARN_ALPHA;
						}
						break;//找到第一个权值不为0的即可
					}
				}
			}
			//如果GMM_MAX_COMPONT都曾经赋值过，则用新来的高斯代替权值最弱的高斯，
			//权值不变，只更新均值和方差
			else if (nfit == GMM_MAX_COMPONT&&w[GMM_MAX_COMPONT - 1].at<float>(m, n) != 0)
			{
				u[GMM_MAX_COMPONT - 1].at<unsigned char>(m, n) = (unsigned char)img.at<unsigned char>(m, n);
				//the opencv library code is 15.0
				sigma[GMM_MAX_COMPONT - 1].at<float>(m, n) = 15.0;
			}
		}
}
//对输入图像每个像素GMM选择合适的个数函数声明
void GMM::GMM_Fit_Num(Mat img)
{
	//float sum_w=0.0;//重新赋值为0，给下一个像素做累积
	for (int m = 0; m < img.rows; m++)
		for (int n = 0; n < img.cols; n++)
		{
			//重新赋值为0，给下一个像素做累积
			float sum_w = 0.0;
			//chose the fittest number fit_num
			for (unsigned char a = 0; a < GMM_MAX_COMPONT; a++)
			{
				//cout<<w[a].at<float>(m,n)<<endl;
				sum_w += w[a].at<float>(m, n);
				//如果这里THRESHOD_SUMW=0.6的话，那么得到的高斯数目都为1，因为每个像素都有一个权值接近1
				if (sum_w >= GMM_THRESHOD_SUMW)
				{
					fit_num.at<unsigned char>(m, n) = a + 1;
					break;
				}
			}
		}
}
//GMM测试函数的实现
void GMM::GMM_Test(Mat img)
{
	for (int m = 0; m < img.rows; m++)
		for (int n = 0; n < img.cols; n++)
		{
			unsigned char a = 0;
			for (; a < fit_num.at<unsigned char>(m, n); a++)
			{
				//如果对sigma取根号的话，树枝当做前景的概率会更大，不过人被检测出来的效果也更好些；用2相乘，不用开根号效果还不错
				//if(abs(img.at<unsigned char>(m,n)-u[a].at<unsigned char>(m,n))<(unsigned char)(2*(sigma[a].at<float>(m,n))))
				if (abs(img.at<unsigned char>(m, n) - u[a].at<unsigned char>(m, n)) < (unsigned char)(2.5*(sigma[a].at<float>(m, n))))
				{
					gmask.at<unsigned char>(m, n) = 1;//背景
					break;
				}
			}
			if (a == fit_num.at<unsigned char>(m, n))
				gmask.at<unsigned char>(m, n) = 255;//前景
		}
}
//连通域去噪函数声明
void GMM::Find_Connected_Components(Mat img)
{
	//高级形态变化
	morphologyEx(gmask, gmask, MORPH_OPEN, Mat());
	//morphologyEx(gmask,gmask,MORPH_CLOSE,Mat());
}
//训练
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
//测试,返回二值图像
Mat GMM::Apply(Mat img)
{
	GMM_Test(img);
	Find_Connected_Components(img);
	return gmask;
}
