//***********************************************************************
// File Name		: GeoMatch.h
// Last Modified By : raorao
// Last Modified On : 03-08-2018
// Description      : 实现基于边缘的模板匹配
//***********************************************************************

#include "StdAfx.h"
#include "GeoMatch1.h"


GeoMatch1::GeoMatch1()
{
	// 模型点中cppodinate的初始NO
	noOfCordinates = 0;
	modelDefined = false;
}
//创建模版匹配模版
int GeoMatch1::CreateGeoMatchModel(const void *templateArr, double maxContrast, double minContrast)
{

	CvMat *gx = 0;		//存储X导数的矩阵
	CvMat *gy = 0;		//存储Y导数的矩阵
	CvMat *nmsEdges = 0;//存储临时RESTOR的矩阵
	CvSize Ssize;

	// 将IPL图像转换为整数操作的矩阵
	CvMat srcstub, *src = (CvMat*)templateArr;
	src = cvGetMat(src, &srcstub);
	if (CV_MAT_TYPE(src->type) != CV_8UC1)
	{
		return 0;
	}

	// set width and height
	Ssize.width = src->width;
	Ssize.height = src->height;
	modelHeight = src->height;		//保存模版高度
	modelWidth = src->width;			//保存模版宽度

	noOfCordinates = 0;	//初始化
	//为模板图像中所选点的协同工作分配内存
	cordinates = new CvPoint[modelWidth *modelHeight];
	//为选定点的边缘大小分配内存
	edgeMagnitude = new double[modelWidth *modelHeight];
	//为选定点分配边缘X导数的内存
	edgeDerivativeX = new double[modelWidth *modelHeight];
	//为选定的点分配边缘y衍生物的内存
	edgeDerivativeY = new double[modelWidth *modelHeight];			


	//计算模板梯度							
	gx = cvCreateMat(Ssize.height, Ssize.width, CV_16SC1);
	gy = cvCreateMat(Ssize.height, Ssize.width, CV_16SC1);
	cvSobel(src, gx, 1, 0, 3);		//X方向梯度
	cvSobel(src, gy, 0, 1, 3);	//Y方向梯度
	//创建矩阵以存储最终的nmsEdges
	nmsEdges = cvCreateMat(Ssize.height, Ssize.width, CV_32F);		
	const short* _sdx;//x剃度值
	const short* _sdy;//y剃度值
	double fdx, fdy;
	double MagG, DirG;//xy剃度距离
	double MaxGradient = -99999.99;
	double direction;//梯度角度
	int *orients = new int[Ssize.height *Ssize.width];
	int count = 0, i, j; // count variable;

	double **magMat;
	CreateDoubleMatrix(magMat, Ssize);
	//梯度计算
	for (i = 1; i < Ssize.height - 1; i++)
	{
		for (j = 1; j < Ssize.width - 1; j++)
		{
			_sdx = (short*)(gx->data.ptr + gx->step*i);
			_sdy = (short*)(gy->data.ptr + gy->step*i);
			fdx = _sdx[j]; 
			fdy = _sdy[j]; //读取XY导数read x, y derivatives

			MagG = sqrt((float)(fdx*fdx) + (float)(fdy*fdy)); //Magnitude = Sqrt(gx^2 +gy^2)
			direction = cvFastArctan((float)fdy, (float)fdx);	 //Direction = invtan (Gy / Gx)
			magMat[i][j] = MagG;

			if (MagG>MaxGradient)
				//获取归一化的最大梯度值。
				MaxGradient = MagG;


			//从0，45，90，135得到最近的角度
			if ((direction>0 && direction < 22.5) || (direction >157.5 && direction < 202.5) || (direction>337.5 && direction<360))
				direction = 0;
			else if ((direction>22.5 && direction < 67.5) || (direction >202.5 && direction <247.5))
				direction = 45;
			else if ((direction >67.5 && direction < 112.5) || (direction>247.5 && direction<292.5))
				direction = 90;
			else if ((direction >112.5 && direction < 157.5) || (direction>292.5 && direction<337.5))
				direction = 135;
			else
				direction = 0;

			orients[count] = (int)direction;
			count++;
		}
	}
	//初始化计数器，
	count = 0; 
	//非最大抑制
	double leftPixel, rightPixel;

	for (i = 1; i < Ssize.height - 1; i++)
	{
		for (j = 1; j < Ssize.width - 1; j++)
		{
			switch (orients[count])
			{
			case 0:
				leftPixel = magMat[i][j - 1];
				rightPixel = magMat[i][j + 1];
				break;
			case 45:
				leftPixel = magMat[i - 1][j + 1];
				rightPixel = magMat[i + 1][j - 1];
				break;
			case 90:
				leftPixel = magMat[i - 1][j];
				rightPixel = magMat[i + 1][j];
				break;
			case 135:
				leftPixel = magMat[i - 1][j - 1];
				rightPixel = magMat[i + 1][j + 1];
				break;
			}
			//比较当前像素值和相邻像素
			if ((magMat[i][j] < leftPixel) || (magMat[i][j] < rightPixel))
				(nmsEdges->data.ptr + nmsEdges->step*i)[j] = 0;
			else
				(nmsEdges->data.ptr + nmsEdges->step*i)[j] = (uchar)(magMat[i][j] / MaxGradient * 255);

			count++;
		}
	}

	int RSum = 0, CSum = 0;
	int curX, curY;
	int flag = 1;

	//Hysterisis 阀值
	for (i = 1; i < Ssize.height - 1; i++)
	{
		for (j = 1; j < Ssize.width; j++)
		{
			_sdx = (short*)(gx->data.ptr + gx->step*i);
			_sdy = (short*)(gy->data.ptr + gy->step*i);
			fdx = _sdx[j]; fdy = _sdy[j];

			MagG = sqrt(fdx*fdx + fdy*fdy); //Magnitude = Sqrt(gx^2 +gy^2)
			DirG = cvFastArctan((float)fdy, (float)fdx);	 //Direction = tan(y/x)

			 ////((uchar*)(imgGDir->imageData + imgGDir->widthStep*i))[j]= MagG;
			flag = 1;
			if (((double)((nmsEdges->data.ptr + nmsEdges->step*i))[j]) < maxContrast)
			{
				if (((double)((nmsEdges->data.ptr + nmsEdges->step*i))[j])< minContrast)
				{

					(nmsEdges->data.ptr + nmsEdges->step*i)[j] = 0;
					flag = 0; // remove from edge
					////((uchar*)(imgGDir->imageData + imgGDir->widthStep*i))[j]=0;
				}
				else
				{   // if any of 8 neighboring pixel is not greater than max contraxt remove from edge
					if ((((double)((nmsEdges->data.ptr + nmsEdges->step*(i - 1)))[j - 1]) < maxContrast) &&
						(((double)((nmsEdges->data.ptr + nmsEdges->step*(i - 1)))[j]) < maxContrast) &&
						(((double)((nmsEdges->data.ptr + nmsEdges->step*(i - 1)))[j + 1]) < maxContrast) &&
						(((double)((nmsEdges->data.ptr + nmsEdges->step*i))[j - 1]) < maxContrast) &&
						(((double)((nmsEdges->data.ptr + nmsEdges->step*i))[j + 1]) < maxContrast) &&
						(((double)((nmsEdges->data.ptr + nmsEdges->step*(i + 1)))[j - 1]) < maxContrast) &&
						(((double)((nmsEdges->data.ptr + nmsEdges->step*(i + 1)))[j]) < maxContrast) &&
						(((double)((nmsEdges->data.ptr + nmsEdges->step*(i + 1)))[j + 1]) < maxContrast))
					{
						(nmsEdges->data.ptr + nmsEdges->step*i)[j] = 0;
						flag = 0;
						////((uchar*)(imgGDir->imageData + imgGDir->widthStep*i))[j]=0;
					}
				}

			}

			//保存选定的边缘信息
			curX = i;	curY = j;
			if (flag != 0)
			{
				if (fdx != 0 || fdy != 0)
				{
					// 重心行和和列和
					RSum = RSum + curX;	CSum = CSum + curY; 

					cordinates[noOfCordinates].x = curX;
					cordinates[noOfCordinates].y = curY;
					edgeDerivativeX[noOfCordinates] = fdx;
					edgeDerivativeY[noOfCordinates] = fdy;

					//手柄除以零handle divide by zero
					if (MagG != 0)
						edgeMagnitude[noOfCordinates] = 1 / MagG;  //梯度级
					else
						edgeMagnitude[noOfCordinates] = 0;

					noOfCordinates++;
				}
			}
		}
	}

	centerOfGravity.x = RSum / noOfCordinates; // 重心
	centerOfGravity.y = CSum / noOfCordinates;	// 重心

	//改变坐标以反映重心
	for (int m = 0; m<noOfCordinates; m++)
	{
		int temp;

		temp = cordinates[m].x;
		cordinates[m].x = temp - centerOfGravity.x;
		temp = cordinates[m].y;
		cordinates[m].y = temp - centerOfGravity.y;
	}

	////cvSaveImage("Edges.bmp",imgGDir);

	// free alocated memories
	delete[] orients;
	////cvReleaseImage(&imgGDir);
	cvReleaseMat(&gx);
	cvReleaseMat(&gy);
	cvReleaseMat(&nmsEdges);

	ReleaseDoubleMatrix(magMat, Ssize.height);
	modelDefined = true;
	return 1;
}

