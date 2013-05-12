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
		//adata->F = Mat(3,3,CV_64FC1,p);
		
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
		cout << "Fundamental Matrix: " << F << endl;
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
			if (i != 0)
			{
				setDataRange(p,data->rotations[i].ptr<double>(),(i-1)*6,(i-1)*6+3);
				setDataRange(p,data->translations[i].ptr<double>(),(i-1)*6+3,(i+0)*6);
			}
			
			
			//setDataRange(p,data->rotations[i].ptr<double>(),i*6,i*6+3);
			//setDataRange(p,data->translations[i].ptr<double>(),i*6+3,(i+1)*6);
			//cout << "rVec: " << data->rotations[i] << endl;
			//cout << "tVec: " << data->translations[i] << endl;
			
			Rodrigues(data->rotations[i].clone(), data->R);
			//cout << "R: " << data->R << endl;
			hconcat(cv::Mat::eye(3,3,CV_64FC1), data->translations[i].clone(), data->C);
			//cout <<"C matrix " << data->C << endl;
			data->C = data->R * data->C;
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
			cv::Mat R = (*it)->R.clone();
			cout << "first R: \n" << R << endl << endl;
			cout << "det(R): \n" << determinant(R) << endl << endl;
			/*
			if(determinant(R) < 0)
			{
				cout << "!det == -1, fixin'\n";
				R.col(2) *= -1;
				cout << "now: det(R): \n" << determinant(R) << endl << endl;
			}
			cout << "R*R': \n" << R*R.t() << endl << endl;
			*/
			/*
			if(norm(rVec) > 3.14159265)
			{
				rVec = (rVec + 3.14159265);
			}
			Rodrigues(rVec, data.R);
			cout << "New Rod R: " << data.R << endl;
			
			Quaternion q(R);
			cv::Mat Qr = q.toMat();
			cv::Mat QR = q.toR();
			cout << "Q.R: \n" << QR << "\n\n";
			cout << "Q.r: \n" << Qr << "\n\n";
			cout << "First rVec: " << rVec << endl;
			*/
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
		
		BAResiduals(paramArray, error.data(), (int)params.size(),residualTerms,&data);
		for (int i = 0; i < residualTerms; i++)
		{
			accErr += pow(error[i],2);
			//cout << "Pixel error: " <<  error[i] << endl;
			error[i] = 0;
		}
		cout << "INITIAL ERROR: " << accErr << endl;
		double info[LM_INFO_SZ];
		int ret;
		
		ret = dlevmar_dif(BAResiduals, paramArray, error.data(), (int)params.size(),residualTerms,10000,NULL,info,NULL,NULL,&data);
		printf("Levenberg-Marquardt returned in %g iter, reason %g, output error %g with an initial error of [%g]\n", info[5], info[6], info[1], info[0]);
	
		//Rebuild
		itTrans = data.translations.begin();
		std::vector<cv::Mat>::iterator itRot = data.rotations.begin();
		for(std::list<Camera*>::iterator it = views.begin(); it != views.end(); it++)
		{
			cout << "final translation: " << (*itTrans) << endl;
			cout << "final rotation: " << (*itRot) << endl;
			Rodrigues((*itRot),(*it)->R);
			cout << "final rotation: \n" << (*it)->R << endl << endl;
			//vconcat(*itTrans,data.one,(*it)->t); NO U HOMOGENEOUS!
			hconcat((*it)->R,(*itTrans),(*it)->C);
			(*it)->P = this->K *  (*it)->C;
			itTrans++;
			itRot++;
		} 
	
	}

	
	/*
Quaternion::Quaternion(cv::Mat Rvec)
{ 
	q0 = std::cos(Rvec.ptr<double>()[0]/2);
	q1 = std::sin(Rvec.ptr<double>()[0]/2)*Rvec.ptr<double>()[1];
	q2 = std::sin(Rvec.ptr<double>()[0]/2)*Rvec.ptr<double>()[2];
	q3 = std::sin(Rvec.ptr<double>()[0]/2)*Rvec.ptr<double>()[3];
}


Quaternion::Quaternion(cv::Mat quarternion)
{ 
	q0 = quarternion.ptr<double>()[0];
	q1 = quarternion.ptr<double>()[1];
	q2 = quarternion.ptr<double>()[2];
	q3 = quarternion.ptr<double>()[3];
	normalize();
}
*/
double sign(double x)
{
	return x ? x/std::abs(x) : x;
}


