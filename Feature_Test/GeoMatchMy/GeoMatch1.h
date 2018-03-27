//***********************************************************************
// File Name		: GeoMatch.h
// Last Modified By : raorao
// Last Modified On : 03-08-2018
// Description      : 实现基于边缘的模板匹配
//***********************************************************************

#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "opencv2/highgui.hpp" 
#include <math.h>

class GeoMatch1
{
private:
	int				noOfCordinates;		//坐标阵列中的元素数
	CvPoint			*cordinates;		//坐标数组存储模型点
	int				modelHeight;		//模板高度
	int				modelWidth;			//模板宽度
	double			*edgeMagnitude;		//梯度级
	double			*edgeDerivativeX;	//X方向梯度
	double			*edgeDerivativeY;	//Y方向梯度
	CvPoint			centerOfGravity;	//模板重心

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

