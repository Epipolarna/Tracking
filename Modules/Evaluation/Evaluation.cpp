#include "Evaluation.h"
#include "rapidxml.hpp"

#include <iostream>
#include <fstream>
#include <numeric>
#include <functional>
#define car first
#define cdr second

using namespace std;
using namespace rapidxml;

Evaluation::Evaluation(FrameList* _frameList, int threshold)
{
	T = threshold;
	frameCounter = 0;
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
		numberOfObjects.push_back( i->size() );
	}
	
	cout << "Done!" << endl;
}

void Evaluation::currentFrame()
{
	// in case there is more movie than groundtruth
	if (frameCounter < numberOfFrames)
	{
		// Init variables
		frameDistance = 0;
		frameMismatches = 0;
		// Insert empty map for current frame
		map<int,int> tempMap;
		correspondance.push_back(tempMap);
	
		// The first frame desn't have any previous frame
		if (frameCounter > 0)
		{
			//cout << "Alg part 1.1" << endl;
			hypothesisList = frameList->getLatestFrame().objects;
			// Check if old correspondances, in previous frame, are still valid
				// Use dist < T

			//cout << "Alg part 1.2" << endl;
			for (map<int,int>::iterator i = correspondance.at(frameCounter - 1).begin(); i != correspondance.at(frameCounter - 1).end(); i)
			{
				//cout << "Alg part 1.3.1" << endl;
				// Get Object position from GroundTruthFrameList
				// correspondance.at(frameCounter)[i->first] // Returns Object ID

				// Object id
				//cout << "Alg part 1.3.2" << endl;
				obID = i->first;
				// Hypothesis id
				//cout << "Alg part 1.3.3" << endl;
				hypID = i->second;

				//isCorr also adds the distance between frames to the 
				//total distance, and removes found correspondences from both vectors
				if( isCorr(obID, hypID) )
				{
					//cout << "Alg part 1.4.1" << endl;
					correspondance.at(frameCounter).insert(pair<int,int>(obID, hypID));
					// Remove correctly classified objects from the list and previous frame map.
					//cout << "Alg part 1.4.2" << endl;
					deleteObj(&groundTruth.at(frameCounter), obID);
					//cout << "Alg part 1.4.3" << endl;
					deleteObj(&hypothesisList, hypID);
					// remove matched correspondances from the previous map.
					//cout << "Alg part 1.4.4" << endl;

					map<int,int>::iterator iTemp = i;
					i++;
					correspondance.at(frameCounter - 1).erase(iTemp);
				}
				else
				{
					//cout << "Alg part 1.4.5" << endl;
					i++;
				}
			}
		}
	
		// Objects without correspondance 
		// Find matching hypothesis, allowing only 1-1 match
		//cout << "Alg part 2.1" << endl;
		multimap<double, pair<int, int>> distMap;
		//cout << "Alg part 2.2" << endl;
		// Create map with the distance from all objects to all hypothesis
		for( vector<Object>::iterator truObj = groundTruth.at(frameCounter).begin(); truObj != groundTruth.at(frameCounter).end(); truObj++)
		{
			//cout << "Alg part 2.3" << endl;
		
			for( vector<Object>::iterator hypObj = hypothesisList.begin(); hypObj != hypothesisList.end(); hypObj++)
			{
				//cout << "Alg part 2.3" << endl;
				double distance = sqrt((truObj->x - hypObj->x)^2 + (truObj->y - hypObj->y)^2); 
					if (distance < T)
						distMap.emplace(distance, make_pair(truObj->id, hypObj->id));
			}
		}
	
		// Go through the map to find the shortest distance-pairs
		// while only allowing 1-1 matching
		while (!distMap.empty())
		{
			//cout << "Alg part 3.1" << endl;
			obID = distMap.begin()->second.first;
			hypID = distMap.begin()->second.second;

			//cout << "Alg part 3.2" << endl;
			if (correspondance.at(frameCounter - 1)[obID] != hypID)
			{
				//cout << "Alg part 3.3" << endl;
				frameMismatches += 1;
			}

			//cout << "Alg part 3.4.1" << endl;
			correspondance.at(frameCounter).insert(pair<int,int>(obID, hypID));
			//cout << "Alg part 3.4.2" << endl;
			deleteObj(&groundTruth.at(frameCounter), obID);
			//cout << "Alg part 3.4.3" << endl;
			deleteObj(&hypothesisList, hypID);
			//cout << "Alg part 3.4.4" << endl;
			frameDistance += (float)distMap.begin()->first;
			//cout << "Alg part 3.4.5" << endl;
			for ( multimap<double, pair<int, int>>::iterator it = distMap.begin(); it != distMap.end(); it)
			{
				//cout << "Alg part 3.5" << endl;
				if ( it->second.first == obID)
				{
					//cout << "Alg part 3.6" << endl;
					// It is not allowed to remove the object that the iterator is pointing at
					multimap<double, pair<int, int>>::iterator itTemp = it;
					it++;
					distMap.erase(itTemp);
				}
				else
				{
					it++;
				}
			}
			//cout << "Alg part 3.7" << endl;
			mismatches.push_back(frameMismatches);
		}
	
		//cout << "Alg part 4" << endl;
		// Push frame variables to corresponding vector
		matches.push_back(correspondance.at(frameCounter).size());
		misses.push_back(groundTruth.at(frameCounter).size());
		falsePositive.push_back(hypothesisList.size());
		mismatches.push_back(frameMismatches);
		distance.push_back(frameDistance);

		cout << "FrameCounter:\t" << frameCounter << endl << endl;;

		cout << "Matches:\t" << matches.back() << endl;
		cout << "Misses:\t\t" << misses.back() << endl;
		cout << "False Positive:\t" << falsePositive.back() << endl;
		cout << "Mismatches:\t" << mismatches.back() << endl;
		cout << "Distance:\t" << distance.back()	<< endl;

		frameCounter++;

		// Calculate MOTP & MOTA
		MOTP();
		MOTA();
	}
	else
	{
		cout << "FrameCounter:\t" << frameCounter << endl;
		frameCounter++;
	}
}

