//#include "stdafx.h"

#include <math.h>
#include <time.h>
#include <opencv\cvaux.h>
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <opencv2\nonfree\features2d.hpp>
#include <libs/levmar/levmar.h> //<<<---- numera på sven.
#include <iostream>
#include "Modules/NonLinear.h"
#include "Modules\correspondanceExtractor.h"
#include "Modules\Camera.h"
#include "Modules\Estimate3D.h"
#include "Modules\Visualization\Visualizer.h"

#include <vector>
using namespace cv;
using namespace std;

#define mElapsedTime(t) ((float)(clock() - t))/CLOCKS_PER_SEC

NonLinear::NonLinear nonlin;
void loadFromFile(vis::Visualizer & v, std::string & filename, Estimate3D & dinosaurModel, CorrespondanceExtractor & corrEx, double scale);

enum ePROGRAM_STATE
{
	CALCULATE_CORRESPONDANCES,
	ESTIMATE3D,
	LOADFROMFILE,
	STAND_ALONE_VIEWER
} programState;


//Utility function used in preprocessing for the goldstandard stuff
void keyPoints2Points(vector<KeyPoint>& src, vector<Point>& dst )
{
	for(std::vector<KeyPoint>::iterator it = src.begin(); it != src.end(); ++it)
	{
		dst.push_back(it->pt);
	}
}

