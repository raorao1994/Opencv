//***********************************************************************
// File Name		: GeoMatch.h
// Last Modified By : raorao
// Last Modified On : 03-08-2018
// Description      : ʵ�ֻ��ڱ�Ե��ģ��ƥ��
//***********************************************************************

#include "StdAfx.h"
#include "GeoMatch1.h"


GeoMatch1::GeoMatch1()
{
	// ģ�͵���cppodinate�ĳ�ʼNO
	noOfCordinates = 0;
	modelDefined = false;
}
//����ģ��ƥ��ģ��
int GeoMatch1::CreateGeoMatchModel(const void *templateArr, double maxContrast, double minContrast)
{

	CvMat *gx = 0;		//�洢X�����ľ���
	CvMat *gy = 0;		//�洢Y�����ľ���
	CvMat *nmsEdges = 0;//�洢��ʱRESTOR�ľ���
	CvSize Ssize;

	// ��IPLͼ��ת��Ϊ���������ľ���
	CvMat srcstub, *src = (CvMat*)templateArr;
	src = cvGetMat(src, &srcstub);
	if (CV_MAT_TYPE(src->type) != CV_8UC1)
	{
		return 0;
	}

	// set width and height
	Ssize.width = src->width;
	Ssize.height = src->height;
	modelHeight = src->height;		//����ģ��߶�
	modelWidth = src->width;			//����ģ�����

	noOfCordinates = 0;	//��ʼ��
	//Ϊģ��ͼ������ѡ���Эͬ���������ڴ�
	cordinates = new CvPoint[modelWidth *modelHeight];
	//Ϊѡ����ı�Ե��С�����ڴ�
	edgeMagnitude = new double[modelWidth *modelHeight];
	//Ϊѡ��������ԵX�������ڴ�
	edgeDerivativeX = new double[modelWidth *modelHeight];
	//Ϊѡ���ĵ�����Եy��������ڴ�
	edgeDerivativeY = new double[modelWidth *modelHeight];			


	//����ģ���ݶ�							
	gx = cvCreateMat(Ssize.height, Ssize.width, CV_16SC1);
	gy = cvCreateMat(Ssize.height, Ssize.width, CV_16SC1);
	cvSobel(src, gx, 1, 0, 3);		//X�����ݶ�
	cvSobel(src, gy, 0, 1, 3);	//Y�����ݶ�
	//���������Դ洢���յ�nmsEdges
	nmsEdges = cvCreateMat(Ssize.height, Ssize.width, CV_32F);		
	const short* _sdx;//x���ֵ
	const short* _sdy;//y���ֵ
	double fdx, fdy;
	double MagG, DirG;//xy��Ⱦ���
	double MaxGradient = -99999.99;
	double direction;//�ݶȽǶ�
	int *orients = new int[Ssize.height *Ssize.width];
	int count = 0, i, j; // count variable;

	double **magMat;
	CreateDoubleMatrix(magMat, Ssize);
	//�ݶȼ���
	for (i = 1; i < Ssize.height - 1; i++)
	{
		for (j = 1; j < Ssize.width - 1; j++)
		{
			_sdx = (short*)(gx->data.ptr + gx->step*i);
			_sdy = (short*)(gy->data.ptr + gy->step*i);
			fdx = _sdx[j]; 
			fdy = _sdy[j]; //��ȡXY����read x, y derivatives

			MagG = sqrt((float)(fdx*fdx) + (float)(fdy*fdy)); //Magnitude = Sqrt(gx^2 +gy^2)
			direction = cvFastArctan((float)fdy, (float)fdx);	 //Direction = invtan (Gy / Gx)
			magMat[i][j] = MagG;

			if (MagG>MaxGradient)
				//��ȡ��һ��������ݶ�ֵ��
				MaxGradient = MagG;


			//��0��45��90��135�õ�����ĽǶ�
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
	//��ʼ����������
	count = 0; 
	//���������
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
			//�Ƚϵ�ǰ����ֵ����������
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

	//Hysterisis ��ֵ
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

			//����ѡ���ı�Ե��Ϣ
			curX = i;	curY = j;
			if (flag != 0)
			{
				if (fdx != 0 || fdy != 0)
				{
					// �����кͺ��к�
					RSum = RSum + curX;	CSum = CSum + curY; 

					cordinates[noOfCordinates].x = curX;
					cordinates[noOfCordinates].y = curY;
					edgeDerivativeX[noOfCordinates] = fdx;
					edgeDerivativeY[noOfCordinates] = fdy;

					//�ֱ�������handle divide by zero
					if (MagG != 0)
						edgeMagnitude[noOfCordinates] = 1 / MagG;  //�ݶȼ�
					else
						edgeMagnitude[noOfCordinates] = 0;

					noOfCordinates++;
				}
			}
		}
	}

	centerOfGravity.x = RSum / noOfCordinates; // ����
	centerOfGravity.y = CSum / noOfCordinates;	// ����

	//�ı������Է�ӳ����
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
	int i, j, m;			//��������
	double iTx, iTy, iSx, iSy;
	double gradMag;
	int curX, curY;

	double **matGradMag;  //�ݶȳ����� 

	CvMat srcstub, *src = (CvMat*)srcarr;
	src = cvGetMat(src, &srcstub);
	if (CV_MAT_TYPE(src->type) != CV_8UC1 || !modelDefined)
	{
		return 0;
	}

	// ԴͼƬ��С
	CvSize Ssize;
	Ssize.width = src->width;
	Ssize.height = src->height;
	//����ͼ���Ա����ݶȷ�ֵ
	CreateDoubleMatrix(matGradMag, Ssize); 

	Sdx = cvCreateMat(Ssize.height, Ssize.width, CV_16SC1); // X ����
	Sdy = cvCreateMat(Ssize.height, Ssize.width, CV_16SC1); // y ����

	cvSobel(src, Sdx, 1, 0, 3);  // find X �ݶ�
	cvSobel(src, Sdy, 0, 1, 3); // find Y �ݶ�

	// Ѱ��ģ�͵Ļز��о�
	// Ԥ����С����
	double normMinScore = minScore / noOfCordinates; 
	// Ԥ�� greedniness 
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
			// ��ʼ�����ֺͲ��
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

				// ��Դͼ���ȡ������ӦXY����

				iSx = _Sdx[curY]; 
				iSy = _Sdy[curY];

				if ((iSx != 0 || iSy != 0) && (iTx != 0 || iTy != 0))
				{
					//partial Sum  = Sum of(((Source X derivative* Template X drivative) + Source Y derivative * Template Y derivative)) / Edge magnitude of(Template)* edge magnitude of(Source))
					partialSum = partialSum + ((iSx*iTx) + (iSy*iTy))*(edgeMagnitude[m] * matGradMag[curX][curY]);

				}

				sumOfCoords = m + 1;
				partialScore = partialSum / sumOfCoords;
				// �����ֹ׼��
				// ������ַ�����������ķ��������ڸ�λ�û������ķ�����
				// ���Ǹ����괦�ж����ӡ�
				if (partialScore < (MIN((minScore - 1) + normGreediness*sumOfCoords, normMinScore*  sumOfCoords)))
					break;

			}
			if (partialScore > resultScore)
			{
				//  ƥ�����
				resultScore = partialScore; 
				resultPoint->x = i;			// result coordinate X		
				resultPoint->y = j;			// result coordinate Y
			}
		}
	}

	// ���ʹ�õ���Դ�ͷ��ط���
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

//˫����������ڴ����
void GeoMatch1::CreateDoubleMatrix(double **&matrix, CvSize size)
{
	matrix = new double*[size.height];
	for (int iInd = 0; iInd < size.height; iInd++)
		matrix[iInd] = new double[size.width];
}
//�ͷŴ洢��
void GeoMatch1::ReleaseDoubleMatrix(double **&matrix, int size)
{
	for (int iInd = 0; iInd < size; iInd++)
		delete[] matrix[iInd];
}


//���ƽ��ͼ����Χ�ĵȸ���
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

//��ģ��ͼ���ϻ�������
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