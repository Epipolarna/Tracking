// NonLinear.cpp

#include "NonLinear.h"



using namespace cv;
using namespace std;




namespace NonLinear
{
	
	NonLinear::NonLinear(cv::Mat _K)
	{
		this->K = _K.clone();
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

		//tempPoints3D = Mat(3,adata->nPoints3D,CV_64FC1,&p[12]);
		adata->points3D = Mat(3,adata->nPoints3D,CV_64FC1,&p[12]);
		adata->F = Mat(3,3,CV_64FC1,p);
		
		setDataRange(p,adata->C2.ptr<double>(),0,12);

		for(int i = 0; i < adata->nPoints3D; ++i)
		{
			vconcat(adata->points3D.col(i),one,tempPoint3D);
			temppoint1 = adata->C1*tempPoint3D;
			temppoint2 = adata->C2*tempPoint3D;
			
			error[4*i] = adata->points1[i].x - temppoint1.ptr<double>()[0]/temppoint1.ptr<double>()[2];
			error[4*i+1] = adata->points1[i].y - temppoint1.ptr<double>()[1]/temppoint1.ptr<double>()[2];
			error[4*i+2] = adata->points2[i].x - temppoint2.ptr<double>()[0]/temppoint2.ptr<double>()[2];
			error[4*i+3] = adata->points2[i].y - temppoint2.ptr<double>()[1]/temppoint2.ptr<double>()[2];

			//error[4*i] = adata->points1[i].x - temppoint1.ptr<double>()[0]/temppoint1.ptr<double>()[2];
			//error[4*i+1] = adata->points1[i].y - temppoint1.ptr<double>()[1]/temppoint1.ptr<double>()[2];
			//error[4*i+2] = adata->points2[i].x - temppoint2.ptr<double>()[0]/temppoint2.ptr<double>()[2];
			//error[4*i+3] = adata->points2[i].y - temppoint2.ptr<double>()[1]/temppoint2.ptr<double>()[2];
			
		}
	}