cv::Mat Quaternion::toMat()
{
	cv::Mat rtrn(4,1,CV_64FC1);
	rtrn.ptr<double>()[0] = q[0];
	rtrn.ptr<double>()[1] = q[1];
	rtrn.ptr<double>()[2] = q[2];
	rtrn.ptr<double>()[3] = q[3];
	return rtrn.clone();
}

Quaternion::Quaternion(cv::Mat R)
{
	double * mat = &R.ptr<double>()[0];
	double & W = q[0];
	double & X = q[1];
	double & Y = q[2];
	double & Z = q[3];
	double S;

	double T = mat[0] + mat[4] + mat[8];	// Trace of R

	if ( T > 0.00000001 ) // To avoid large distortions!
	{
		S = std::sqrt(T+1)*2;
		W = 0.25*S;
		X = (mat[7] - mat[5]) / S;
		Y = (mat[2] - mat[6]) / S;
		Z = (mat[3] - mat[1]) / S;
	}
	else // T == 0
	{
		cout << "\n !! T = " << T << "\n\n";
		/* 
		If the trace of the matrix is equal to zero then identify
		which major diagonal element has the greatest value.
		Depending on this, calculate the following:
		*/
		if ( mat[0] > mat[4] && mat[0] > mat[8] )  {	// Column 0: 
			S  = sqrt( 1 + mat[0] - mat[4] - mat[8] ) * 2;
			X = 0.25 * S;
			Y = (mat[3] + mat[1] ) / S;
			Z = (mat[2] + mat[6] ) / S;
			W = (mat[7] - mat[5] ) / S;
		} else if ( mat[4] > mat[8] ) {					// Column 1: 
			S  = sqrt( 1 + mat[4] - mat[0] - mat[8] ) * 2;
			X = (mat[3] + mat[1] ) / S;
			Y = 0.25 * S;
			Z = (mat[7] + mat[5] ) / S;
			W = (mat[2] - mat[6] ) / S;
		} else {										// Column 2:
			S  = sqrt( 1 + mat[8] - mat[0] - mat[4] ) * 2;
			X = (mat[2] + mat[6] ) / S;
			Y = (mat[7] + mat[5] ) / S;
			Z = 0.25 * S;
			W = (mat[3] - mat[1] ) / S;
		}
	}
	normalize();	// Better safe than sorry. But it shouldn't be needed unless faulty rusty rotation matrix..
}


/*
Quaternion::Quaternion(cv::Mat R)
{
	double * m = &R.ptr<double>()[0];
	double & W = q[0];
	double & X = q[1];
	double & Y = q[2];
	double & Z = q[3];

	const double diag = m[0] + m[4] + m[8] + 1;

	if( diag > 0.0f )
	{
		const double scale = sqrtf(diag) * 2.0f; // get scale from diagonal

		// TODO: speed this up
		X = (m[5] - m[7]) / scale;
		Y = (m[6] - m[2]) / scale;
		Z = (m[1] - m[3]) / scale;
		W = 0.25f * scale;
	}
	else
	{
		if (m[0]>m[4] && m[0]>m[8])
		{
			// 1st element of diag is greatest value
			// find scale according to 1st element, and double it
			const double scale = sqrtf(1.0f + m[0] - m[4] - m[8]) * 2.0f;

			// TODO: speed this up
			X = 0.25f * scale;
			Y = (m[3] + m[1]) / scale;
			Z = (m[2] + m[6]) / scale;
			W = (m[5] - m[7]) / scale;
		}
		else if (m[4]>m[8])
		{
			// 2nd element of diag is greatest value
			// find scale according to 2nd element, and double it
			const double scale = sqrtf(1.0f + m[4] - m[0] - m[8]) * 2.0f;

			// TODO: speed this up
			X = (m[3] + m[1]) / scale;
			Y = 0.25f * scale;
			Z = (m[7] + m[5]) / scale;
			W = (m[6] - m[2]) / scale;
		}
		else
		{
			// 3rd element of diag is greatest value
			// find scale according to 3rd element, and double it
			const double scale = sqrtf(1.0f + m[8] - m[0] - m[4]) * 2.0f;

			// TODO: speed this up
			X = (m[6] + m[2]) / scale;
			Y = (m[7] + m[5]) / scale;
			Z = 0.25f * scale;
			W = (m[1] - m[3]) / scale;
		}
	}
	normalize();
}
*/

