#include "Evaluation.h"
#include "rapidxml.hpp"

#include <iostream>
#include <fstream>
#define car first
#define cdr second

using namespace std;
using namespace rapidxml;

Evaluation::Evaluation(FrameList* _frameList, int threshold)
{
	T = threshold;
	frameCounter = 0;
	fill(distance.begin(), distance.end(), 0.0f);
	frameList = _frameList;
}


void Evaluation::readXML2FrameList(char* fileName)
{
	cout << "Reading ground truth from " << fileName << endl;
	ifstream myfile(fileName);

	if(!myfile.is_open())
	{
		cout << "Could not open xml file!\n";
		return;
	}
	xml_document<> doc;

	vector<char> buffer((istreambuf_iterator<char>(myfile)), istreambuf_iterator<char>( ));
	buffer.push_back('\0');
	doc.parse<0> (&buffer[0]);

	xml_node<>* framePointer;
	xml_node<>* objectListPointer;
	xml_node<>* objectPointer;
	xml_node<>* objectPropertyPointer;

	framePointer = doc.first_node()->first_node();
	
	// Extract numberof frames
	char* frameAttribute = doc.first_node()->last_node()->first_attribute()->value();
	numberOfFrames = atoi(frameAttribute);

	vector<vector<Object>> groundTruth;
	
	int frameNumber, objectID;
	int x, y, h, w;

	// Loop untill no frames are left
	while(framePointer != 0)
	{
		frameNumber = atoi(framePointer->first_attribute()->value());
		groundTruth.push_back(vector<Object>());

		objectListPointer = framePointer->first_node();
		objectPointer = objectListPointer->first_node();

		// Loop untill no objects are left
		while(objectPointer != 0)
		{
			// Extract information about objects
			objectID = atoi(objectPointer->first_attribute()->value());
			groundTruth.back().push_back(Object());

			objectPropertyPointer = objectPointer->first_node("box");
			x = atoi(objectPropertyPointer->first_attribute("xc")->value());
			y = atoi(objectPropertyPointer->first_attribute("yc")->value());
			h = atoi(objectPropertyPointer->first_attribute("h")->value());
			w = atoi(objectPropertyPointer->first_attribute("w")->value());

			groundTruth.back().back().x = x;
			groundTruth.back().back().y = y;
			groundTruth.back().back().height = h;
			groundTruth.back().back().width = w;
			groundTruth.back().back().id = objectID;
			
			objectPointer = objectPointer->next_sibling();
		}

		framePointer = framePointer->next_sibling();
	}

	for (vector<vector<Object>>::iterator i = groundTruth.begin(); i != groundTruth.end(); i++)
	{
		numberOfObjects += i->size();
	}

	cout << "Done!" << endl;
}

void Evaluation::currentFrame()
{
	// Init variables
	frameDistance = 0;
	frameMismatches = 0;

	// The first frame desn't have any previous frame
	if (frameCounter > 0)
	{
		hypothesisList = frameList->getLatestFrame().objects;
		// Check if old correspondances, in previous frame, are still valid
			// Use dist < T


		for (map<int,int>::iterator i = correspondance.at(frameCounter - 1).begin(); i != correspondance.at(frameCounter - 1).end(); i++)
		{
			// Get Object position from GroundTruthFrameList
			// correspondance.at(frameCounter)[i->first] // Returns Object ID

			// Object id
			obID = i->first;
			// Hypothesis id
			hypID = i->second;

			//isCorr also adds the distance between frames to the 
			//total distance, and removes found correspondences from both vectors
			if( isCorr(obID, hypID) )
			{
				correspondance.at(frameCounter).insert(pair<int,int>(obID, hypID));
				//occupiedHypothesis.push_back(hypID);
				// Remove correctly classified objects from the list and previous frame map.
				deleteObj(&groundTruth.at(frameCounter), obID);
				deleteObj(&hypothesisList, hypID);
				// remove matched correspondances from the previous map.
				correspondance.at(frameCounter - 1).erase(i);
			}
		}
	}
	
	// Objects without correspondance 
	// Find matching hypothesis, allowing only 1-1 match
	multimap<double, pair<int, int>> distMap;
	for( vector<Object>::iterator truObj = groundTruth.at(frameCounter).begin(); truObj != groundTruth.at(frameCounter).end(); truObj++)
	{
		for( vector<Object>::iterator hypObj = hypothesisList.begin(); hypObj != hypothesisList.end(); hypObj++)
		{
			double distance = sqrt((truObj->x - hypObj->x)^2 + (truObj->y - hypObj->y)^2); 
				if (distance < T)
					distMap.emplace(distance, make_pair(truObj->id, hypObj->id));
		}
	}

	while (!distMap.empty())
	{
		obID = distMap.begin()->second.first;
		hypID = distMap.begin()->second.second;

		if (correspondance.at(frameCounter - 1)[obID] != hypID)
		{
			frameMismatches += 1;
		}

		correspondance.at(frameCounter).insert(pair<int,int>(obID, hypID));
		deleteObj(&groundTruth.at(frameCounter), obID);
		deleteObj(&hypothesisList, hypID);
		distance.at(frameCounter) += (float)distMap.begin()->first;
		for ( multimap<double, pair<int, int>>::iterator it = distMap.begin(); it != distMap.end(); it++)
		{
			if ( it->second.first == obID)
				distMap.erase(it);
		}

		mismatches.push_back(frameMismatches);
	}

	// Calculate the last variables
	matches.push_back(correspondance.at(frameCounter).size());

	misses.push_back(groundTruth.at(frameCounter).size());

	flasePositive.push_back(hypothesisList.size());

	distance.push_back(frameDistance);
}

Object* Evaluation::getObj(vector<Object>* objVec, int ID)
{
	for (vector<Object>::iterator it = objVec->begin(); it != objVec->end(); it++)
	{ 
		if(it->id == ID)
			return &(*it);
	}
	return NULL;
}

void Evaluation::deleteObj(vector<Object>* objVec, int ID)
{
	for (vector<Object>::iterator it = objVec->begin(); it != objVec->end(); it++)
	{
		if (it->id == ID)
		{
			objVec->erase(it);
			return;
		}
	}
	cerr << "object not found" << endl;
}

bool Evaluation::isCorr(int truID, int hypID)
{
	Object* truObj; 
	Object* hypObj;

	truObj = getObj(&groundTruth.at(frameCounter), truID);
	if (!truObj)
		return false;
 
	hypObj = getObj(&hypothesisList, hypID);
	if (!hypObj)
		return false;
 
	obX = truObj->x;
	obY = truObj->y;
	hypX = hypObj->x;
	hypY = hypObj->y;
	float dist = (float)sqrt((obX - hypX)^2 + (obY - hypY)^2);
 
	if (dist < T)
	{
		frameDistance += dist;
		return true;
	} 
	
	return false;   
}