void Evaluation::MOTP()
{
	// Sum distance
	float sumDistance = (float)accumulate(distance.begin(), distance.end(), 0);
	// Sum number of matches
	float sumMatches = (float)accumulate(matches.begin(), matches.end(), 0);
	// Calculate quote
	motpValue = sumDistance/sumMatches;
	cout << "MOTP:\t\t" << motpValue << endl;
}

void Evaluation::MOTA()
{
	// Sum misses
	float sumMisses = (float)accumulate(misses.begin(), misses.end(), 0);
	// Sum false positive
	float sumFalsePositive = (float)accumulate(falsePositive.begin(), falsePositive.end(), 0);
	// Sum mismatches
	float sumMismatches = (float)accumulate(mismatches.begin(), mismatches.end(), 0);
	// Sum total number of objets
	float sumNumberOfObjects = (float)accumulate(numberOfObjects.begin(), numberOfObjects.end(), 0);
	// Calculate quote
	motaValue = 1 - (sumMisses + sumFalsePositive + sumMismatches)/sumNumberOfObjects;
	cout << "MOTA:\t\t" << motaValue << endl;
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
	for (vector<Object>::iterator it = objVec->begin(); it != objVec->end(); it)
	{
		if (it->id == ID)
		{
			vector<Object>::iterator itTemp = it;
			it++;
			objVec->erase(itTemp);
			return;
		}
		else
		{
			it++;
		}
	}
	cerr << "object not found" << endl;
}

bool Evaluation::isCorr(int truID, int hypID)
{
	// Check if a object-hypothesis pair is within threshold limit
	ob = getObj(&groundTruth.at(frameCounter), truID);
	if (!ob)
		return false;
 
	hyp = getObj(&hypothesisList, hypID);
	if (!hyp)
		return false;
 
	obX = ob->x;
	obY = ob->y;
	hypX = hyp->x;
	hypY = hyp->y;
	float dist = (float)sqrt((obX - hypX)^2 + (obY - hypY)^2);
 
	if (dist < T)
	{
		frameDistance += dist;
		return true;
	} 
	return false;   
}