	cv::Mat NonLinear::goldNonLin(cv::Mat _F, cv::Mat C1, cv::Mat C2, cv::Mat _points3D, std::vector<cv::Point2d> _points1, std::vector<cv::Point2d> _points2)
	{
		int n3Dpoints = max(_points3D.size().width, _points3D.size().height);
		int nPoints = (int)_points1.size();
		goldNonLinData adata;
		std::vector<double> params;
		
		//Setup of optimization data
		adata.params = &params;
		adata.K = this->K.clone();
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
		int nPoints = (int)_points1.size();
		cv::Mat F = _F;
		goldStandardData adata;
		
		
		Mat points1(3, (int)_points1.size(), CV_64FC1);
		Mat points2((int)_points1.size(), 3, CV_64FC1);
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


	void BAResiduals(double* p, double* error, int m, int n, void* derp)
	{
		int errorIdx = 0;
		int paramIdx = 0;
		int pointIdx = 0;
		BAData* data = static_cast<BAData*>(derp);
		//Rebuild 3Dpoints (sigh)
		//data->all3DPoints = Mat(3,data->n3DPoints,CV_64FC1,&p[data->nViews*6]);
		/*
		for(int i = 0; i < data->n3DPoints; i++)
		{
			(*data)->all3DPoints[i] = p[data->nViews*6 + 3*i];
			(*data->all3DPoints)[i]->y = p[data->nViews*6 + 3*i + 1];
			(*data->all3DPoints)[i]->z = p[data->nViews*6 + 3*i + 2];
		}*/


		for(std::vector<Visible3DPoint>::iterator it = data->all3DPoints->begin(); it != data->all3DPoints->end(); it++)
		{
			it->point3D->x = p[data->nViews*6 + 3*pointIdx];
			it->point3D->y = p[data->nViews*6 + 3*pointIdx + 1];
			it->point3D->z = p[data->nViews*6 + 3*pointIdx + 2];
			pointIdx++;
		}

		// For all cameras
		for (int i = 0; i < data->rotations.size(); i++)
		{
			//Extract rotation and translation vectors
			//setDataRange(p,data->rVec.ptr<double>(),i*6,i*6+3);
			//setDataRange(p,data->t.ptr<double>(),i*6+3,(i+1)*6);
			if (i != 0)
			{
				setDataRange(p,data->rotations[i].ptr<double>(),(i-1)*6,(i-1)*6+3);
				setDataRange(p,data->translations[i].ptr<double>(),(i-1)*6+3,(i+0)*6);
			}
			
			
			//setDataRange(p,data->rotations[i].ptr<double>(),i*6,i*6+3);
			//setDataRange(p,data->translations[i].ptr<double>(),i*6+3,(i+1)*6);
			//cout << "rVec: " << data->rVec << endl;
			//cout << "tVec: " << data->t << endl;
			
			Rodrigues(data->rotations[i], data->R);
			//cout << "R: " << data->R << endl;
			hconcat(data->R, data->translations[i], data->C);
			//cout << "C: " << data->C << endl;
			//cout << "K: " << data->K << endl;
			data->P = data->K * data->C;
			//cout << "P: " << data->P << endl;
			// for all visible 3D points, calculate the reprojection error
			for(int j = 0; j < data->points3D[i].size(); j++)
			{
				// Get the 3D point in hom. coords
				data->point3D.ptr<double>()[0] = data->points3D[i][j]->x;
				data->point3D.ptr<double>()[1] = data->points3D[i][j]->y;
				data->point3D.ptr<double>()[2] = data->points3D[i][j]->z;
				data->point3D.ptr<double>()[3] = 1;
				//cout << "3D point: " << data->point3D << endl;
				
				// Project onto image plane
				data->point2D = data->P * data->point3D;
				//cout << data->point2D.ptr<double>()[0] / data->point2D.ptr<double>()[2] << endl;
				//cout << data->point2D.ptr<double>()[1] / data->point2D.ptr<double>()[2] << endl;
				//Calculate residuals.
				error[errorIdx] = (*data->imagePoints[i])[j].x - data->point2D.ptr<double>()[0] / data->point2D.ptr<double>()[2];
				error[errorIdx+1] = (*data->imagePoints[i])[j].y - data->point2D.ptr<double>()[1] / data->point2D.ptr<double>()[2];
				errorIdx += 2;
				//cout << "errors: " << error[errorIdx] << " " << error[errorIdx + 1] << endl;
			}
		}
	}

	void BAResiduals2(double* p, double* error, int m, int n, void* derp)
	{
		int errorIdx = 0;
		int paramIdx = 0;
		int pointIdx = 0;
		BAData* data = static_cast<BAData*>(derp);

		for(std::vector<Visible3DPoint>::iterator it = data->all3DPoints->begin(); it != data->all3DPoints->end(); it++)
		{
			it->point3D->x = p[(data->nViews-1)*6 + 3*pointIdx];
			it->point3D->y = p[(data->nViews-1)*6 + 3*pointIdx + 1];
			it->point3D->z = p[(data->nViews-1)*6 + 3*pointIdx + 2];
			pointIdx++;
		}

		// For all cameras
		for (int i = 0; i < data->rotations.size(); i++)
		{
			//Extract rotation and translation vectors
			//setDataRange(p,data->rVec.ptr<double>(),i*6,i*6+3);
			//setDataRange(p,data->t.ptr<double>(),i*6+3,(i+1)*6);
			//cout << "R: " << data->rotations[i] << endl;
			//cout << "t: " << data->translations[i] << endl;
			//for(int k = 0; k < 6; k++)
			//{
				//cout << "the fucking params: " <<  p[6*i + k] << std::endl;
			//}
			if (i != 0)
			{
				setDataRange(p,data->rotations[i].ptr<double>(),(i-1)*6,(i-1)*6+3);
				setDataRange(p,data->translations[i].ptr<double>(),(i-1)*6+3,(i+0)*6);
			}
				//cout << "rVec: " << data->rVec << endl;
			//cout << "tVec: " << data->t << endl;
			//cout << data->rotations[i] << endl;
			//cout << data->translations[i] << endl;
			Rodrigues(data->rotations[i], data->R);
			//cout << "R: " << data->rotations[i] << endl;
			//cout << "R: " << data->translations[i] << endl;
			hconcat(data->R, data->translations[i], data->C);
			//cout << "C: " << data->C << endl;
			//cout << "K: " << data->K << endl;
			//data->P = data->K * data->C;
			//cout << "P: " << data->P << endl;
			// for all visible 3D points, calculate the reprojection error
			std::vector<double> derp; 
			std::vector<Point2f> some2dVec;
			cv::Point3f temp3Dp;
			data->float3DPs.resize(data->points3D[i].size());
			some2dVec.resize(data->points3D[i].size());
			//data->float2DPs.resize(data->points3D[i].size());
			for(int j = 0; j < data->points3D[i].size(); j++)
			{
				// Get the 3D point in hom. coords
				temp3Dp = Point3f(*(data->points3D[i][j]));
				//cout << temp3Dp << "  " << *(data->points3D[i][j]) << endl;
				data->float3DPs[j] = temp3Dp;
			}
			projectPoints(data->float3DPs,data->rotations[i],data->translations[i],data->K, derp, some2dVec);
			for(int j = 0; j < data->points3D[i].size(); j++)
			{
				//cout << "3D point: " << data->point3D << endl;
				//cout << some2dVec[j] << endl;
				// Project onto image plane
				//data->point2D = data->P * data->point3D;
				//cout << data->point2D.ptr<double>()[0] / data->point2D.ptr<double>()[2] << endl;
				//cout << data->point2D.ptr<double>()[1] / data->point2D.ptr<double>()[2] << endl;
				//Calculate residuals.
				error[errorIdx] = (float)(*data->imagePoints[i])[j].x - some2dVec[j].x;
				error[errorIdx+1] = (float)(*data->imagePoints[i])[j].y - some2dVec[j].y;
				errorIdx += 2;
				//cout << "errors: " << error[errorIdx] << " " << error[errorIdx + 1] << endl;
			}
			some2dVec.clear();
		}
	}



	void NonLinear::BundleAdjust(std::list<Camera*>& views, std::vector<Visible3DPoint>* _all3DPoints)
	{
		int nPoints = 0;
		//I want this :/
		//cv::Mat ALL3DPOINTS(3,1337,CV_64FC1);
		//data.n3DPoints = ALL3DPOINTS.size().width;
		
		BAData data;
		cv::Mat rVec;
		std::vector<double> params;
		int residualTerms = 0;
		int pointcounter = 0;
		//data.K = this->K;
		data.nViews = (int)views.size();
		

		//preallocate everything
		double oneData[1] = {1};
		data.one = cv::Mat(1,1,CV_64FC1,oneData);
		data.R = cv::Mat::eye(3,3,CV_64FC1);
		data.rVec = cv::Mat(3,1,CV_64FC1);
		data.t = cv::Mat(3,1,CV_64FC1);
		data.point2D = cv::Mat(3,1,CV_64FC1);
		data.point3D = cv::Mat(4,1,CV_64FC1);
		data.C = cv::Mat(3,4,CV_64FC1);
		data.P = cv::Mat(3,4,CV_64FC1);
		data.all3DPoints = _all3DPoints;
		data.K = this->K.clone();
		data.rotations.clear();
		
		//Save data.
		for(std::list<Camera*>::iterator it = views.begin(); it != views.end(); it++)
		{
			
			Rodrigues((*it)->R.clone(), rVec);
			data.rotations.push_back(rVec.clone());
			data.translations.push_back((*it)->t.clone());
			data.imagePoints.push_back(&((*it)->imagePoints));
			data.points3D.push_back(((*it)->visible3DPoints));
			residualTerms += (int)(*it)->imagePoints.size()*2;
		}
		
		//For all cameras
		std::vector<cv::Mat>::iterator itTrans = ++data.translations.begin();
		for (std::vector<cv::Mat>::iterator itRot = ++data.rotations.begin(); itRot != data.rotations.end(); itRot++)
		{
			// Rotation parameters
			params.push_back(itRot->ptr<double>()[0]);
			params.push_back(itRot->ptr<double>()[1]);
			params.push_back(itRot->ptr<double>()[2]);
			// Translation paramters
			params.push_back(itTrans->ptr<double>()[0]);
			params.push_back(itTrans->ptr<double>()[1]);
			params.push_back(itTrans->ptr<double>()[2]);
			itTrans++;
			// 3D points
		}
		//All 3D points
		/*
		for(MatIterator_<double> matIt = ALL3DPOINTS.begin<double>(); matIt != ALL3DPOINTS.end<double>(); ++matIt)
		{
			params.push_back(*matIt);
		}*/
		for(std::vector<Visible3DPoint>::iterator it = data.all3DPoints->begin(); it != data.all3DPoints->end(); it++)
		{
			params.push_back(it->point3D->x);
			params.push_back(it->point3D->y);
			params.push_back(it->point3D->z);
		}


		double* paramArray = params.data();
		double accErr = 0;
		std::vector<double> error;
		error.resize(residualTerms);
		
		BAResiduals2(paramArray, error.data(), (int)params.size(),residualTerms,&data);
		for (int i = 0; i < residualTerms; i++)
		{
			accErr += pow(error[i],2);
			//cout << error[i] << endl;
			error[i] = 0;
		}
		//cout << "INITIAL ERROR PROPER: " << accErr << endl;
		double info[LM_INFO_SZ];
		int ret;
		
		ret = dlevmar_dif(BAResiduals2, paramArray, error.data(), (int)params.size(),residualTerms,10000,NULL,info,NULL,NULL,&data);
		printf("Levenberg-Marquardt returned in %g iter, reason %g, output error %g with an initial error of [%g]\n", info[5], info[6], info[1], info[0]);
	
		//Rebuild
		itTrans = data.translations.begin();
		std::vector<cv::Mat>::iterator itRot = data.rotations.begin();
		for(std::list<Camera*>::iterator it = views.begin(); it != views.end(); it++)
		{
			cout << "final translation: " << (*itTrans) << endl;
			cout << "final rotation: " << (*itRot) << endl;
			Rodrigues((*itRot),(*it)->R);
			//vconcat(*itTrans,data.one,(*it)->t); NO U HOMOGENEOUS!
			hconcat((*it)->R,(*itTrans),(*it)->C);
			(*it)->P = this->K *  (*it)->C;
			itTrans++;
			itRot++;
		} 
	
	}


	
}