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

cv::Mat crossMat(cv::Mat in)
{
	cv::Mat out = cv::Mat::zeros(3, 3, in.type());
	out.row(0).col(1) = -*in.row(2).col(0).ptr<double>();
	out.row(0).col(2) =  *in.row(1).col(0).ptr<double>();
	out.row(1).col(0) =  *in.row(2).col(0).ptr<double>();
	out.row(1).col(2) = -*in.row(0).col(0).ptr<double>();
	out.row(2).col(0) = -*in.row(1).col(0).ptr<double>();
	out.row(2).col(1) =  *in.row(0).col(0).ptr<double>();

	return out;
}

void standardToNormalizedCoordinates(cv::vector<cv::Point2d> & inP, cv::Mat K, cv::vector<cv::Point2d> & outP)
{
	cv::Mat Kinv = K.inv();
	for (cv::vector<cv::Point2d>::iterator i = inP.begin(); i != inP.end(); i++)
	{
		cv::Mat p = cv::Mat(*i);
		cv::vconcat(p, 1, p);
		p = Kinv*p;
		outP.push_back(cv::Point2d(p.rowRange(cv::Range(0,2)).clone()));
	}
}

void Estimate3D::init(cv::vector<cv::Point2d> & p1, cv::vector<cv::Point2d> & p2, cv::Mat & K_)
{
	K = K_.clone();
	GoldStandardOutput GO;

	cv::Mat F = getGoldStandardF(p1,p2, K, &GO);
	cv::Mat E = K.t()*F*K;

	cv::Mat RGold = GO.P2.rowRange(cv::Range(0,3)).colRange(cv::Range(0,3));
	cv::Mat tGold = GO.P2.rowRange(cv::Range(0,3)).colRange(cv::Range(3,4));

	//std::cout << "\nGoldF: \n" << F << "\n";

	// Convert to camNormalized points
	cv::vector<cv::Point2d> p1Cnorm, p2Cnorm;
	standardToNormalizedCoordinates(GO.inlier1, K, p1Cnorm);
	standardToNormalizedCoordinates(GO.inlier2, K, p2Cnorm);

	Camera * cam1 = new Camera();
	Camera * cam2 = new Camera();
	cameras.push_back(cam1);
	cameras.push_back(cam2);

	cam1->id = 0;
	cam2->id = 1;
	cam1->K = K;
	cam2->K = K;

	estimateRt(E, cam2->R, cam2->t, p1Cnorm.front(), p2Cnorm.front());
		
	/*
	std::cout << "The World According to Master Klas: " << std::endl;
	std::cout << "K: " << std::endl << K << std::endl;
	std::cout << "R: " << std::endl << cam2->R << std::endl;
	std::cout << "t: " << std::endl << cam2->t << std::endl;
	std::cout << "The World According to Master GoldStandard: " << std::endl;
	std::cout << "RGold: " << std::endl << RGold << std::endl;
	std::cout << "tGold: " << std::endl << tGold << std::endl;
	*/

	cam1->R = cv::Mat::eye(3, 3, CV_64FC1);
	cam1->t = cv::Mat::zeros(3, 1, CV_64FC1);

	cv::hconcat(cam1->R, cam1->t, cam1->C);
	cv::hconcat(cam2->R, cam2->t, cam2->C);	
	cam1->P = K*cam1->C;
	cam2->P = K*cam2->C;

	cameraPair.push_back(CameraPair(cam1,cam2));
	cameraPair.back().F = F;
	cameraPair.back().E = E;

	std::cout << "Camera Properties " << std::endl << std::endl;
	std::cout << "cam1->R " << std::endl << cam1->R << std::endl;
	std::cout << "cam1->t " << std::endl << cam1->t << std::endl;
	std::cout << "cam1->C " << std::endl << cam1->C << std::endl << std::endl;

	std::cout << "cam1 world pose: " << std::endl << -cam1->R.t()*cam1->t << std::endl;
	std::cout << "cam2->R " << std::endl << cam2->R << std::endl;
	std::cout << "cam2->t " << std::endl << cam2->t << std::endl;
	std::cout << "cam2->C " << std::endl << cam2->C << std::endl;
	std::cout << "cam2 world pose: " << std::endl << -cam2->R.t()*cam2->t << std::endl;



	//triangulate like a BAWS
	cv:: Mat HomPoints3D;
	cv::triangulatePoints(cam1->P, cam2->P, GO.inlier1, GO.inlier2, HomPoints3D);
	HomPoints3D.convertTo(HomPoints3D, CV_64FC1);
	
	// Fill up the data hierarchy (visibility etc)
	cv::Point3d * p3d;
	int n = 5;
	for(int n = 0; n < HomPoints3D.size().width; n++)
	{
		p3d = new cv::Point3d(HomPoints3D.at<double>(0,n)/HomPoints3D.at<double>(3,n),
							  HomPoints3D.at<double>(1,n)/HomPoints3D.at<double>(3,n),
							  HomPoints3D.at<double>(2,n)/HomPoints3D.at<double>(3,n));
		//cout << *p3d << endl;
		visible3DPoint.push_back(Visible3DPoint(p3d, ObserverPair(cam1, cam2, pointPair(GO.inlier1[n],GO.inlier2[n]))));
		
		// Convert to C-normalized image coordinates
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

	cv::Mat F = getGoldStandardF(p1,p2, K, &GO);
	cv::Mat E = K.t()*F*K;

	cv::Mat RGold = GO.P2.rowRange(cv::Range(0,3)).colRange(cv::Range(0,3));
	cv::Mat tGold = GO.P2.rowRange(cv::Range(0,3)).colRange(cv::Range(3,4));

	//std::cout << "GoldF: " << F << "\n";

	// Convert to camNormalized points
	cv::vector<cv::Point2d> p1Cnorm, p2Cnorm;
	standardToNormalizedCoordinates(GO.inlier1, K, p1Cnorm);
	standardToNormalizedCoordinates(GO.inlier2, K, p2Cnorm);

	Camera * cam1 = cameras.back();
	Camera * cam2 = new Camera();
	cameras.push_back(cam2);

	cam2->id = cam1->id + 1;
	cam2->K = K;

	estimateRt(E, cam2->R, cam2->t, p1Cnorm.front(), p2Cnorm.front());
	
	/*
	std::cout << "The World According to Master Klas: " << std::endl;
	std::cout << "K: " << std::endl << K << std::endl;
	std::cout << "R: " << std::endl << cam2->R << std::endl;
	std::cout << "t: " << std::endl << cam2->t << std::endl;
	std::cout << "The World According to Master GoldStandard: " << std::endl;
	std::cout << "RGold: " << std::endl << RGold << std::endl;
	std::cout << "tGold: " << std::endl << tGold << std::endl;
	*/
	
	 cam2->t = cam2->R*cam1->t + cam2->t;
	 cam2->R = cam2->R*cam1->R;

	 cv::hconcat(cam2->R, cam2->t, cam2->C); 
	 cam2->P = K*cam2->C;

	 cameraPair.push_back(CameraPair(cam1,cam2));
	 cameraPair.back().F = F;
	 cameraPair.back().E = E;

	std::cout << "Camera Properties " << std::endl << std::endl;
	std::cout << "cam1->R " << std::endl << cam1->R << std::endl;
	std::cout << "cam1->t " << std::endl << cam1->t << std::endl;
	std::cout << "cam1->C " << std::endl << cam1->C << std::endl << std::endl;

	std::cout << "cam1 world pose: " << std::endl << -cam1->R.t()*cam1->t << std::endl;
	std::cout << "cam2->R " << std::endl << cam2->R << std::endl;
	std::cout << "cam2->t " << std::endl << cam2->t << std::endl;
	std::cout << "cam2->C " << std::endl << cam2->C << std::endl;
	std::cout << "cam2 world pose: " << std::endl << -cam2->R.t()*cam2->t << std::endl;

	//triangulate like a BAWS
	cv:: Mat HomPoints3D;
	std::vector<cv::Point3d> point3D;
	cv::triangulatePoints(cam1->P, cam2->P, GO.inlier1, GO.inlier2, HomPoints3D);
	HomPoints3D.convertTo(HomPoints3D, CV_64FC1);
	for(int n = 0; n < HomPoints3D.size().width; n++)
		point3D.push_back(cv::Point3d( HomPoints3D.at<double>(0,n)/HomPoints3D.at<double>(3,n),
									   HomPoints3D.at<double>(1,n)/HomPoints3D.at<double>(3,n),
									   HomPoints3D.at<double>(2,n)/HomPoints3D.at<double>(3,n)));
	
	std::vector<int> inlierMask;
	//NonLinear::PnPSolver(*cam2);
	//NonLinear::PnPSolverOCV(*cam2, GO.inlier2, point3D, inlierMask);
	for(int n = 0; n < point3D.size(); n++)
		inlierMask.push_back(n);

	/*
	std::cout << "inlierMask.size(): " << inlierMask.size() << "\n";
	std::cout << "point3D.size(): " << point3D.size() << "\n";
	std::cout << "cam2->R " << std::endl << cam2->R << std::endl;
	std::cout << "cam2->t " << std::endl << cam2->t << std::endl;
	std::cout << "cam2->C " << std::endl << cam2->C << std::endl;
	*/

	cv::Point3d * p3d;
	cv::Mat a,c,r;
	int m = 1;
	int nonUniqueAmount = 0;
	// Fill up the data hierarchy (visibility etc)
	int n;
	for(int i = 0; i < inlierMask.size(); i++)
	{
		n = inlierMask[i];
		p3d = new cv::Point3d(point3D[n]);
		if(isUnique3DPoint(&p3d, cam1, GO.inlier1[n]))
		{
			visible3DPoint.push_back(Visible3DPoint(p3d, ObserverPair(cam1, cam2, pointPair(GO.inlier1[n],GO.inlier2[n]))));

			cam1->imagePoints.push_back(GO.inlier1[n]);
			cam2->imagePoints.push_back(GO.inlier2[n]);

			cam1->visible3DPoints.push_back(p3d);
			cam2->visible3DPoints.push_back(p3d);
			cameraPair.back().pointPairs.push_back(pointPair(GO.inlier1[n], GO.inlier2[n]));
			cameraPair.back().point3Ds.push_back(p3d);
			/*a = cv::Mat(*p3d);
			c = cam1->R*a + cam1->t;	// Change coordinate system to the global coordinate system(the one of the initial first camera)
			*p3d = cv::Point3d(c);
			*/
		}
		else
		{
			nonUniqueAmount++;
			// TODO: Only add the 3D points to the new camera, and add the pairs.. (etc..)
			cam2->imagePoints.push_back(GO.inlier2[n]);
			cam2->visible3DPoints.push_back(p3d);
			cameraPair.back().pointPairs.push_back(pointPair(GO.inlier1[n], GO.inlier2[n]));
			cameraPair.back().point3Ds.push_back(p3d);
			//std::cout << m << ") Unique point found!";
			m++;
		}
	}
	std::cout << "\n\n" << nonUniqueAmount << " non unique found!! :D\n\n";
	errorFile << "Number of connections (nonUnique): " << nonUniqueAmount << "\n";
	logFile << "Number of connections (nonUnique): " << nonUniqueAmount << "\n";
}


bool LessThanVisible3DPoint(Visible3DPoint & left, Visible3DPoint & right) { return point3dLessThan()(*left.point3D, *right.point3D); }

bool Estimate3D::isUnique3DPoint(cv::Point3d ** p3D, Camera * c, cv::Point2d & p2D)
{
	for(int n = 0; n < c->imagePoints.size(); n++)
	{
		if(cv::norm(c->imagePoints[n] - p2D) < 10)
		{
			delete *p3D;
			*p3D = c->visible3DPoints[n];
			return false;
		}
	}

	
	double margin = 0.05;
	cv::Point3d * closestMatch = 0;
	double minError = 1000000000;
	int e;

	//std::sort(visible3DPoint.begin(), visible3DPoint.end(), LessThanVisible3DPoint);

	for(std::vector<Visible3DPoint>::iterator i = visible3DPoint.begin(); i != visible3DPoint.end(); i++)
	{
		if(norm(*i->point3D - (**p3D)) < margin)
		{
			minError = norm(*i->point3D - (**p3D));
			closestMatch = i->point3D;
		}
	}

	if(closestMatch != 0)
	{
		delete *p3D;
		*p3D = closestMatch;
		return false;
	}

	

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
		return cv::Mat(3,3,CV_64FC1,Ydata).clone();
	}
	else if(X.size().height == 3 && X.size().width == 3)
	{
		float Ydata[] = {  X.at<float>(1,2),	X.at<float>(2,0),  X.at<float>(0,1)  };
		return cv::Mat(1,2,CV_64FC1,Ydata).clone();
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
	return cv::Mat(3,4,CV_64FC1,Cdata).clone();
}

// Gold standard F. points have to be ordered with correspondances at the same indices.
cv::Mat getGoldStandardF(cv::vector<cv::Point2d> & points1, cv::vector<cv::Point2d> & points2,  cv::Mat _K, GoldStandardOutput * Gout, int RANSAC_threshold)
{
	NonLinear::NonLinear nonlin(_K);
	cv::vector<uchar> inlierMask;
	cv::Mat P1,P2,dinoHomPoints;
	// inital Fhat
	cv::Mat F = findFundamentalMat(points1, points2, CV_FM_RANSAC, RANSAC_threshold, 0.99, inlierMask);

	//Extract the inliers in the data
	cv::vector<cv::Point2d> inliers1;
	cv::vector<cv::Point2d> inliers2;

	// Limit the number of points to reduce calculations
	int pointLimit = 100;

	for (int i = 0; i < inlierMask.size(); ++i)
	{
		if(inlierMask[i] != 0 && inliers1.size() < pointLimit)
		{
			inliers1.push_back(points1[i]);
			inliers2.push_back(points2[i]);
		}
	}

	std::cout << "RANSAC reduced " << points1.size() << " points to " << inliers1.size() << std::endl;

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
	//std::cout << "OpenCV F before GS: " << F.clone()/F.at<double>(2,2) << std::endl;
	//nonlin.goldStandardRefine(F.clone()/F.at<double>(2,2), inliers1,inliers2);
	
	
	//Actual call to the nonlinear part of the goldstandard estimation
	clock_t t;
	t = clock();
	F = nonlin.goldNonLin(F, P1, P2, normalized3Dpoints,inliers1,inliers2);
	F = F/F.at<double>(2,2);	// Normalize
	std::cout << "Gold standard nonlin time: " << ((float)(clock() - t))/CLOCKS_PER_SEC << std::endl;

	//std::cout << "Refined Fundamental Matrix: " << F/F.at<double>(2,2) << std::endl;
	//std::cout << "det(F): " << determinant(F) << std::endl;
	//return F/F.at<double>(2,2);
	//std::cout << "\n\n";
	//std::cout << "OpenCV F AFTER GS: " << F.clone()/F.at<double>(2,2) << std::endl;
	//nonlin.goldStandardRefine(F.clone(), inliers1,inliers2);

	//std::cout << "Refined Fundamental Matrix: " << F << std::endl;
	//std::cout << "det(F): " << determinant(F) << std::endl;


	if(Gout != 0)
	{
		Gout->P1 = P1;
		Gout->P2 = P2;
		Gout->inlier1 = cv::vector<cv::Point2d>(inliers1.begin(), inliers1.end());
		Gout->inlier2 = cv::vector<cv::Point2d>(inliers2.begin(), inliers2.end());
		Gout->point3D = normalized3Dpoints;
	}

	return F;
}


void estimateRt(cv::Mat& E, cv::Mat& R, cv::Mat& t, cv::Point2f p1, cv::Point2f p2)
{
	using namespace std;
	cv::Mat U, S, V, Vt, W, R1, R2, C1, C21, C22, C23, C24, t1, t2, x1, x2, x3, x4, x21, x22, x23, x24, y11, y21, y22, y23, y24;

	

	// Decompose Essential matrix
	cv::SVDecomp(E, S, U, Vt);
	V = Vt.t();

	// Translation is found as third column of V
	t1 = V.col(2);
	cv::normalize(t1, t1);

	// Define W. Will be used to find R
	double Wdata[] = {0, 1, 0,
				     -1, 0, 0,
				      0, 0, 1};
	W = cv::Mat(3, 3, CV_64FC1, Wdata);

	// Calculate R
	R1 = V*W.t()*U.t();
	R2 = V*W*U.t();
	//R1 = R1.t();
	//R2 = R2.t();

	
	// Same initial, only sign should differ
	t2 = t1;
	
	// No mirroring please
	
	if(cv::determinant(R1) < 0)
	{
		std::cout << "\n! DETERMINANT of R1 == " << cv::determinant(R1) << "\n\n";
		t1 = cv::determinant(R1)*t1;
		R1 = cv::determinant(R1)*R1;
	}
	if(cv::determinant(R2) < 0)
	{
		std::cout << "\n! DETERMINANT of R2 == " << cv::determinant(R2) << "\n\n";
		t2 = cv::determinant(R2)*t2;
		R2 = cv::determinant(R2)*R2;
	}


	C1 = cv::Mat::eye(3,4,CV_64FC1);

	// Create the 4 possible cameras
	cv::hconcat(R1, t1, C21);
	cv::hconcat(R1,-t1, C22);
	cv::hconcat(R2, t2, C23);
	cv::hconcat(R2,-t2, C24);

	// Triangulate the 4 possible 3D-points
	cv::triangulatePoints(C1, C21, cv::Mat(p1), cv::Mat(p2), x1);
	cv::triangulatePoints(C1, C22, cv::Mat(p1), cv::Mat(p2), x2);
	cv::triangulatePoints(C1, C23, cv::Mat(p1), cv::Mat(p2), x3);
	cv::triangulatePoints(C1, C24, cv::Mat(p1), cv::Mat(p2), x4);

	x1.convertTo(x1, CV_64FC1);
	x2.convertTo(x2, CV_64FC1);
	x3.convertTo(x3, CV_64FC1);
	x4.convertTo(x4, CV_64FC1);

	x1 = x1.rowRange(cv::Range(0,3)).clone()/x1.at<double>(3,0);
	x2 = x2.rowRange(cv::Range(0,3)).clone()/x2.at<double>(3,0);
	x3 = x3.rowRange(cv::Range(0,3)).clone()/x3.at<double>(3,0);
	x4 = x4.rowRange(cv::Range(0,3)).clone()/x4.at<double>(3,0);
	
	// Calculate position of 3D point relative the four posible cameras
	x21 = R1*x1 + t1;
	x22 = R1*x2 - t1;
	x23 = R2*x3 + t2;
	x24 = R2*x4 - t2;
	
	cout << "x1: " << x1 << endl;
	cout << "x21: " << x21 << endl;
	cout << "x2: " << x2 << endl;
	cout << "x22: " << x22 << endl;
	cout << "x3: " << x3 << endl;
	cout << "x23: " << x23 << endl;
	cout << "x4: " << x4 << endl;
	cout << "x24: " << x24 << endl;

	if ( x21.at<double>(2,0) > 0 && x1.at<double>(2,0) > 0 )
	{
		cout << "Configuration 1" << endl;
		R = R1;
		t = t1;
	}
	else if ( x22.at<double>(2,0) > 0 && x2.at<double>(2,0) > 0 )
	{
		cout << "Configuration 2" << endl;
		R = R1;
		t = -t1;
	}
	else if ( x23.at<double>(2,0) > 0 && x3.at<double>(2,0) > 0 )
	{
		cout << "Configuration 3" << endl;
		R = R2;
		t = t2;
	}
	else if ( x24.at<double>(2,0) > 0 && x4.at<double>(2,0) > 0 )
	{
		cout << "Configuration 4" << endl;
		R = R2;
		t = -t2;
	}
	else
	{
		std::cout << "!!!!!!!!!!!!!!!!!!!!!!\n!!!!!!!!!!!!!!!!!!!!!!!!!\n - - - ERROR: no R & t found?! - - -\n!!!!!!!!!!!!!!!!!!!!!!!!!\n!!!!!!!!!!!!!!!!!!!!!!!!!\n";
	}
	R = R.t();
	//t = -R.t()*t;
	t = -t;
	
	//std::cout << "Chosen t: \n" << t << "\n";
	//std::cout << "Chosen R: \n" << R << "\n";
		
}


void Estimate3D::saveToFile(std::string path)
{
	std::ofstream file(path, std::ios::binary);
	file.precision(20);
	if(!file)
	{
		std::cout << "! Could not open file \"" << path << "\", could not load Estimate3D model\n";
		return;
	}
	int n;
	std::stringstream ss;
	ss.precision(20);

	// Unique 3D-points
	file << "#Unique 3D-points (" << visible3DPoint.size() << ")\n";
	for(std::vector<Visible3DPoint>::iterator i = visible3DPoint.begin(); i != visible3DPoint.end(); i++)
	{
		file << "p3D = " << *i->point3D << "\n";
		for(std::list<ObserverPair>::iterator c = i->observerPair.begin(); c != i->observerPair.end(); c++)
		{
			file << "  C1.id = " << c->camera1->id << "\n";
			file << "  C2.id = " << c->camera2->id << "\n";
			file << "   p2D1 = " << c->point2D.p1 << "\n";
			file << "   p2D2 = " << c->point2D.p2 << "\n";
		}
	}
	file << "\n";
	
	// Cameras
	n = 0;
	for(std::list<Camera*>::iterator i = cameras.begin(); i != cameras.end(); i++)
	{
		file << "#Camera" << n+1 << " (of " << cameras.size() << ")\n";
		file << "id = " << (*i)->id << "\n";
		file << "P  = \n " << (*i)->P << "\n";
		file << "K  = \n " << (*i)->K << "\n";
		file << "C  = \n " << (*i)->C << "\n";
		file << "R  = \n " << (*i)->R << "\n";
		file << "t  = " << (*i)->t << "\n";
		file << " #image points (" << (*i)->imagePoints.size() << ")\n";
		for(std::vector<cv::Point2d>::iterator p = (*i)->imagePoints.begin(); p != (*i)->imagePoints.end(); p++)
		{
			file << "p2D = " << *p << "\n";
		}
		file << " #visible 3D-points (" << (*i)->visible3DPoints.size() << ")\n";
		for(std::vector<cv::Point3d*>::iterator p = (*i)->visible3DPoints.begin(); p != (*i)->visible3DPoints.end(); p++)
		{
			file << "p3D = " << **p << "\n";
		}
		n++;
	}
	file << "\n";

	// CameraPairs
	file << "#Camera pair (" << cameraPair.size() << ")\n";
	for(std::vector<CameraPair>::iterator i = cameraPair.begin(); i != cameraPair.end(); i++)
	{
		file << "C1.id = " << i->camera1->id << "\n";
		file << "C2.id = " << i->camera2->id << "\n";
		file << " F = \n " << i->F << "\n";
		file << " E = \n " << i->E << "\n";
		file << " #point pairs (" << i->pointPairs.size() << ")\n";
		for(std::vector<pointPair>::iterator p = i->pointPairs.begin(); p != i->pointPairs.end(); p++)
		{
			file << "p2D 1 = " << p->p1 << "\n";
			file << "p2D 2 = " << p->p2 << "\n";
		}
		file << " #point 3Ds (" << i->point3Ds.size() << ")\n";
		for(std::vector<cv::Point3d*>::iterator p = i->point3Ds.begin(); p != i->point3Ds.end(); p++)
		{
			file << "p3D = " << **p << "\n";
		}
	}

	file.close();
}

void Estimate3D::loadFromFile(std::string path)
{
	std::ifstream file(path, std::ios::binary);
	file.precision(20);
	if(!file)
	{
		std::cout << "! Could not open file \"" << path << "\", could not save Estimate3D model\n";
		return;
	}
	std::string line;
	std::stringstream ss;
	ss.precision(20);
	int pos, amount;
	double x,y,z;
	int id1,id2;
	char tmp; 
	cv::Point2d p2d,p2d2;
	cv::Point3d p3d;
	std::map<cv::Point3d,cv::Point3d*,point3dLessThan> pointMap;
	std::map<int,Camera *> cameraMap;
	while(std::getline(file, line))
	{
		if(pos = line.find("#Unique") != std::string::npos)
		{
			std::getline(file, line);
			while(line[0] == 'p')
			{
				parser.parsePoint3d(line, p3d);
				visible3DPoint.push_back(Visible3DPoint(new cv::Point3d(p3d)));
				pointMap[p3d] = visible3DPoint.back().point3D;
				std::getline(file, line);
				do
				{
					parser.parseVariable(line, id1);	// C1.id
					std::getline(file, line);
					parser.parseVariable(line, id2);	// C2.id
					std::getline(file, line);
					parser.parsePoint2d(line, p2d);
					std::getline(file, line);
					parser.parsePoint2d(line, p2d2);
					visible3DPoint.back().observerPair.push_back(ObserverPair((Camera*)id1, (Camera*)id2, pointPair(p2d,p2d2))); // Must be reprocessed in the end (when the cameras have been parsed..)
					std::getline(file, line);
				} while(line.find("C1.id") != std::string::npos);
			}
			
		}
		else
		if(pos = line.find("#Camera pair") != std::string::npos)
		{
			std::getline(file, line);
			parser.parseVariable(line, x);
			std::getline(file, line);
			parser.parseVariable(line, y);
			cameraPair.push_back(CameraPair(cameraMap[x], cameraMap[y]));
			parser.parseMatrix(file, cameraPair.back().F);
			parser.parseMatrix(file, cameraPair.back().E);
			std::getline(file, line);	//#point pairs (..)
			std::getline(file, line);
			while(line[0] == 'p')
			{
				parser.parsePoint2d(line, p2d);
				std::getline(file, line);
				parser.parsePoint2d(line, p2d2);
				cameraPair.back().pointPairs.push_back(pointPair(p2d,p2d2));
				std::getline(file, line);
			}	
			do                         //#point 3Ds (..)
			{
				std::getline(file, line);
				parser.parsePoint3d(line, p3d);
				cameraPair.back().point3Ds.push_back(pointMap[p3d]);
			}while(file.peek() == 'p');

		}
		else
		if(pos = line.find("#Camera") != std::string::npos)
		{
			cameras.push_back(new Camera());
			parser.parseVariable(file, cameras.back()->id);
			parser.parseMatrix(file, cameras.back()->P);
			parser.parseMatrix(file, cameras.back()->K);
			parser.parseMatrix(file, cameras.back()->C);
			parser.parseMatrix(file, cameras.back()->R);
			parser.parseMatrix(file, cameras.back()->t);
			std::getline(file, line);	//#image points (..)
			std::getline(file, line);
			while(line[0] == 'p')
			{
				parser.parsePoint2d(line, p2d);
				cameras.back()->imagePoints.push_back(p2d);
				std::getline(file, line);
			}	
			do                         //#visible 3D-points (..)
			{
				std::getline(file, line);
				parser.parsePoint3d(line, p3d);
				cameras.back()->visible3DPoints.push_back(pointMap[p3d]);
			}while(file.peek() == 'p');

			// Prepare lookup for Camera pair construction
			cameraMap[cameras.back()->id] = cameras.back();
		}
	}

	// Post process
	// Build ObserverPairs by pointing to the actual cameras
	for(std::vector<Visible3DPoint>::iterator v = visible3DPoint.begin(); v != visible3DPoint.end(); v++)
	{
		for(std::list<ObserverPair>::iterator o = v->observerPair.begin(); o != v->observerPair.end(); o++)
		{
			o->camera1 = cameraMap[(int)o->camera1];
			o->camera2 = cameraMap[(int)o->camera2];
		}
	}

	file.close();
}

void Estimate3D::clear()
{
	visible3DPoint.clear();
	cameras.clear();
	cameraPair.clear();
	views.clear();
}


bool point3dLessThan::operator()(const cv::Point3d & left, const cv::Point3d & right) const
{
	return left.x < right.x || (left.x == right.x && left.y < right.y) || (left.x == right.x && left.y == right.y && left.z < right.z);
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

