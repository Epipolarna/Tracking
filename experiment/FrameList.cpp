#include "FrameList.h"

FrameList::FrameList(int framesToKeep){
	maxFrames = framesToKeep;
}

void FrameList::appendFrame(IplImage *frameImage){

	Mat m = Mat(frameImage);
	Frame f = Frame(m.clone());


	if(getNumFrames() >= maxFrames){
		oldFrames.pop_front();
	}
	oldFrames.push_back(f);
}

int FrameList::getMaxFrames(){
	return maxFrames;
}

int FrameList::getNumFrames(){
	return oldFrames.size();
}

Frame FrameList::getLast(){
	return oldFrames.back();
}

vector<Frame> FrameList::toVector(){
	return vector<Frame>(oldFrames.begin(),oldFrames.begin());
}