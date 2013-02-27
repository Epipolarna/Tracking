#include "FrameList.h"

int FrameList::foo(){
	return 10;
}

void FrameList::appendFrame(IplImage *frameImage){

	Mat m = Mat(frameImage);
	Frame f = Frame(m);
	
	framesList.push_back(f);

}


int FrameList::getNumFrames(){
	return framesList.size();
}