double GeoMatch1::FindGeoMatchModel(const void* srcarr, double minScore, double greediness, CvPoint *resultPoint)
{
	CvMat *Sdx = 0, *Sdy = 0;
	double resultScore = 0;
	double partialSum = 0;
	double sumOfCoords = 0;
	double partialScore;
	const short* _Sdx;
	const short* _Sdy;
	int i, j, m;			//计数变量
	double iTx, iTy, iSx, iSy;
	double gradMag;
	int curX, curY;

	double **matGradMag;  //梯度场矩阵 

	CvMat srcstub, *src = (CvMat*)srcarr;
	src = cvGetMat(src, &srcstub);
	if (CV_MAT_TYPE(src->type) != CV_8UC1 || !modelDefined)
	{
		return 0;
	}

	// 源图片大小
	CvSize Ssize;
	Ssize.width = src->width;
	Ssize.height = src->height;
	//创建图像以保存梯度幅值
	CreateDoubleMatrix(matGradMag, Ssize); 

	Sdx = cvCreateMat(Ssize.height, Ssize.width, CV_16SC1); // X 导数
	Sdy = cvCreateMat(Ssize.height, Ssize.width, CV_16SC1); // y 导数

	cvSobel(src, Sdx, 1, 0, 3);  // find X 梯度
	cvSobel(src, Sdy, 0, 1, 3); // find Y 梯度

	// 寻找模型的回采判据
	// 预计最小分数
	double normMinScore = minScore / noOfCordinates; 
	// 预计 greedniness 
	double normGreediness = ((1 - greediness * minScore) / (1 - greediness)) / noOfCordinates; 

	for (i = 0; i < Ssize.height; i++)
	{
		_Sdx = (short*)(Sdx->data.ptr + Sdx->step*(i));
		_Sdy = (short*)(Sdy->data.ptr + Sdy->step*(i));

		for (j = 0; j < Ssize.width; j++)
		{
			iSx = _Sdx[j];  // X derivative of Source image
			iSy = _Sdy[j];  // Y derivative of Source image

			gradMag = sqrt((iSx*iSx) + (iSy*iSy)); //Magnitude = Sqrt(dx^2 +dy^2)

			if (gradMag != 0) // hande divide by zero
				matGradMag[i][j] = 1 / gradMag;   // 1/Sqrt(dx^2 +dy^2)
			else
				matGradMag[i][j] = 0;

		}
	}
	for (i = 0; i < Ssize.height; i++)
	{
		for (j = 0; j < Ssize.width; j++)
		{
			// 初始化部分和测度
			partialSum = 0; 
			for (m = 0; m<noOfCordinates; m++)
			{
				curX = i + cordinates[m].x;	// template X coordinate
				curY = j + cordinates[m].y; // template Y coordinate
				iTx = edgeDerivativeX[m];	// template X derivative
				iTy = edgeDerivativeY[m];    // template Y derivative

				if (curX<0 || curY<0 || curX>Ssize.height - 1 || curY>Ssize.width - 1)
					continue;

				_Sdx = (short*)(Sdx->data.ptr + Sdx->step*(curX));
				_Sdy = (short*)(Sdy->data.ptr + Sdy->step*(curX));

				// 从源图像获取曲线响应XY导数

				iSx = _Sdx[curY]; 
				iSy = _Sdy[curY];

				if ((iSx != 0 || iSy != 0) && (iTx != 0 || iTy != 0))
				{
					//partial Sum  = Sum of(((Source X derivative* Template X drivative) + Source Y derivative * Template Y derivative)) / Edge magnitude of(Template)* edge magnitude of(Source))
					partialSum = partialSum + ((iSx*iTx) + (iSy*iTy))*(edgeMagnitude[m] * matGradMag[curX][curY]);

				}

				sumOfCoords = m + 1;
				partialScore = partialSum / sumOfCoords;
				// 检查终止准则
				// 如果部分分数低于所需的分数，则在该位置获得所需的分数。
				// 在那个坐标处中断连接。
				if (partialScore < (MIN((minScore - 1) + normGreediness*sumOfCoords, normMinScore*  sumOfCoords)))
					break;

			}
			if (partialScore > resultScore)
			{
				//  匹配分数
				resultScore = partialScore; 
				resultPoint->x = i;			// result coordinate X		
				resultPoint->y = j;			// result coordinate Y
			}
		}
	}

	// 免费使用的资源和返回分数
	ReleaseDoubleMatrix(matGradMag, Ssize.height);
	cvReleaseMat(&Sdx);
	cvReleaseMat(&Sdy);

	return resultScore;
}
// destructor
GeoMatch1::~GeoMatch1(void)
{
	delete[] cordinates;
	delete[] edgeMagnitude;
	delete[] edgeDerivativeX;
	delete[] edgeDerivativeY;
}

