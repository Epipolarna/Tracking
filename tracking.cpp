#include "Modules/FrameList.h"
#include "Modules/Background modelling/BackgroundModel.h"
#include "Modules/Foreground processing/ForegroundProcessor.h"
#include "Modules/Object identification/Identification.h"
#include "Modules/Prediction/KalmanFilter.h"

int main()
{
	// Frame container
	FrameList frameList(100);	// Keep a history of up to 100 frames (might be used by some modules)
	
	// Modules
	BackgroundModelling::BackgroundModel backgroundModel;
	ForegroundProcessing::ForegroundProcessor foregroundProcessor;
	Identification::Identifier identifier;
	Prediction::KalmanFilter kalmanFilter;
	
	// Load frame source
	frameList.open("data/camera1.mov");
	
	// Create windows
	namedWindow("Background",CV_WINDOW_AUTOSIZE);
	namedWindow("Foreground",CV_WINDOW_AUTOSIZE);
	namedWindow("Tracking",CV_WINDOW_AUTOSIZE);
	
	// Track objects through all frames
	while(!frameList.isSourceEmpty())
	{
		// Do the nessecary processing
		backgroundModel.update(frameList.getFrames());
		foregroundProcessor.segmentForeground(frameList.getLatestFrame());
		identifier.identify(frameList.getFrames());
		kalmanFilter.predict(frameList.getLatestFrame());
		
		// Display result
		frameList.display("Tracking");
		frameList.displayBackground("Background");
		frameList.displayForeground("Foreground");
		
		// Optional pause between each frame
		// waitKey(0);
		
		// Read next frame from source
		frameList.queryNextFrame();
	}
	
	waitKey(0);

	return 0;
}