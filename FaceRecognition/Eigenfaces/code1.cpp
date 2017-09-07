// Eigenfaces.cpp : 定义控制台应用程序的入口点。
// Eigenfaces特征脸createEigenFaceRecognizer()
#define _ITERATOR_DEBUG_LEVEL 0
#define RAPIDJSON_HAS_STDSTRING 1
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#if _ITERATOR_DEBUG_LEVEL == 0 && _SECURE_SCL != 0 
#error _SECURE_SCL != 0 while _ITERATOR_DEBUG_LEVEL == 0 
#endif 

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#define _WINSOCKAPI_  

#include "stdafx.h"
#include "opencv2/opencv.hpp""
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/face.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <chrono>
#include <memory>
#include <cstdlib>
#include <ctime>

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
extern "C" {
}

#define INBUF_SIZE 4096
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096
using namespace cv;
using namespace std;
using namespace cv::face;

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

// default receive buffer length
#define DEFAULT_BUFLEN 1024
// server ip address and port
#define DEFAULT_SERVER_IP "127.0.0.1"
#define DEFAULT_PORT "8866"
//#define DEFAULT_SERVER_IP "172.30.17.175"
//#define DEFAULT_PORT "27015"
// rtsp video stream
#define DEFAULT_RTSP "rtsp://admin:zhengyuan123@172.30.17.8/Streaming/Channels/101"
// boolean value, if the program will receive any information or command from socket server
#define RECV_FROM_SOCKET false


static void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';') {
	std::ifstream file(filename.c_str(), ifstream::in);
	if (!file) {
		string error_message = "No valid input file was given, please check the given filename.";
		CV_Error(CV_StsBadArg, error_message);
	}
	string line, path, classlabel;
	while (getline(file, line)) {
		stringstream liness(line);
		getline(liness, path, separator);
		getline(liness, classlabel);
		if (!path.empty() && !classlabel.empty()) {
			Mat img = imread(path, 0);
			//printf("%s",path);
			//printf("%d %d", img.cols, img.rows);
			images.push_back(imread(path, 0));
			labels.push_back(atoi(classlabel.c_str()));
		}
	}
}

/**
* double转换为string
*/
string convertToString(double d) {
	ostringstream os;
	if (os << d)
		return os.str();
	return "invalid conversion";
}

