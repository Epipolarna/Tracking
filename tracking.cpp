#include "Modules/FrameList.h"
#include "Modules/BackgroundModelling/BackgroundModel.h"
#include "Modules/BackgroundModelling_simple/BackgroundModel_simple.h"
#include "Modules/ForegroundProcessing/ForegroundProcessor.h"
#include "Modules/ObjectIdentification/Identification.h"
#include "Modules/Prediction/Kalman.h"
#include "Modules/Evaluation/Evaluation.h"

#include "Modules/Profiler.h"

#define PROFILER_INIT() ProfilerClock c;
#define PROFILER_RESET() c.reset();
#define PROFILE(string) frameList.setTime(string, c.getTime()); c.lap();
#define PROFILE_TOTALTIME() frameList.setTime("Total Time", c.getTotalTime());
#define PROFILE_TOTALFPS() frameList.setTime("Total FPS", c.getTotalFPS());

int main()
{
	// Profiler init
	PROFILER_INIT();

	// Frame container
	FrameList frameList(10);	// Keep a history of up to 100 frames (might be used by some modules)
	
	// Modules
	BackgroundModelling_simple::BackgroundModel backgroundModel;
	ForegroundProcessing::ForegroundProcessor foregroundProcessor;
	Identification::Identifier identifier;
	Prediction::Kalman kalmanFilter;
	Evaluation Evaluate(&frameList);

	// Init
	foregroundProcessor.setAlgortihm(ForegroundProcessing::AREA);
	foregroundProcessor.init(50, 3, 5, 50, 3);
	identifier.init(Identification::Experimental);
	Evaluate.readXML2FrameList("clip1.xml");
	
	
	// Load frame source
	frameList.open("camera1.mov");
	
	// Create windows
	namedWindow("Info",CV_WINDOW_AUTOSIZE);
	namedWindow("Background",CV_WINDOW_AUTOSIZE);
	namedWindow("Foreground",CV_WINDOW_AUTOSIZE);
	namedWindow("Tracking",CV_WINDOW_AUTOSIZE);
	namedWindow("BackgroundModel",CV_WINDOW_AUTOSIZE);

	
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
		frameList.displayForeground("Foreground");
		frameList.displayProbabilityMap("Background");
		backgroundModel.display("BackgroundModel");
				
		// Give the GUI time to render
		waitKey(1);															PROFILE("Display");

		// Optional pause between each frame
		// waitKey(0);
						
		// Read next frame from source
		frameList.queryNextFrame();											PROFILE("QueryNextFrame");										
																			PROFILE_TOTALTIME();
																			PROFILE("Total FPS");
																			PROFILE_TOTALFPS();
				
		// Display info
		frameList.displayInfo("Info");
	}
	
	waitKey(0);

	return 0;
}