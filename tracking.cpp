#include "Modules/FrameList.h"
#include "Modules/Background modelling/BackgroundModel.h"
#include "Modules/Foreground processing/ForegroundProcessor.h"
#include "Modules/Object identification/Identification.h"
#include "Modules/Prediction/KalmanFilter.h"

#include <time.h>

int main()
{
	// Frame container
	FrameList frameList(100);	// Keep a history of up to 100 frames (might be used by some modules)
	
	// Modules
	BackgroundModelling::BackgroundModel backgroundModel;
	ForegroundProcessing::ForegroundProcessor foregroundProcessor;
	Identification::Identifier identifier;
	Prediction::KalmanFilter kalmanFilter;

	// Init
	foregroundProcessor.init(ForegroundProcessing::FAST, 50, 3);
	
	// Load frame source
	frameList.open("camera1.mov");
	
	// Create windows
	namedWindow("Background",CV_WINDOW_AUTOSIZE);
	namedWindow("Foreground",CV_WINDOW_AUTOSIZE);
	namedWindow("Tracking",CV_WINDOW_AUTOSIZE);
	
	//
	int time, totalTime;

	// Track objects through all frames
	while(!frameList.isSourceEmpty())
	{
		std::cout << "------------\nProfiling:\n";
		totalTime = time = clock();
		// Do the nessecary processing
		backgroundModel.update(frameList.getFrames());
		std::cout << "update:       " << (double(clock()-time)/CLOCKS_PER_SEC) << "\n"; time = clock();
		foregroundProcessor.segmentForeground(frameList.getLatestFrame());
		std::cout << "segmentation: " << (double(clock()-time)/CLOCKS_PER_SEC) << "\n"; time = clock();
		identifier.identify(frameList.getFrames());
		std::cout << "identify:     " << (double(clock()-time)/CLOCKS_PER_SEC) << "\n"; time = clock();
		kalmanFilter.predict(frameList.getLatestFrame());
		std::cout << "predict:      " << (double(clock()-time)/CLOCKS_PER_SEC) << "\n"; time = clock();
		
		// Display result
		frameList.display("Tracking");
		frameList.displayBackground("Background");
		frameList.displayForeground("Foreground");
		std::cout << "display:      " << (double(clock()-time)/CLOCKS_PER_SEC) << "\n"; time = clock();

		
		waitKey(1);

		// Optional pause between each frame
		//waitKey(0);
		// Else let it try running in real time
		/*
		if(double(clock()-time)/CLOCKS_PER_SEC > 1000/double(frameList.getFrameRate()))
			waitKey(1);
		else
			waitKey(1000/double(frameList.getFrameRate())-double(clock()-time)/CLOCKS_PER_SEC);
		*/

		// Read next frame from source
		frameList.queryNextFrame();

		std::cout << "totalTime:    " << (double(clock()-totalTime)/CLOCKS_PER_SEC) << "\n";
	}
	
	waitKey(0);

	return 0;
}