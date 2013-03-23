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
	void MOTP();
	void MOTA();

	// GroundTruth
	vector<vector<Object>> groundTruth;
	// Reference to global framelist
	FrameList* frameList;
	vector<Object> hypothesisList;

private:
	int frameCounter, numberOfFrames, frameMismatches;
	int T, obID, hypID, obX, obY, hypX, hypY;
	float frameDistance, motpValue, motaValue;
	float sumDistance, sumMisses, sumMatches, sumFalsePositive, sumMismatches, sumNumberOfObjects;
	vector<int> numberOfObjects, matches, misses, falsePositive, mismatches;
	vector<float> distance;
	vector<map<int, int>> correspondance;
	Object *ob, *hyp;

	Object* getObj(vector<Object>* objVec, int ID);
	void deleteObj(vector<Object>* objVec, int ID);
	bool isCorr(int truID, int hypID);
};

#endif
