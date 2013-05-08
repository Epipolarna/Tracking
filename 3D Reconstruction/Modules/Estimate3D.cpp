#include "Estimate3D.h"


Estimate3D::Estimate3D(void)
{
}


Estimate3D::~Estimate3D(void)
{
	for(std::list<Camera*>::iterator i = cameras.begin(); i != cameras.end(); i++)
	{
		delete *i;
	}
	for(std::vector<Visible3DPoint>::iterator i = visible3DPoint.begin(); i != visible3DPoint.end(); i++)
	{
		delete i->point3D;
	}
}

void Estimate3D::init(cv::vector<cv::Point2d> & p1, cv::vector<cv::Point2d> & p2, cv::Mat & K_)
{
	K = K_.clone();
	GoldStandardOutput GO;
	cv::Mat F = getGoldStandardF(p1,p2, &GO);
	cv::Point3d * p3d;
	Camera * cam1 = new Camera();
	Camera * cam2 = new Camera();
	cameras.push_back(cam1);
	cameras.push_back(cam2);

	cam1->id = 0;
	cam2->id = 1;
	cam1->P = GO.P1;
	cam2->P = GO.P2;
	//cam1->K = K;
	//cam2->K = K;

	cameraPair.push_back(CameraPair(cam1,cam2));
	cameraPair.back().F = F;
	cameraPair.back().E = K.t()*F*K;

	std::cout << "P:\n" << cam1->P << "\n";

	// Estimate R and t for the cameras
	decomposeProjectionMatrix(cam1->P, cam1->K, cam1->R, cam1->t);
	decomposeProjectionMatrix(cam2->P, cam2->K, cam2->R, cam2->t);

	// Fill up the data hierarchy (visibility etc)
	for(int n = 0; n < GO.point3D.size().width; n++)
	{
		p3d = new cv::Point3d(GO.point3D.at<double>(0,n), GO.point3D.at<double>(1,n), GO.point3D.at<double>(2,n));
		visible3DPoint.push_back(Visible3DPoint(p3d, ObserverPair(cam1, cam2, pointPair(GO.inlier1[n],GO.inlier2[n]))));
		cam1->imagePoints.push_back(GO.inlier1[n]);
		cam2->imagePoints.push_back(GO.inlier2[n]);
		cam1->visible3DPoints.push_back(p3d);
		cam2->visible3DPoints.push_back(p3d);
		cameraPair.back().pointPairs.push_back(pointPair(GO.inlier1[n], GO.inlier2[n]));
		cameraPair.back().point3Ds.push_back(p3d);
	}
}

void Estimate3D::addView(cv::vector<cv::Point2d> & p1, cv::vector<cv::Point2d> & p2)
{
	GoldStandardOutput GO;
	cv::Mat F = getGoldStandardF(p1,p2, &GO);
	cv::Point3d * p3d;
	Camera * cam1 = cameras.back();
	Camera * cam2 = new Camera();
	cameras.push_back(cam2);
	
	// Estimate R and t for the cameras
	/*vconcat(cam1->R, cv::Mat::zeros(1,3,CV_64FC1), cam1->C);
	hconcat(cam1->C, cam1->t/cam1->t.at<float>(0,3), cam1->C);
	cam1->P.at<float>(3,3) = 1;*/
	cam2->id = cam1->id+1;
	cam2->P = GO.P2;
	decomposeProjectionMatrix(cam2->P, cam2->K, cam2->R, cam2->t);

	// TODO: change coordinates of cam2 to the global coordinate system (the one which cam1 is in)

	// Fill up the data hierarchy (visibility etc)
	// Fill up the data hierarchy (visibility etc)
	for(int n = 0; n < GO.point3D.size().width; n++)
	{
		p3d = new cv::Point3d(GO.point3D.at<double>(0,n), GO.point3D.at<double>(1,n), GO.point3D.at<double>(2,n));
		if(isUnique3DPoint(cam1, GO.inlier1[n], &p3d))
		{
			visible3DPoint.push_back(Visible3DPoint(p3d, ObserverPair(cam1, cam2, pointPair(GO.inlier1[n],GO.inlier2[n]))));
			cam1->imagePoints.push_back(GO.inlier1[n]);
			cam2->imagePoints.push_back(GO.inlier2[n]);
			cam1->visible3DPoints.push_back(p3d);
			cam2->visible3DPoints.push_back(p3d);
			cameraPair.back().pointPairs.push_back(pointPair(GO.inlier1[n], GO.inlier2[n]));
			cameraPair.back().point3Ds.push_back(p3d);
		}
		else
		{
			// TODO: Only add the 3D points to the new camera, and add the pairs.. (etc..)
			cam2->imagePoints.push_back(GO.inlier2[n]);
			cam2->visible3DPoints.push_back(p3d);
			cameraPair.back().pointPairs.push_back(pointPair(GO.inlier1[n], GO.inlier2[n]));
			cameraPair.back().point3Ds.push_back(p3d);
		}
	}

}