int main() {
    char *rtsp = "rtsp://localhost:8554/vlc";
	rtsp = "rtmp://live.hkstv.hk.lxdns.com/live/hks";
	/********************************* hard coded values **************************************/
	// size of recognized face
	int minsize = 50;
	int maxsize = 1000;
	// scale of original video
	int scale_w = 2;
	int scale_h = 2;
	// path to csv file
	string fn_csv = "Face/face.csv";
	// path to haarcascades
	string fn_haar = "data/haarcascade_frontalface_alt.xml";
	vector<string> names = { "CXW","CJD","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20","21","22","23","24","25","26" };
	//default command, method could be 0 or 1, param could be any id of a person
	//默认命令，方法可能是0或1，参数可以是任何身份的人
	int method = 0;//是否特定识别某人0：识别所有人，1识别某一个人
	int param = 13;//method==1时，识别的对象index

	/***************************************************************************************************************/
	#pragma region socket链接

	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	char *sendbuf = "face recognition : this is a test";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;


	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return -1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(DEFAULT_SERVER_IP, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return -2;
	}

	// Attempt to connect to an address until one succeeds
	//尝试连接到一个地址直到成功为止。
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return -3;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return -1;
	}

	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return -1;
	}

	printf("Bytes Sent: %ld\n", iResult);

	if (RECV_FROM_SOCKET) {
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);
			printf("content %s\n", recvbuf);
		}
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

		if (recvbuf == "") {
			return -4;
		}
	}

	#pragma endregion
	/*************************************************************************************************************/

	//CvCapture *cap = cvCaptureFromFile(rtsp);
	//获取摄像头  
	VideoCapture cap1(0);
	
	CascadeClassifier cascade;
	//cvNamedWindow("face recognition", CV_WINDOW_AUTOSIZE);
	// These vectors hold the images and corresponding labels:
	vector<Mat> images, images_orig;//csv所有图片列表
	vector<int> labels, labels_orig;//csv所有图片列表后index
	// load haarcascades
	if (!cascade.load(fn_haar)) {

		printf("no cascade file \n");
		return -5;
	}

	// Read in the data (fails if no valid input filename is given, but you'll get an error message):
	try {
		read_csv(fn_csv, images, labels);
	}
	catch (cv::Exception& e) {
		cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
		exit(1);
	}

	images_orig = images;
	labels_orig = labels;
	int im_width = images[0].cols;
	int im_height = images[0].rows;
	printf("im_width %d im_height %d\n", im_width, im_height);
	printf("labels %d\n", labels.size());

	/******************************************************/
	#pragma region 设置特定识别某人
	if (method == 1) {
		vector<Mat> img_temp;
		vector<int> label_temp;
		for (int i = 0; i < labels.size(); i++) {
			if (labels[i] == param) {
				img_temp.push_back(images[i]);
				label_temp.push_back(labels[i]);
			}
		}
		images = img_temp;
		labels = label_temp;
	}
	#pragma endregion
	/******************************************************/
	//三种识别模式
	//Ptr<FaceRecognizer> model = createFisherFaceRecognizer();
	//Ptr<FaceRecognizer> model = createLBPHFaceRecognizer();
	Ptr<FaceRecognizer> model = createEigenFaceRecognizer();
	//model->train(images, labels);
	//model->train(images, labels);
	model->train(images, labels);

	int count = 0;
	int frame_count = 0;
	vector<Rect> faces;

	namedWindow("image", 1);

	while (cvWaitKey(10) != atoi("q"))
	{
		frame_count++;
		frame_count = frame_count % 100000;
		
		//获取要识别的图片
		Mat frame;
		cap1 >> frame;
		IplImage *img_v = &IplImage(frame);//本地摄像头
		
		//IplImage *img_v = cvQueryFrame(cap);//rtsp流
		if (!img_v) {
			printf("no image %d\n", frame_count);
			continue;
		}

		cv::Mat img = cv::cvarrToMat(img_v);

		Mat gray, smallImg(cvRound(img.rows / scale_h), cvRound(img.cols / scale_w), CV_8UC1);
		cvtColor(img, gray, CV_BGR2GRAY);
		resize(img, smallImg, smallImg.size(), 0, 0, INTER_LINEAR);

		string json_result = "";
		char* sendbuf = "人脸识别 : 开始测试";

		try
		{
			cascade.detectMultiScale(smallImg, faces, 1.1, 5, CV_HAAR_FIND_BIGGEST_OBJECT, Size(minsize, minsize));
			int min_distance = 100000;
			int min_x = 0;
			int min_y = 0;
			Rect min_face_i;
			//判断有几个人脸
			//遍历显示
			for (int i = 0; i < faces.size(); i++) {
				Rect face_i = faces[i];
				Mat face = gray(face_i);
				Mat face_resized;
				resize(face, face_resized, Size(im_width, im_height), 1.0, 1.0, INTER_CUBIC);
				int prediction, prediction2, prediction3 = -1;
				double confidence, confidence2, confidence3 = 10000.0;
				//model1->predict(face_resized, prediction, confidence);
				//model2->predict(face_resized, prediction2, confidence2);
				model->predict(face_resized, prediction3, confidence3);
				string name = names[prediction3]+" confidence = "+ convertToString(confidence3);
				cout << "这是第几个:" << names[prediction3] << "相识度为:" << confidence3 << endl;
				//Mat save_resized;
				//imwrite("C:\\Users\\xiaoyan\\Documents\\Face\\face" + to_string(frame_count) + ".jpg", Mat(smallImg, face_i));
				//string box_text = format("%s, confidence = %f", names[prediction], confidence);
				//string box_text2 = format("%s, confidence = %f", names[prediction2], confidence2);
				string box_text3 = format("%s, confidence = %f", name, confidence3);
				box_text3 = name;
				int pos_x = max(face_i.tl().x - 10, 0);
				int pos_y = max(face_i.tl().y - 10, 0);

				switch (method) {
				case 1:
					//putText(smallImg, box_text, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(255, 255, 0), 2.0);
					//putText(smallImg, box_text2, Point(pos_x, pos_y - 20), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(255, 255, 0), 2.0);
					putText(smallImg, box_text3, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0, 255, 0), 2.0);
				default:
					//putText(smallImg, box_text, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(255, 255, 0), 2.0);
					//putText(smallImg, box_text2, Point(pos_x, pos_y - 20), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(255, 255, 0), 2.0);
					putText(smallImg, box_text3, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0, 255, 0), 2.0);
					break;
				}
				// Save face					
				//myFile << frame_count <<" "<< prediction<<" "<<names[prediction]<<" "<<confidence<<" "<<face_i.tl().x<<" "<<face_i.tl().y<<" "<<face_i.width<<" "<<face_i.height<<endl;
				rectangle(smallImg, face_i, CV_RGB(255, 255, 0), 1);
			}

			if (method == 2) {
				string box_text3 = format("%s", names[param]);
				putText(smallImg, box_text3, Point(min_x, min_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0, 255, 0), 2.0);
				rectangle(smallImg, min_face_i, CV_RGB(0, 255, 0), 2);
			}
		}
		catch (Exception ex)
		{
			printf("error in cascade\n");
		}
		imshow("image", smallImg);

		if (frame_count % 50 == 0) {
			printf("sendbuf: %s\n", sendbuf);

			int iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
			if (iResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ConnectSocket);
				WSACleanup();
				return 3;
			}

			printf("Bytes Sent: %ld\n", iResult);

			if (RECV_FROM_SOCKET) {
				iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);

				if (iResult > 0) {
					printf("Bytes received: %d\n", iResult);
					printf("content %s\n", recvbuf);
				}
				else if (iResult == 0)
					printf("Connection closed\n");
				else
					printf("recv failed with error: %d\n", WSAGetLastError());

			}

		}
	}
	printf("socket finished\n");
	//myFile.close();
	//cvReleaseCapture(&cap1);
	//cvReleaseCapture(&cap2);
	//cvDestroyWindow("face recognition");
	return 1;
}
