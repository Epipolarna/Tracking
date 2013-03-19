#include "Modules/FrameList.h"
#include "Modules/Background modelling/BackgroundModel.h"
#include "Modules/Foreground processing/ForegroundProcessor.h"
#include "Modules/Object identification/Identification.h"
#include "Modules/Prediction/KalmanFilter.h"

#include "Modules/Profiler.h"

#define PROFILER_INIT() ProfilerClock c;
#define PROFILER_RESET() c.reset();
#define PROFILE(string) frameList.setTime(string, c.getTime()); c.lap();
#define PROFILE_TOTALTIME() frameList.setTime("Total Time", c.getTotalTime());

int main()
{
	// Profiler init
	PROFILER_INIT();

	// Frame container
	FrameList frameList(10000);	// Keep a history of up to 100 frames (might be used by some modules)
	
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
	namedWindow("Info",CV_WINDOW_AUTOSIZE);
	namedWindow("Background",CV_WINDOW_AUTOSIZE);
	namedWindow("Foreground",CV_WINDOW_AUTOSIZE);
	namedWindow("Tracking",CV_WINDOW_AUTOSIZE);
	
	// Track objects through all frames
	while(!frameList.isSourceEmpty())
	{
		// Reset profiler
		PROFILER_RESET();

		// Do the nessecary processing
		backgroundModel.update(frameList.getFrames());						PROFILE("BackgroundModel");
		foregroundProcessor.segmentForeground(frameList.getLatestFrame());	PROFILE("ForegroundSeg.");
		identifier.identify(frameList.getFrames());							PROFILE("Identification");	
		kalmanFilter.predict(frameList.getLatestFrame());					PROFILE("Kalman Prediction");		

		
		// Display result
		frameList.display("Tracking");
		frameList.displayBackground("Background");
		frameList.displayForeground("Foreground");							PROFILE("Display");
				
		// Give the GUI time to render
		waitKey(1);

		// Optional pause between each frame
		//waitKey(0);
						
		// Display info
		frameList.displayInfo("Info");

		// Read next frame from source
		frameList.queryNextFrame();											PROFILE_TOTALTIME();

	}
	
	waitKey(0);

	return 0;
}