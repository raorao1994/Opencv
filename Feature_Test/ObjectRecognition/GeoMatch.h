//***********************************************************************
// File Name		: GeoMatch.h
// Last Modified By : raorao
// Last Modified On : 03-08-2018
// Description      : 实现基于边缘的模板匹配
//***********************************************************************

#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <cxcore.h>
#include "opencv2/highgui.hpp" 
#include <math.h>

class GeoMatch
{
private:
	int				noOfCordinates;		//Number of elements in coordinate array
	CvPoint			*cordinates;		//Coordinates array to store model points	
	int				modelHeight;		//Template height
	int				modelWidth;			//Template width
	double			*edgeMagnitude;		//gradient magnitude
	double			*edgeDerivativeX;	//gradient in X direction
	double			*edgeDerivativeY;	//radient in Y direction	
	CvPoint			centerOfGravity;	//Center of gravity of template 

	bool			modelDefined;

	void CreateDoubleMatrix(double **&matrix, CvSize size);
	void ReleaseDoubleMatrix(double **&matrix, int size);
public:
	GeoMatch(void);
	GeoMatch(const void* templateArr);
	~GeoMatch(void);

	int CreateGeoMatchModel(const void* templateArr, double, double);
	double FindGeoMatchModel(const void* srcarr, double minScore, double greediness, CvPoint *resultPoint);
	void DrawContours(IplImage* pImage, CvPoint COG, CvScalar, int);
	void DrawContours(IplImage* pImage, CvScalar, int);
};