//双贝尔矩阵的内存分配
void GeoMatch1::CreateDoubleMatrix(double **&matrix, CvSize size)
{
	matrix = new double*[size.height];
	for (int iInd = 0; iInd < size.height; iInd++)
		matrix[iInd] = new double[size.width];
}
//释放存储器
void GeoMatch1::ReleaseDoubleMatrix(double **&matrix, int size)
{
	for (int iInd = 0; iInd < size; iInd++)
		delete[] matrix[iInd];
}


//绘制结果图像周围的等高线
void GeoMatch1::DrawContours(IplImage* source, CvPoint COG, CvScalar color, int lineWidth)
{
	CvPoint point;
	point.y = COG.x;
	point.x = COG.y;
	for (int i = 0; i<noOfCordinates; i++)
	{
		point.y = cordinates[i].x + COG.x;
		point.x = cordinates[i].y + COG.y;
		cvLine(source, point, point, color, lineWidth);
	}
}

//在模板图像上绘制轮廓
void GeoMatch1::DrawContours(IplImage* source, CvScalar color, int lineWidth)
{
	CvPoint point;
	for (int i = 0; i<noOfCordinates; i++)
	{
		point.y = cordinates[i].x + centerOfGravity.x;
		point.x = cordinates[i].y + centerOfGravity.y;
		cvLine(source, point, point, color, lineWidth);
	}
}
