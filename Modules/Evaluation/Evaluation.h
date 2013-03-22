#ifndef _EALUATION_H_
#define _EALUATION_H_

#include "../FrameList.h"
#include <vector>

class Evaluation
{
public:
	Evaluation(FrameList* frameList, int threshold = 2);

	void readXML2FrameList(char* fileName);
	void currentFrame();

	// GroundTruth
	vector<vector<Object>> groundTruth;
	// Reference to global framelist
	FrameList* frameList;
	vector<Object>* hypothesisList;

	
	void MOTP();
	void MOTA();

private:
	int frameCounter;
	int numberOfFrames, T, obID, hypID, obX, obY, hypX, hypY;
	vector<int> numberOfObjects, matches, misses, flasePositive, mismatches;
	vector<float> distance;
	vector<map<int, int>> correspondance;
	vector<int> occupiedHypothesis;
	Object *ob, *hyp;

	Object* getObj(vector<Object>* objVec, int ID);
	bool isCorr(int truID, int hypID);
};

#endif