int main()
{
	// Variables
	//--------------
	int cameraAmount = 1;
	int version = 1;
	clock_t t_BA, t_mainLoop;
	std::string fileName;
	
	// Dinosaur K
	
	double Kdata[9] = {		3217.328669180762, -78.606641008226180, 289.8672403229193,
							0,					2292.424143977958,  -1070.516234777778,
							0,					0,					1};
	
	

	/*
	double Kdata[9] = { 666.2647,   -1.9125,    399.0122,
						0,			672.7446,	265.9638,
						0,          0,			1.0000};
	*/
	cv::Mat K = cv::Mat(3,3,CV_64FC1,Kdata);
	//cv::Mat K = cv::Mat::eye(3,3,CV_64FC1);
	
	NonLinear::NonLinear nonlin(K);
	Estimate3D dinosaurModel;
	CorrespondanceExtractor corrEx;
	vector<vector<pointPair>> matchesVector;

	// Select program state
	//programState = ePROGRAM_STATE::STAND_ALONE_VIEWER;
	//programState = ePROGRAM_STATE::CALCULATE_CORRESPONDANCES;
	programState = ePROGRAM_STATE::ESTIMATE3D;
	//programState = ePROGRAM_STATE::LOADFROMFILE;
	
	fileName = "iteration1.2.alx";

// The main program and it's 3 states
//-----------------------------------
	if(programState == ePROGRAM_STATE::CALCULATE_CORRESPONDANCES)
	{		
		corrEx.init("dinosaur");
		std::cout << "Correspondances are computed and saved to file!\n";
	}
	else
	if(programState == ePROGRAM_STATE::LOADFROMFILE)
	{
		std::cout << "# Loading file \"" << fileName << "\"..\n";
		dinosaurModel.loadFromFile(fileName);
		std::cout << "# File loaded!\n";
	}
	else
	if(programState == ePROGRAM_STATE::ESTIMATE3D)
	{
		// Load matches

		corrEx.loadMatches("dinosaur.alx");
		//corrEx.loadMatches("dinosaur.alx");

		vector<Point2d> imagePoints1;
		vector<Point2d> imagePoints2;
		corrEx.getBAPoints(0, imagePoints1, imagePoints2);

		// Pre Main loop
		//--------------------
			std::cout << "# Iteration " << 1 << " started..\n";	
			t_mainLoop = clock();
		dinosaurModel.init(imagePoints1, imagePoints2, K);
		dinosaurModel.saveToFile("iteration"+std::to_string(1)+".1.alx");
	
			std::cout << "# | Bundle adjustment started..\n";
			t_BA = clock();
		nonlin.BundleAdjust(dinosaurModel.cameras, &dinosaurModel.visible3DPoint);
			std::cout << "# | Bundle adjustment finished! \n";
			std::cout << "# |		Bundle adjustment time: " << mElapsedTime(t_BA) << " sec\n";
 		dinosaurModel.saveToFile("iteration"+std::to_string(1)+".2.alx");
				std::cout << "# Iteration " << 1 << " Finished!\n";
				std::cout << "# | Main loop time: " << mElapsedTime(t_mainLoop) << " sec\n";
				std::cout << "# -----------------------------\n";
	
		// Main loop
		//--------------------	
		for(int imageCounter = 1; imageCounter < cameraAmount; imageCounter++)
		{
				std::cout << "# Iteration " << imageCounter+1 << " started..\n";
				t_mainLoop = clock();
			imagePoints1.clear();
			imagePoints2.clear();
			corrEx.getBAPoints(imageCounter, imagePoints1, imagePoints2);
			dinosaurModel.addView(imagePoints1, imagePoints2);
			dinosaurModel.saveToFile("iteration"+std::to_string(imageCounter+1)+".1.alx");
				std::cout << "# | Bundle adjustment started..\n";
				t_BA = clock();
			nonlin.BundleAdjust(dinosaurModel.cameras, &dinosaurModel.visible3DPoint);
				std::cout << "# |	Bundle adjustment time: " << mElapsedTime(t_BA) << " sec\n";
			dinosaurModel.saveToFile("iteration"+std::to_string(imageCounter+1)+".2.alx");
		
				std::cout << "# Iteration " << imageCounter+1 << " Finished!\n";
				std::cout << "# | Main loop time: " << mElapsedTime(t_mainLoop) << " sec\n";
				std::cout << "# -----------------------------\n";
		}
	}


	if(1); else  // No more viewing here, use the Stand alone "3D reconstruction viewer"
	if(programState == LOADFROMFILE || programState == ESTIMATE3D)
	{
		double scale = 10.01;
		std::cout << "# Starting Visualizer...\n";
		vis::Visualizer v = vis::Visualizer();
		vector<Visible3DPoint> pvector = dinosaurModel.visible3DPoint;
		corrEx.readImages("data/dinosaur/im (", 37, ").ppm");
		vector<Mat> imageList = corrEx.imageList;

		for(std::list<Camera*>::iterator i = dinosaurModel.cameras.begin(); i != dinosaurModel.cameras.end(); i++)
			v.addCamera((*i)->C);

		for(vector<Visible3DPoint>::iterator it = pvector.begin(); it != pvector.end(); ++it){
			cv::Point3d* whatIsThePoint = it->point3D;
			int camer1ID = it->observerPair.front().camera1->id;
			Point2d imageCoordinate;
			imageCoordinate = it->observerPair.front().point2D.p1;

			Mat image = imageList.at(camer1ID);
			//Mat patch = image(Rect(imageCoordinate.x,imageCoordinate.y,2,2));
			Mat patch = image(Rect(315,240,2,2));

			cv::Vec3f coordinate = Vec3f(whatIsThePoint->x*scale, whatIsThePoint->y*scale, whatIsThePoint->z*scale);
			v.addPoint(coordinate,patch);
			//cout << *whatIsThePoint << endl;
		}
	
		Mat image = imageList.back();
		Mat patch = image(Rect(0,0,2,2));
		/*
		cout << "Patch: " << patch << endl;
		char colorR[3] = {1,0,0};
		char colorG[3] = {0,1,0};
		char colorB[3] = {0,0,1};
							
		cv::Mat red = cv::imread("red.jpg", CV_LOAD_IMAGE_ANYCOLOR);
		cv::Mat green = cv::imread("green.jpg", CV_LOAD_IMAGE_ANYCOLOR);
		cv::Mat blue = cv::imread("blue.jpg", CV_LOAD_IMAGE_ANYCOLOR);
							
		//cv::Mat c1 = cv::Mat(1,1, , colorR);
		cv::Mat c2 = cv::Mat(1,1, patch.type(), colorG);
		cv::Mat c3 = cv::Mat(1,1, patch.type(), colorB);

		//cout << "c1: " << c1 << endl;
		cout << "c2: " << c2 << endl;
		cout << "c3: " << c3 << endl;
		*/
		v.addPoint(cv::Vec3f(10,0,0), patch);
		v.addPoint(cv::Vec3f(0,10,0), patch);
		v.addPoint(cv::Vec3f(0,0,10), patch);
		v.addPoint(cv::Vec3f(5,0,0), patch);
		v.addPoint(cv::Vec3f(0,5,0), patch);
		v.addPoint(cv::Vec3f(0,0,5), patch);
		


		std::cout << "# Visualizer finished, lets see some balls!\n";
		v.mainLoop();

	}
	else
	if(programState == ePROGRAM_STATE::STAND_ALONE_VIEWER)
	{
		double scale = 1;
		int iteration = 1;
		int version = 2;
		std::string name;
		std::ifstream file; 
		std::cout << "# Starting...\n";
		corrEx.readImages("data/dinosaur/im (", 37, ").ppm");
		vis::Visualizer v = vis::Visualizer();
		vector<Visible3DPoint> pvector;

		std::cout << "\n# Welcome to the stand alone viewer 1.0\n################################\n\n";
		std::cout << "Enter filename (xxx#.#.alx, only the xxx are needed)> ";
		std::getline(cin, name);
		fileName = name + "1.2.alx";
		std::cout << "Enter starting scale> ";
		std::cin >> scale;
		std::cout << "# Loading file \"" << fileName << "\"..\n";
		dinosaurModel.loadFromFile(fileName);
		std::cout << "# File loaded!\n";
		std::cout << "# Starting Visualization mode...\n";
		loadFromFile(v,fileName,dinosaurModel,corrEx, scale);		

			// Main loop
			bool running = true;
			int dx, dy; 
			dx = dy = 0;
			bool lockMouse = false;
			bool mouseHasBeenPressed = false;

			while(running)
			{
				sf::Event event;

				while(v.displayWindow->pollEvent(event)){
					if(event.type == sf::Event::Closed){
						running = false;
					}else if(event.type == sf::Event::Resized){
						glViewport(0,0,event.size.width, event.size.height);
					}else if(event.type == sf::Event::MouseMoved){
				
					}else if(event.type == sf::Event::MouseButtonPressed) {
						if(sf::Mouse::isButtonPressed(sf::Mouse::Right) && !mouseHasBeenPressed)
						{
							mouseHasBeenPressed = true;
							if(version == 2)
							{
								version = 1;
								iteration++;
							}
							else
								version = 2;
							std::string fName = name + std::to_string(iteration) + "." + std::to_string(version) + ".alx";
							file.open(fName);
							if(!file)
							{
								std::cout << "\"" + fName + "\" do not exist!\n";
								if(version == 1)
								{
									version = 2;
									iteration--;
								}
								else
									version = 1;
							}
							else
							{
								file.close();
								fileName = fName;
								dinosaurModel.clear();
								std::cout << "# Loading file \"" << fileName << "\"..\n";
								dinosaurModel.loadFromFile(fileName);
								std::cout << "# File loaded!\n";
								loadFromFile(v,fileName,dinosaurModel,corrEx,scale);		
							}
							cout << "\n#  Current file: " << fileName;
							cout << "\n#  Current scale: " << scale << "\n ----------------- \n\n";
						}else
						if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mouseHasBeenPressed)
						{
							mouseHasBeenPressed = true;
							if(version == 1)
							{
								version = 2;
								iteration--;
							}
							else
								version = 1;
							std::string fName = name + std::to_string(iteration) + "." + std::to_string(version) + ".alx";
							file.open(fName);
							if(!file)
							{
								std::cout << "\"" + fName + "\" do not exist!\n";
								iteration = 1;
								version = 1;
							}
							else
							{
								file.close();
								fileName = fName;
								dinosaurModel.clear();
								std::cout << "# Loading file \"" << fileName << "\"..\n";
								dinosaurModel.loadFromFile(fileName);
								std::cout << "# File loaded!\n";
								loadFromFile(v,fileName,dinosaurModel,corrEx,scale);		
							}
							cout << "\n#  Current file: " << fileName;
							cout << "\n#  Current scale: " << scale << "\n ----------------- \n\n";
						}else
						if(sf::Mouse::isButtonPressed(sf::Mouse::XButton1) && !mouseHasBeenPressed)
						{
							mouseHasBeenPressed = true;
							scale *= 2;
							loadFromFile(v,fileName,dinosaurModel,corrEx,scale);	
							cout << "\n#  Current file: " << fileName;
							cout << "\n#  Current scale: " << scale << "\n ----------------- \n\n";
						}else
						if(sf::Mouse::isButtonPressed(sf::Mouse::XButton2) && !mouseHasBeenPressed)
						{
							mouseHasBeenPressed = true;
							scale /= 2;
							loadFromFile(v,fileName,dinosaurModel,corrEx,scale);	
							cout << "\n#  Current file: " << fileName;
							cout << "\n#  Current scale: " << scale << "\n ----------------- \n\n";
						}
					}else
						mouseHasBeenPressed = false;

					if(event.type == sf::Event::KeyPressed){
						if(event.key.code == sf::Keyboard::Escape)
							running = false;

						if(event.key.code == sf::Keyboard::F5)
						{
							dinosaurModel.clear();
							std::cout << "# Loading file \"" << fileName << "\"..\n";
							dinosaurModel.loadFromFile(fileName);
							std::cout << "# File loaded!\n";
							loadFromFile(v,fileName,dinosaurModel,corrEx, scale);	
							cout << "\n#  Current file: " << fileName;
							cout << "\n#  Current scale: " << scale << "\n ----------------- \n\n";
						}

						// Change iterations
						if(event.key.code == sf::Keyboard::Right)
						{
							if(version == 2)
							{
								version = 1;
								iteration++;
							}
							else
								version = 2;
							std::string fName = name + std::to_string(iteration) + "." + std::to_string(version) + ".alx";
							file.open(fName);
							if(!file)
							{
								std::cout << "\"" + fName + "\" do not exist!\n";
								if(version == 1)
								{
									version = 2;
									iteration--;
								}
								else
									version = 1;
							}
							else
							{
								file.close();
								fileName = fName;
								dinosaurModel.clear();
								std::cout << "# Loading file \"" << fileName << "\"..\n";
								dinosaurModel.loadFromFile(fileName);
								std::cout << "# File loaded!\n";
								loadFromFile(v,fileName,dinosaurModel,corrEx,scale);		
							}
							cout << "\n#  Current file: " << fileName;
							cout << "\n#  Current scale: " << scale << "\n ----------------- \n\n";
						}
						if(event.key.code == sf::Keyboard::Left)
						{
							if(version == 1)
							{
								version = 2;
								iteration--;
							}
							else
								version = 1;
							std::string fName = name + std::to_string(iteration) + "." + std::to_string(version) + ".alx";
							file.open(fName);
							if(!file)
							{
								std::cout << "\"" + fName + "\" do not exist!\n";
								iteration = 1;
								version = 1;
							}
							else
							{
								file.close();
								fileName = fName;
								dinosaurModel.clear();
								std::cout << "# Loading file \"" << fileName << "\"..\n";
								dinosaurModel.loadFromFile(fileName);
								std::cout << "# File loaded!\n";
								loadFromFile(v,fileName,dinosaurModel,corrEx,scale);		
							}
							cout << "\n#  Current file: " << fileName;
							cout << "\n#  Current scale: " << scale << "\n ----------------- \n\n";
						}

						// Change scale
						if(event.key.code == sf::Keyboard::Add)
						{
							scale *= 2;
							loadFromFile(v,fileName,dinosaurModel,corrEx,scale);	
							cout << "\n#  Current file: " << fileName;
							cout << "\n#  Current scale: " << scale << "\n ----------------- \n\n";
						}
						if(event.key.code == sf::Keyboard::Subtract)
						{
							scale /= 2;
							loadFromFile(v,fileName,dinosaurModel,corrEx,scale);	
							cout << "\n#  Current file: " << fileName;
							cout << "\n#  Current scale: " << scale << "\n ----------------- \n\n";
						}

					}else if(event.type == sf::Event::GainedFocus)
						lockMouse = true;
					else if(event.type == sf::Event::LostFocus)
						lockMouse = false;
				}
				if(lockMouse)
				{
					dx = sf::Mouse::getPosition(*v.displayWindow).x - v.displayWindow->getSize().x / 2;
					dy = sf::Mouse::getPosition(*v.displayWindow).y - v.displayWindow->getSize().y / 2;
					dy = -dy;
					sf::Mouse::setPosition(sf::Vector2i(v.displayWindow->getSize().x / 2, v.displayWindow->getSize().y / 2),  *v.displayWindow);
					v.cam.updatePosition(dx, dy);
				}
				dx = dy = 0;
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				v.redraw();

				// Draw SFML 2D-stuff
				//v.displayWindow->pushGLStates();
				//v.displayWindow->resetGLStates();
				//v.displayWindow->popGLStates();

				// Restore state
				v.displayWindow->display();
			}
	}

	waitKey(0);
	return 0;
}