/*
Quaternion::Quaternion(cv::Mat R)
{
	const float trace = 1.0f + R.ptr<double>()[0] + R.ptr<double>()[4] + R.ptr<double>()[8];

	if (trace > 0.00001f)
	{
		const float s = sqrt(trace) * 2;
		Quaternion(
			(R.ptr<double>()[7] - R.ptr<double>()[5]) / s,
			(R.ptr<double>()[2] - R.ptr<double>()[6]) / s,
			(R.ptr<double>()[3] - R.ptr<double>()[1]) / s,
			s / 4);
	}
	else if (R.ptr<double>()[0] > R.ptr<double>()[4] && R.ptr<double>()[0] > R.ptr<double>()[8])
	{
		const float s = sqrt(1.0f + R.ptr<double>()[0] - R.ptr<double>()[4] - R.ptr<double>()[8]) * 2;
		Quaternion(
			s / 4,
			(R.ptr<double>()[3] + R.ptr<double>()[1]) / s,
			(R.ptr<double>()[2] + R.ptr<double>()[6]) / s,
			(R.ptr<double>()[7] - R.ptr<double>()[5]) / s);
	}
	else if (R.ptr<double>()[4] > R.ptr<double>()[8])
	{
		const float s = sqrt(1.0f + R.ptr<double>()[4] - R.ptr<double>()[0] - R.ptr<double>()[8]) * 2;
		Quaternion(
			(R.ptr<double>()[3] + R.ptr<double>()[1]) / s,
			s / 4,
			(R.ptr<double>()[7] + R.ptr<double>()[5]) / s,
			(R.ptr<double>()[2] - R.ptr<double>()[6]) / s);
	}
	else
	{
		const float s = sqrt(1.0f + R.ptr<double>()[8] - R.ptr<double>()[0] - R.ptr<double>()[4]) * 2;
		Quaternion(
			(R.ptr<double>()[2] + R.ptr<double>()[6]) / s,
			(R.ptr<double>()[7] + R.ptr<double>()[5]) / s,
			s / 4,
			(R.ptr<double>()[3] - R.ptr<double>()[1]) / s);
	}
	normalize();
}
*/
/*

double ReciprocalSqrt( double x ) {
	long i;
	double y, r;
	y = x * 0.5f;
	i = *(long *)( &x );
	i = 0x5f3759df - ( i >> 1 );
	r = *(double *)( &i );
	r = r * ( 1.5f - r * r * y );
	return r;
}

Quaternion::Quaternion(cv::Mat R)
{
	q[0] = (  R.ptr<double>()[0] + R.ptr<double>()[4] + R.ptr<double>()[8] + 1 ) / 4;
	q[1] = (  R.ptr<double>()[0] - R.ptr<double>()[4] - R.ptr<double>()[8] + 1 ) / 4;
	q[2] = ( -R.ptr<double>()[0] + R.ptr<double>()[4] - R.ptr<double>()[8] + 1 ) / 4;
	q[3] = ( -R.ptr<double>()[0] - R.ptr<double>()[4] + R.ptr<double>()[8] + 1 ) / 4;

	if(q[0] < 0) q[0] = 0;
	if(q[1] < 0) q[1] = 0;
	if(q[2] < 0) q[2] = 0;
	if(q[3] < 0) q[3] = 0;

	q[0] = std::sqrt(q[0]);
	q[1] = std::sqrt(q[1]);
	q[2] = std::sqrt(q[2]);
	q[3] = std::sqrt(q[3]);

	if(q[0] >= q[1] && q[0] >= q[2] && q[0] >= q[3]) {
		q[0] *= +1.0f;
		q[1] *= sign(R.ptr<double>()[7] - R.ptr<double>()[5]);
		q[2] *= sign(R.ptr<double>()[2] - R.ptr<double>()[6]);
		q[3] *= sign(R.ptr<double>()[3] - R.ptr<double>()[1]);
	} else if(q[1] >= q[0] && q[1] >= q[2] && q[1] >= q[3]) {
		q[0] *= sign(R.ptr<double>()[7] - R.ptr<double>()[5]);
		q[1] *= +1.0f;
		q[2] *= sign(R.ptr<double>()[3] + R.ptr<double>()[1]);
		q[3] *= sign(R.ptr<double>()[2] + R.ptr<double>()[6]);
	} else if(q[2] >= q[0] && q[2] >= q[1] && q[2] >= q[3]) {
		q[0] *= sign(R.ptr<double>()[2] - R.ptr<double>()[6]);
		q[1] *= sign(R.ptr<double>()[3] + R.ptr<double>()[1]);
		q[2] *= +1.0f;
		q[3] *= sign(R.ptr<double>()[7] + R.ptr<double>()[5]);
	} else if(q[3] >= q[0] && q[3] >= q[1] && q[3] >= q[2]) {
		q[0] *= sign(R.ptr<double>()[3] - R.ptr<double>()[1]);
		q[1] *= sign(R.ptr<double>()[6] + R.ptr<double>()[2]);
		q[2] *= sign(R.ptr<double>()[7] + R.ptr<double>()[5]);
		q[3] *= +1.0f;
	} else {
		std::cout << "! AIDS IN QUATERNION :(((( Quaternions closed\n";
	}
	normalize();
}
*/


