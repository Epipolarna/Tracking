// NonLinear.cpp

#include "Modules/NonLinear.h"

using namespace cv;
using namespace std;




namespace NonLinear
{

	
	void optimize_THIS(double* p, double* error, int m, int n, void* derp)
	{	
		error[0] = pow(p[0]-1,2);
	}
	
	void NonLinear::testFunc(double* params, double* residuals)
	{	
		dlevmar_dif(optimize_THIS, params, NULL, 1,1,10000,NULL,NULL,NULL,NULL,NULL);
	}

	void getOptimalF(double* p, double* error, int m, int n, void* derp)
	{
		goldStandardData* adata = static_cast<goldStandardData*>(derp);
		
		Mat temp, F;
		adata->F = Mat(3,3,CV_64FC1,p);
		for(int i = 0; i < adata->nPoints; ++i)
		{
			temp  = adata->points2.row(i) * (adata->F) * adata->points1.col(i);
			error[i] = temp.at<double>(0,0) * temp.at<double>(0,0);
		}
	}




	void NonLinear::goldStandardRefine(cv::Mat _F, vector<Point2d> _points1, vector<Point2d> _points2)
	{
		int nPoints = _points1.size();
		cv::Mat F = _F;
		goldStandardData adata;
		
		
		Mat points1(3, _points1.size(), CV_64FC1);
		Mat points2(_points1.size(), 3, CV_64FC1);
		for(int i = 0; i < _points1.size(); ++i)
		{
			points1.at<double>(0,i) = _points1[i].x;
			points1.at<double>(1,i) = _points1[i].y;
			points1.at<double>(2,i) = 1;
			points2.at<double>(i,0) = _points2[i].x;
			points2.at<double>(i,1) = _points2[i].y;
			points2.at<double>(i,2) = 1;
		}

		//cout << points2.size() << endl;

		adata.nPoints = nPoints;
		adata.points1 = points1;
		adata.points2 = points2;
		adata.F = F;

		double* fElems = adata.F.ptr<double>();
		
		//DEBUG
		/*
		double epDist = 0;
		Mat temp;
		for(int i = 0; i < nPoints; i++)
		{
			temp  = adata.points2.col(i).t() * (adata.F) * adata.points1.col(i);
			cout << temp.at<double>(0,0)* temp.at<double>(0,0) << endl;
			cout << "points are: " << adata.points2.col(i).t() << " and " << adata.points1.col(i) << endl;
			epDist += temp.at<double>(0,0) * temp.at<double>(0,0);
		}
		cout << fElems[0] << endl;
		cout << fElems[1] << endl;
		cout << fElems[2] << endl;
		cout << fElems[3] << endl;
		cout << fElems[4] << endl;
		cout << fElems[5] << endl;
		cout << fElems[6] << endl;
		cout << fElems[7] << endl;
		cout << fElems[8] << endl;
		cout << "total epipolar distance before: " << epDist << endl;
		*/
		// Where it happens
		double info[LM_INFO_SZ];
		int ret;
		ret = dlevmar_dif(getOptimalF, fElems, NULL, 9,nPoints,10000,NULL,info,NULL,NULL,&adata);
		printf("Levenberg-Marquardt returned in %g iter, reason %g, output error %g with an initial error of [%g]\n", info[5], info[6], info[1], info[0]);
		
		/*
		epDist = 0;
		adata.F = Mat(3,3,CV_64FC1,fElems);
		for(int i = 0; i < nPoints; ++i)
		{
			temp  = adata.points2.col(i).t() * (adata.F) * adata.points1.col(i);
			cout << temp.at<double>(0,0)* temp.at<double>(0,0) << endl;
			epDist += temp.at<double>(0,0) * temp.at<double>(0,0);
		}
		*/		
		cout << fElems[0] << endl;
		cout << fElems[1] << endl;
		cout << fElems[2] << endl;
		cout << fElems[3] << endl;
		cout << fElems[4] << endl;
		cout << fElems[5] << endl;
		cout << fElems[6] << endl;
		cout << fElems[7] << endl;
		cout << fElems[8] << endl;
		/*
		cout << "total epipolar distance after: " << epDist << endl;

		cout << "number of inliers " << nPoints << endl;
		*/
		
	}


}