// NonLinear.cpp

#include "NonLinear.h"

using namespace cv;
using namespace std;




namespace NonLinear
{
	
	NonLinear::NonLinear()
	{
		double Kdata[9] = {	3217.328669180762, -78.606641008226180, 289.8672403229193,
							0,					2292.424143977958,  -1070.516234777778,
							0,					0,					1};
		cv::Mat K(3,3,CV_64FC1,Kdata);
	};


	Mat crossop(cv::Mat vector)
	{
		Mat derp = Mat::zeros(3,3,CV_64FC1);
		derp.at<double>(0,1) = -vector.at<double>(2,0);
		derp.at<double>(0,2) = vector.at<double>(1,0);
		derp.at<double>(1,0) = vector.at<double>(2,0);
		derp.at<double>(1,2) = -vector.at<double>(0,0);
		derp.at<double>(2,0) = -vector.at<double>(1,0);
		derp.at<double>(2,1) = vector.at<double>(0,0);
		return derp;
	}
	
	void setDataRange(double* _data, double* dest, int rangeBegin, int rangeEnd)
	{
		int elems = rangeEnd - rangeBegin;
		for (int i = 0; i < elems; ++i)
		{
			dest[i] = _data[rangeBegin + i];
		}
		return;
	}

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

	void getGoldenF(double* p, double* error, int m, int n, void* derp)
	{
		goldNonLinData* adata = static_cast<goldNonLinData*>(derp);

		double oneData[1] = {1};
		Mat one(1,1,CV_64FC1,oneData);
		Mat tempPoints3D, F, temppoint1, temppoint2, tempPoint3D;

		tempPoints3D = Mat(3,adata->nPoints3D,CV_64FC1,&p[12]);
		adata->F = Mat(3,3,CV_64FC1,p);
		
		setDataRange(p,adata->C2.ptr<double>(),0,12);

		for(int i = 0; i < adata->nPoints3D; ++i)
		{
			vconcat(tempPoints3D.col(i),one,tempPoint3D);
			temppoint1 = adata->C1*tempPoint3D;
			temppoint2 = adata->C2*tempPoint3D;
			
			error[4*i] = adata->points1[i].x - temppoint1.ptr<double>()[0]/temppoint1.ptr<double>()[2];
			error[4*i+1] = adata->points1[i].y - temppoint1.ptr<double>()[1]/temppoint1.ptr<double>()[2];
			error[4*i+2] = adata->points2[i].x - temppoint2.ptr<double>()[0]/temppoint2.ptr<double>()[2];
			error[4*i+3] = adata->points2[i].y - temppoint2.ptr<double>()[1]/temppoint2.ptr<double>()[2];
			
		}
	}


	cv::Mat NonLinear::goldNonLin(cv::Mat _F, cv::Mat C1, cv::Mat C2, cv::Mat _points3D, std::vector<cv::Point2d> _points1, std::vector<cv::Point2d> _points2)
	{
		int n3Dpoints = max(_points3D.size().width, _points3D.size().height);
		int nPoints = _points1.size();
		goldNonLinData adata;
		std::vector<double> params;
		
		//Setup of optimization data
		adata.params = &params;
		adata.K = this->K;
		adata.nPoints3D = n3Dpoints;
		adata.nPoints2D = nPoints;
		adata.F = _F;
		adata.points3D = _points3D;
		adata.points1 = _points1;
		adata.points2 = _points2;
		adata.C1 = C1;
		adata.C2 = C2;

		
		for(MatIterator_<double> matIt = adata.C2.begin<double>(); matIt != adata.C2.end<double>(); ++matIt)
		{
			params.push_back(*matIt);
		}
		for(MatIterator_<double> matIt = adata.points3D.begin<double>(); matIt != adata.points3D.end<double>(); ++matIt)
		{
			params.push_back(*matIt);
		}

		//Call levmar
		double info[LM_INFO_SZ];
		int ret;
		double* p =params.data();
		ret = dlevmar_dif(getGoldenF,p, NULL, 12 + 3* n3Dpoints,nPoints*4,10000,NULL,info,NULL,NULL,&adata);
		printf("Levenberg-Marquardt returned in %g iter, reason %g, output error %g with an initial error of [%g]\n", info[5], info[6], info[1], info[0]);

		cv::Mat M(adata.C2.clone(), cv::Range(0, 3), cv::Range(0, 3));
		cv::Mat F(3,3,CV_64FC1);
		F = crossop(adata.C2.col(3))*M;
		return F;
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

		adata.nPoints = nPoints;
		adata.points1 = points1;
		adata.points2 = points2;
		adata.F = F;

		double* fElems = adata.F.ptr<double>();
		
		
		// Where it happens
		double info[LM_INFO_SZ];
		int ret;
		ret = dlevmar_dif(getOptimalF, fElems, NULL, 9,nPoints,10000,NULL,info,NULL,NULL,&adata);
		printf("Levenberg-Marquardt returned in %g iter, reason %g, output error %g with an initial error of [%g]\n", info[5], info[6], info[1], info[0]);
		
	}


}