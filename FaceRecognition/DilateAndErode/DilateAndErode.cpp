// DilateAndErode.cpp : 定义控制台应用程序的入口点。
//http://blog.csdn.net/poem_qianmo/article/details/23710721
//膨胀和腐蚀
//-----------------------------------【头文件包含部分】---------------------------------------  
//            描述：包含程序所依赖的头文件  
//----------------------------------------------------------------------------------------------  
#include "stdafx.h"
#include <opencv2/opencv.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <iostream>  
//-----------------------------------【命名空间声明部分】---------------------------------------  
//            描述：包含程序所使用的命名空间  
//-----------------------------------------------------------------------------------------------  
using namespace std;
using namespace cv;


//-----------------------------------【全局变量声明部分】--------------------------------------  
//            描述：全局变量声明  
//-----------------------------------------------------------------------------------------------  
Mat g_srcImage, g_dstImage;//原始图和效果图  
int g_nTrackbarNumer = 0;//0表示腐蚀erode, 1表示膨胀dilate  
int g_nStructElementSize = 3; //结构元素(内核矩阵)的尺寸
int g_nElementShape = MORPH_RECT;//元素结构的形状 

//变量接收的TrackBar位置参数  
int g_nMaxIterationNum = 10;
int g_nOpenCloseNum = 0;
int g_nErodeDilateNum = 0;
int g_nTopBlackHatNum = 0;

//-----------------------------------【全局函数声明部分】--------------------------------------  
//            描述：全局函数声明  
//-----------------------------------------------------------------------------------------------  
static void Process();//膨胀和腐蚀的处理函数  
static void on_TrackbarNumChange(int, void *);//回调函数  
static void on_ElementSizeChange(int, void *);//回调函数  

static void on_OpenClose(int, void*);//回调函数  
static void on_ErodeDilate(int, void*);//回调函数  
static void on_TopBlackHat(int, void*);//回调函数  
static void ShowHelpText();//帮助文字显示

static int start1();
static int start2();


int main()
{
	//start1();
	start2();
	return 0;
}
//-----------------------------【start1( )函数】------------------------------------  
//            描述：腐蚀和膨胀操作  
//-----------------------------------------------------------------------------------------  
static int start1()
{
	//改变console字体颜色  
	system("color5E");

	//载入原图  
	g_srcImage = imread("d:/8.jpg");
	if (!g_srcImage.data) { printf("读取srcImage错误~！\n"); return 1; }

	//显示原始图  
	namedWindow("【原始图】");
	imshow("【原始图】", g_srcImage);

	//进行初次腐蚀操作并显示效果图  
	namedWindow("【效果图】");
	//获取自定义核  
	Mat element = getStructuringElement(MORPH_RECT, Size(2 * g_nStructElementSize + 1, 2 * g_nStructElementSize + 1), Point(g_nStructElementSize, g_nStructElementSize));
	erode(g_srcImage, g_dstImage, element);
	imshow("【效果图】", g_dstImage);

	//创建轨迹条  
	createTrackbar("腐蚀/膨胀", "【效果图】", &g_nTrackbarNumer, 1, on_TrackbarNumChange);
	createTrackbar("内核尺寸", "【效果图】", &g_nStructElementSize, 21, on_ElementSizeChange);

	//输出一些帮助信息  
	cout << endl << "\t运行成功，请调整滚动条观察图像效果~\n\n"
		<< "\t按下“q”键时，程序退出~!\n";

	//轮询获取按键信息，若下q键，程序退出  
	while (char(waitKey(1)) != 'q') {}
}
//-----------------------------【Process( )函数】------------------------------------  
//            描述：进行自定义的腐蚀和膨胀操作  
//-----------------------------------------------------------------------------------------  
void Process()
{
	//获取自定义核  
	Mat element = getStructuringElement(MORPH_RECT, Size(2 * g_nStructElementSize + 1, 2 * g_nStructElementSize + 1), Point(g_nStructElementSize, g_nStructElementSize));

	//进行腐蚀或膨胀操作  
	if (g_nTrackbarNumer == 0) {
		erode(g_srcImage, g_dstImage, element);
	}
	else {
		dilate(g_srcImage, g_dstImage, element);
	}

	//显示效果图  
	imshow("【效果图】", g_dstImage);
}

//-----------------------------【on_TrackbarNumChange( )函数】------------------------------------  
//            描述：腐蚀和膨胀之间切换开关的回调函数  
//-----------------------------------------------------------------------------------------------------  
static void on_TrackbarNumChange(int, void *)
{
	//腐蚀和膨胀之间效果已经切换，回调函数体内需调用一次Process函数，使改变后的效果立即生效并显示出来  
	Process();
}


//-----------------------------【on_ElementSizeChange( )函数】-------------------------------------  
//            描述：腐蚀和膨胀操作内核改变时的回调函数  
//-----------------------------------------------------------------------------------------------------  
static void on_ElementSizeChange(int, void *)
{
	//内核尺寸已改变，回调函数体内需调用一次Process函数，使改变后的效果立即生效并显示出来  
	Process();
}