bool isUnique3DPoint(Camera * cam, cv::Point2f p2D, cv::Point3d ** p3D)
{
	// TODO: Loop through cam's image points and find a match to p2D. Optionaly: find such match that additionally has a close p3D..
	return true;
}

cv::Mat cameraFromFundamentalMatrix(cv::Mat & F)
{
	cv::Mat C = cv::Mat(3,4,CV_64FC1);
	cv::SVD USV = cv::SVD(F, cv::SVD::FULL_UV);
	cv::Mat e = USV.u.col(2);
	cv::Mat a = -crossop(e)*F;
	a.copyTo(C(cv::Range::all(), cv::Range(0,3)));
	e.copyTo(C.col(3));
	return C;
}


cv::Mat crossop(cv::Mat vector)
{
	cv::Mat derp = cv::Mat::zeros(3,3,CV_64FC1);
	derp.at<double>(0,1) = -vector.at<double>(2,0);
	derp.at<double>(0,2) = vector.at<double>(1,0);
	derp.at<double>(1,0) = vector.at<double>(2,0);
	derp.at<double>(1,2) = -vector.at<double>(0,0);
	derp.at<double>(2,0) = -vector.at<double>(1,0);
	derp.at<double>(2,1) = vector.at<double>(0,0);
	return derp;
}

cv::Mat crossOperator(cv::Mat X)
{
	if(X.size().height * X.size().width == 3)
	{
		float Ydata[] = {  0,		  -X.data[2],	 X.data[1],
						   X.data[2],  0,			-X.data[0],
						  -X.data[1],  X.data[0],    0			};
		return cv::Mat(3,3,CV_64FC1,Ydata);
	}
	else if(X.size().height == 3 && X.size().width == 3)
	{
		float Ydata[] = {  X.at<float>(1,2),	X.at<float>(2,0),  X.at<float>(0,1)  };
		return cv::Mat(1,2,CV_64FC1,Ydata);
	}
	else
	{
		std::cout << "!ERROR! crossOperator not defined for a matrix with size (width,height)=(" << X.size().width << "," << X.size().height << ")\n";
		while(1);
		return X;
	}
}

cv::Mat normalizedCamera()
{
	float Cdata[] = { 1, 0, 0, 0,
					  0, 1, 0, 0,
					  0, 0, 1, 0  };
	return cv::Mat(3,4,CV_64FC1,Cdata);
}

// Gold standard F. points have to be ordered with correspondances at the same indices.
cv::Mat getGoldStandardF(cv::vector<cv::Point2d>& points1, cv::vector<cv::Point2d>& points2, GoldStandardOutput * Gout, int RANSAC_threshold)
{
	NonLinear::NonLinear nonlin;
	cv::vector<uchar> inlierMask;
	cv::Mat P1,P2,dinoHomPoints;
	// inital Fhat
	cv::Mat F = findFundamentalMat(points1, points2, CV_FM_RANSAC, RANSAC_threshold, 0.99, inlierMask);

	//Extract the inliers in the data
	cv::vector<cv::Point2d> inliers1;
	cv::vector<cv::Point2d> inliers2;

	//std::cout << "nu börjar det\n";

	for (int i = 0; i < inlierMask.size(); ++i)
	{
		if(inlierMask[i] != 0)
		{
			inliers1.push_back(points1[i]);
			inliers2.push_back(points2[i]);

			//std::cout << points1[i] << std::endl;
			//std::cout << points2[i] << std::endl;
		}
	}
	//Get epipolar point coordinates for P2 estimation
	cv::SVD singval = cv::SVD(F,cv::SVD::FULL_UV);
	cv::Mat eppoint2 = singval.u.col(2);
	cv::Mat epCross2 = crossop(eppoint2);
	P1 = cv::Mat::eye(3,4,CV_64FC1);
	// Create P2
	hconcat(epCross2 * F,eppoint2.clone(),P2);

	//Triangulate
	triangulatePoints(P1, P2, inliers1, inliers2, dinoHomPoints);
	
	cv::Mat normalized3Dpoints = dinoHomPoints.rowRange(0,3);
	// Get 3D points as an 3-by-N Matrix for easier data access by the nonlinear module.
	for (int i = 0; i < dinoHomPoints.size().width; ++i)
	{
		normalized3Dpoints.col(i) = normalized3Dpoints.col(i)/dinoHomPoints.at<double>(3,i);
	}
	
	//std::cout << "Inlier percentage  " << (double)inliers1.size()/(double)points1.size()*100 << std::endl;
	//std::cout << "Fundamental Matrix: " << F << std::endl;
	//nonlin.goldStandardRefine(F.clone(), inliers1,inliers2);
	
	
	//Actual call to the nonlinear part of the goldstandard estimation
	clock_t t;
	t = clock();
	F = nonlin.goldNonLin(F, P1, P2, normalized3Dpoints,inliers1,inliers2);
	F = F/F.at<double>(2,2);	// Normalize
	std::cout << "Gold standard nonlin time: " << ((float)(clock() - t))/CLOCKS_PER_SEC << std::endl;

	//std::cout << "Refined Fundamental Matrix: " << F << std::endl;
	//std::cout << "det(F): " << determinant(F) << std::endl;
	//return F/F.at<double>(2,2);
	//std::cout << "\n\n";
	//nonlin.goldStandardRefine(F, inliers1,inliers2);

	//std::cout << "Refined Fundamental Matrix: " << F << std::endl;
	//std::cout << "det(F): " << determinant(F) << std::endl;


	if(Gout != 0)
	{
		Gout->P1 = P1;
		Gout->P2 = P2;
		Gout->inlier1 = cv::vector<cv::Point2f>(inliers1.begin(), inliers1.end());
		Gout->inlier2 = cv::vector<cv::Point2f>(inliers2.begin(), inliers2.end());
		Gout->point3D = normalized3Dpoints;
	}

	return F;
}

