#ifndef _EALUATION_H_
#define _EALUATION_H_

#include "../FrameList.h"
#include <vector>

class Evaluation
{
public:
	Evaluation(int threshold = 2);

	void readXML2FrameList(char* fileName);
	void currentFrame();

	// GroundTruth
	vector<vector<Object>> groundTruth;
	void MOTP();
	void MOTA();

private:
	int frameCounter;
	int numberOfFrames, T, obID, hypID, obX, obY, hypX, hypY;
	vector<int> numberOfObjects, matches, misses, flasePositive, mismatches;
	vector<vector<float>> distance;
	vector<map<int, int>> correspondance;

};

#endif
