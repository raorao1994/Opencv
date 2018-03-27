//***********************************************************************
// File Name		: GeoMatch.h
// Last Modified By : raorao
// Last Modified On : 03-08-2018
// Description      : ʵ�ֻ��ڱ�Ե��ģ��ƥ��
//***********************************************************************

#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "opencv2/highgui.hpp" 
#include <math.h>

class GeoMatch1
{
private:
	int				noOfCordinates;		//���������е�Ԫ����
	CvPoint			*cordinates;		//��������洢ģ�͵�
	int				modelHeight;		//ģ��߶�
	int				modelWidth;			//ģ����
	double			*edgeMagnitude;		//�ݶȼ�
	double			*edgeDerivativeX;	//X�����ݶ�
	double			*edgeDerivativeY;	//Y�����ݶ�
	CvPoint			centerOfGravity;	//ģ������

	bool			modelDefined;

	void CreateDoubleMatrix(double **&matrix, CvSize size);
	void ReleaseDoubleMatrix(double **&matrix, int size);
public:
	GeoMatch1();
	~GeoMatch1();
	int CreateGeoMatchModel(const void* templateArr, double, double);
	double FindGeoMatchModel(const void* srcarr, double minScore, double greediness, CvPoint *resultPoint);
	void DrawContours(IplImage* pImage, CvPoint COG, CvScalar, int);
	void DrawContours(IplImage* pImage, CvScalar, int);
};