/*
	CameraPair cameraPair;
	// correctMatches(F, bestPoints1, bestPoints2, optimizedBestPoints1, optimizedBestPoints2)
	// decomposePorjectionMatrix(

	double Kdata[9] = {	3217.328669180762, -78.606641008226180, 289.8672403229193,
						0,					2292.424143977958,  -1070.516234777778,
						0,					0,					1};
	cv::Mat K(3,3,CV_64FC1,Kdata);
	cv::Mat K_transposed;
	transpose(K, K_transposed);
	
	cv::Mat E = K_transposed*F*K;

	// Calculate R and t
	SVD E_svd = SVD(E,SVD::FULL_UV);
	double Wdata[9] = { 0, 0, 0,
					   -1, 0, 0,
						0, 0, 1};
	cv::Mat W(3,3,CV_64FC1,Wdata);
	cv::Mat R1, R2, V, U_transpose, W_transpose;
	transpose(E_svd.u, U_transpose);
	transpose(E_svd.vt, V);
	transpose(W, W_transpose);

	R1 = V*W*U_transpose;
	R2 = V*W_transpose*U_transpose;
		
	double C1data[16] = { 1, 0, 0, 0,
					      0, 1, 0, 0,
						  0, 0, 1, 0,
						  0, 0, 0, 1 };	
	double C21data[16] = { R1.data[0], R1.data[1], R1.data[2], V.data[2],
					       R1.data[3], R1.data[4], R1.data[5], V.data[5],
						   R1.data[6], R1.data[7], R1.data[8], V.data[8],
						   0		 , 0		 , 0		 , 1         };
	double C22data[16] = { R1.data[0], R1.data[1], R1.data[2], -V.data[2],
					       R1.data[3], R1.data[4], R1.data[5], -V.data[5],
						   R1.data[6], R1.data[7], R1.data[8], -V.data[8],
						   0		 , 0		 , 0		 , 1         };
	double C23data[16] = { R2.data[0], R2.data[1], R2.data[2], V.data[2],
					       R2.data[3], R2.data[4], R2.data[5], V.data[5],
						   R2.data[6], R2.data[7], R2.data[8], V.data[8],
						   0		 , 0		 , 0		 , 1         };
	double C24data[16] = { R2.data[0], R2.data[1], R2.data[2], -V.data[2],
					       R2.data[3], R2.data[4], R2.data[5], -V.data[5],
						   R2.data[6], R2.data[7], R2.data[8], -V.data[8],
						   0		 , 0		 , 0		 , 1         };	
	cv::Mat C1(4,4,CV_64FC1,C1data),
			C21(4,4,CV_64FC1,C21data),
			C22(4,4,CV_64FC1,C22data),
			C23(4,4,CV_64FC1,C23data),
			C24(4,4,CV_64FC1,C24data);

	cv::Mat point4D;
	cv::vector<Point2d> point1; point1.push_back(bestPoints1.front());
	cv::vector<Point2d> point2; point2.push_back(bestPoints2.front());
	cv::triangulatePoints(C1, C21, point1, point2, point4D);
	*/

