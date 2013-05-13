#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <opencv\cv.h>
#include <opencv\cvaux.h>
#include "Modules\correspondanceExtractor.h"

struct Camera
{
public:
	int id;
	cv::Mat P;	 // Camera/projection matrix (P = K*C)
	cv::Mat K;	 // Internal camera parameters
	cv::Mat C;	 // External camera parameters (C = [R t])
	cv::Mat R;	 // Rotation
	cv::Mat t;	 // Translation
	std::vector<cv::Point2d> imagePoints;
	std::vector<cv::Point3d*> visible3DPoints;
public:
	Camera(void);
	~Camera(void);
};

struct CameraPair
{
public:
	Camera * camera1;
	Camera * camera2;
	cv::Mat F;
	cv::Mat E;
	std::vector<pointPair> pointPairs;
	std::vector<cv::Point3d*> point3Ds;
public:
	CameraPair(Camera * cam1, Camera * cam2):camera1(cam1),camera2(cam2){}
};

class View
{
public:
	Camera * camera;
	cv::Mat image;
	
public:
	void render();
};

class ObserverPair
{
public:
	Camera * camera1;
	Camera * camera2;
	pointPair point2D;
public:
	ObserverPair(Camera * cam1, Camera * cam2, pointPair p2):camera1(cam1),camera2(cam2),point2D(p2){}
};

class Visible3DPoint
{
public:
	cv::Point3d * point3D;
	std::list<ObserverPair> observerPair;
public:
	Visible3DPoint(cv::Point3d * p):point3D(p) {}
	Visible3DPoint(cv::Point3d * p, ObserverPair op):point3D(p) {observerPair.push_back(op);}
};

#endif