void loadFromFile(vis::Visualizer & v, std::string & filename, Estimate3D & dinosaurModel, CorrespondanceExtractor & corrEx, double scale)
{
	v.plottedPoints.clear();

	for(std::list<Camera*>::iterator i = dinosaurModel.cameras.begin(); i != dinosaurModel.cameras.end(); i++)
		v.addCamera((*i)->C);

	for(vector<Visible3DPoint>::iterator it = dinosaurModel.visible3DPoint.begin(); it != dinosaurModel.visible3DPoint.end(); ++it){
		cv::Point3d* whatIsThePoint = it->point3D;
		int camer1ID = it->observerPair.front().camera1->id;
		Point2d imageCoordinate;
		imageCoordinate = it->observerPair.front().point2D.p1;

		Mat image = corrEx.imageList.at(camer1ID);
		Mat patch = image(Rect(315,240,2,2));

		cv::Vec3f coordinate = Vec3f(whatIsThePoint->x*scale, whatIsThePoint->y*scale, whatIsThePoint->z*scale);
		v.addPoint(coordinate,patch);
	}
	
	Mat image = corrEx.imageList.back();
	Mat patch = image(Rect(0,0,2,2));
	v.addPoint(cv::Vec3f(10,0,0), patch);
	v.addPoint(cv::Vec3f(0,10,0), patch);
	v.addPoint(cv::Vec3f(0,0,10), patch);
	v.addPoint(cv::Vec3f(5,0,0), patch);
	v.addPoint(cv::Vec3f(0,5,0), patch);
	v.addPoint(cv::Vec3f(0,0,5), patch);		
	std::cout << "# Visualizer finished loading, lets see some balls!\n";
}