static int start2()
{
	//改变console字体颜色  
	system("color 2F");

	ShowHelpText();

	//载入原图  
	g_srcImage = imread("d:/8.jpg");//工程目录下需要有一张名为1.jpg的素材图  
	if (!g_srcImage.data) { printf("读取srcImage错误~！ \n"); return 1; }

	//显示原始图  
	namedWindow("【原始图】");
	imshow("【原始图】", g_srcImage);

	//创建三个窗口  
	namedWindow("【开运算/闭运算】", 1);
	namedWindow("【腐蚀/膨胀】", 1);
	namedWindow("【顶帽/黑帽】", 1);

	//参数赋值  
	g_nOpenCloseNum = 9;
	g_nErodeDilateNum = 9;
	g_nTopBlackHatNum = 2;

	//分别为三个窗口创建滚动条  
	createTrackbar("迭代值", "【开运算/闭运算】", &g_nOpenCloseNum, g_nMaxIterationNum * 2 + 1, on_OpenClose);
	createTrackbar("迭代值", "【腐蚀/膨胀】", &g_nErodeDilateNum, g_nMaxIterationNum * 2 + 1, on_ErodeDilate);
	createTrackbar("迭代值", "【顶帽/黑帽】", &g_nTopBlackHatNum, g_nMaxIterationNum * 2 + 1, on_TopBlackHat);

	//轮询获取按键信息  
	while (1)
	{
		int c;

		//执行回调函数  
		on_OpenClose(g_nOpenCloseNum, 0);
		on_ErodeDilate(g_nErodeDilateNum, 0);
		on_TopBlackHat(g_nTopBlackHatNum, 0);

		//获取按键  
		c = waitKey(0);

		//按下键盘按键Q或者ESC，程序退出  
		if ((char)c == 'q' || (char)c == 27)
			break;
		//按下键盘按键1，使用椭圆(Elliptic)结构元素结构元素MORPH_ELLIPSE  
		if ((char)c == 49)//键盘按键1的ASII码为49  
			g_nElementShape = MORPH_ELLIPSE;
		//按下键盘按键2，使用矩形(Rectangle)结构元素MORPH_RECT  
		else if ((char)c == 50)//键盘按键2的ASII码为50  
			g_nElementShape = MORPH_RECT;
		//按下键盘按键3，使用十字形(Cross-shaped)结构元素MORPH_CROSS  
		else if ((char)c == 51)//键盘按键3的ASII码为51  
			g_nElementShape = MORPH_CROSS;
		//按下键盘按键space，在矩形、椭圆、十字形结构元素中循环  
		else if ((char)c == ' ')
			g_nElementShape = (g_nElementShape + 1) % 3;
	}
}
//-----------------------------------【on_OpenClose( )函数】----------------------------------  
//      描述：【开运算/闭运算】窗口的回调函数  
//-----------------------------------------------------------------------------------------------  
static void on_OpenClose(int, void*)
{
	//偏移量的定义  
	int offset = g_nOpenCloseNum - g_nMaxIterationNum;//偏移量  
	int Absolute_offset = offset > 0 ? offset : -offset;//偏移量绝对值  
														//自定义核  
	Mat element = getStructuringElement(g_nElementShape, Size(Absolute_offset * 2 + 1, Absolute_offset * 2 + 1), Point(Absolute_offset, Absolute_offset));
	//进行操作  
	if (offset < 0)
		morphologyEx(g_srcImage, g_dstImage, CV_MOP_OPEN, element);
	else
		morphologyEx(g_srcImage, g_dstImage, CV_MOP_CLOSE, element);
	//显示图像  
	imshow("【开运算/闭运算】", g_dstImage);
}
//-----------------------------------【on_ErodeDilate( )函数】----------------------------------  
//      描述：【腐蚀/膨胀】窗口的回调函数  
//-----------------------------------------------------------------------------------------------  
static void on_ErodeDilate(int, void*)
{
	//偏移量的定义  
	int offset = g_nErodeDilateNum - g_nMaxIterationNum;    //偏移量  
	int Absolute_offset = offset > 0 ? offset : -offset;//偏移量绝对值  
														//自定义核  
	Mat element = getStructuringElement(g_nElementShape, Size(Absolute_offset * 2 + 1, Absolute_offset * 2 + 1), Point(Absolute_offset, Absolute_offset));
	//进行操作  
	if (offset < 0)
		erode(g_srcImage, g_dstImage, element);
	else
		dilate(g_srcImage, g_dstImage, element);
	//显示图像  
	imshow("【腐蚀/膨胀】", g_dstImage);
}


//-----------------------------------【on_TopBlackHat( )函数】--------------------------------  
//      描述：【顶帽运算/黑帽运算】窗口的回调函数  
//----------------------------------------------------------------------------------------------  
static void on_TopBlackHat(int, void*)
{
	//偏移量的定义  
	int offset = g_nTopBlackHatNum - g_nMaxIterationNum;//偏移量  
	int Absolute_offset = offset > 0 ? offset : -offset;//偏移量绝对值  
														//自定义核  
	Mat element = getStructuringElement(g_nElementShape, Size(Absolute_offset * 2 + 1, Absolute_offset * 2 + 1), Point(Absolute_offset, Absolute_offset));
	//进行操作  
	if (offset < 0)
		morphologyEx(g_srcImage, g_dstImage, MORPH_TOPHAT, element);
	else
		morphologyEx(g_srcImage, g_dstImage, MORPH_BLACKHAT, element);
	//显示图像  
	imshow("【顶帽/黑帽】", g_dstImage);
}

//-----------------------------------【ShowHelpText( )函数】----------------------------------  
//      描述：输出一些帮助信息  
//----------------------------------------------------------------------------------------------  
static void ShowHelpText()
{
	//输出一些帮助信息  
	printf("\n\n\n\t请调整滚动条观察图像效果~\n\n");
	printf("\n\n\t按键操作说明: \n\n"
		"\t\t键盘按键【ESC】或者【Q】- 退出程序\n"
		"\t\t键盘按键【1】- 使用椭圆(Elliptic)结构元素\n"
		"\t\t键盘按键【2】- 使用矩形(Rectangle )结构元素\n"
		"\t\t键盘按键【3】- 使用十字型(Cross-shaped)结构元素\n"
		"\t\t键盘按键【空格SPACE】- 在矩形、椭圆、十字形结构元素中循环\n"
	);
}