void Quaternion::normalize()
{
	q[0] /= norm();
	q[1] /= norm();
	q[2] /= norm();
	q[3] /= norm();
}


cv::Mat Quaternion::toR()
{
	cv::Mat rtrn(3,3,CV_64FC1);
	normalize();

	double * dest = &rtrn.ptr<double>()[0];
	double W = q[0];
	double X = q[1];
	double Y = q[2];
	double Z = q[3];

	dest[0] = 1.0 - 2.0*(Y*Y + Z*Z);
	dest[1] = 2.0*(X*Y - Z*W);
	dest[2] = 2.0*(X*Z + Y*W);

	dest[3] = 2.0*(X*Y + Z*W);
	dest[4] = 1.0 - 2.0*(X*X + Z*Z);
	dest[5] = 2.0*(Z*Y - X*W);

	dest[6] = 2.0*(X*Z - Y*W);
	dest[7] = 2.0*(Z*Y + X*W);
	dest[8] = 1.0f - 2.0*(X*X + Y*Y);

	return rtrn.clone();
}

/*

cv::Mat Quaternion::toR()
{
	cv::Mat rtrn(3,3,CV_64FC1);
	normalize();

	double * dest = &rtrn.ptr<double>()[0];
	double W = q[0];
	double X = q[1];
	double Y = q[2];
	double Z = q[3];

	dest[0] = 1.0f - 2.0f*Y*Y - 2.0f*Z*Z;
	dest[1] = 2.0f*X*Y + 2.0f*Z*W;
	dest[2] = 2.0f*X*Z - 2.0f*Y*W;

	dest[3] = 2.0f*X*Y - 2.0f*Z*W;
	dest[4] = 1.0f - 2.0f*X*X - 2.0f*Z*Z;
	dest[5] = 2.0f*Z*Y + 2.0f*X*W;

	dest[6] = 2.0f*X*Z + 2.0f*Y*W;
	dest[7] = 2.0f*Z*Y - 2.0f*X*W;
	dest[8] = 1.0f - 2.0f*X*X - 2.0f*Y*Y;

	return rtrn.clone();
}
*/

/*
cv::Mat Quaternion::toR()
{
	cv::Mat rtrn(3,3,CV_64FC1);
	normalize();
	rtrn.ptr<double>()[0] = 1 - 2*(q[1]*q[1] - q[2]*q[2]);
	rtrn.ptr<double>()[1] = 2*(q[0]*q[1] - q[3]*q[2]);
	rtrn.ptr<double>()[2] = 2*(q[0]*q[2] + q[3]*q[1]);
	rtrn.ptr<double>()[3] = 2*(q[0]*q[1] + q[3]*q[2]);
	rtrn.ptr<double>()[4] = 1 - 2*(q[0]*q[0] - q[2]*q[2]);
	rtrn.ptr<double>()[5] = 2*(q[1]*q[2] + q[3]*q[0]);
	rtrn.ptr<double>()[6] = 2*(q[0]*q[2] - q[3]*q[1]);
	rtrn.ptr<double>()[7] = 2*(q[1]*q[2] - q[3]*q[0]);
	rtrn.ptr<double>()[8] = 1 - 2*(q[0]*q[0] - q[1]*q[1]);
	
	/*
	rtrn.ptr<double>()[0] = q[0]*q[0] + q[1]*q[1] - q[2]*q[2] - q[3]*q[3];
	rtrn.ptr<double>()[1] = 2*(q[1]*q[2] - q[0]*q[3]);
	rtrn.ptr<double>()[2] = 2*(q[1]*q[3] + q[0]*q[2]);
	rtrn.ptr<double>()[3] = 2*(q[1]*q[2] + q[0]*q[3]);
	rtrn.ptr<double>()[4] = q[0]*q[0] - q[1]*q[1] + q[2]*q[2] - q[3]*q[3];
	rtrn.ptr<double>()[5] = 2*(q[2]*q[3] - q[0]*q[1]);
	rtrn.ptr<double>()[6] = 2*(q[1]*q[3] - q[0]*q[2]);
	rtrn.ptr<double>()[7] = 2*(q[2]*q[3] + q[0]*q[1]);
	rtrn.ptr<double>()[8] = q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3];*/
//	return rtrn.clone();
//}


double Quaternion::norm()
{
	return std::sqrt(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
}
	
}