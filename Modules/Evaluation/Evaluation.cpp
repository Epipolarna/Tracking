#include "Evaluation.h"
#include "rapidxml.hpp"

#include <iostream>
#include <fstream>

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

	list<vector<Object>> groundTruth;
	
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

	cout << "Done!" << endl;
}

void Evaluation::currentFrame()
{
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
				// Remove correctly classified objects from the list.
				deleteObj(&groundTruth.at(frameCounter), obID);
				deleteObj(&hypothesisList, hypID);
			}
		}
		
	}
	
	// Objects without correspondance 
		// Find matching hypothesis, allowing only 1-1 match
	for (map<int,int>::iterator i = correspondance.at(frameCounter - 1).begin(); i != correspondance.at(frameCounter - 1).end(); i++)
	{
		obID = i->first;
		if ( correspondance.at(frameCounter).find(obID) == correspondance.at(frameCounter).end() )
		{
			// If you end up here the object has no correspondance.
			// Look for best candidate with dist < T
			for (vector<Object>::iterator j = hypothesisList.begin(); j != hypothesisList.end(); j++)
			{
				hypID = j->id;
				for (vector<int>::iterator k = occupiedHypothesis.begin(); k != occupiedHypothesis.end(); k++)
				{
					if ( hypID != *k )
					{
						hypX = j->x;
						hypY = j->y;
						
						ob = getObj(&groundTruth.at(frameCounter), obID);
						if ( ob )
						{

						}
					}
				}
			}
		}
	}
		// Minimize total distance(object, hypothesis), Munker's Alg

	// Check if objects has changed hypothesis
		// correspondance(Object, hypothesis) at t - 1 != correspondance(Object, hypothesis) at t
		// Replace old correspondance with new and add an error to mismatches

	frameCounter++;
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
 
	int truX = truObj->x;
	int truY = truObj->y;
	int hypX = hypObj->x;
	int hypY = hypObj->y;
	float dist = sqrt((truX - hypX)^2 + (truY - hypY)^2);
 
	if (dist < T)
	{
		distance.at(frameCounter) += dist;
		return true;
	} 
	
	return false;   
}