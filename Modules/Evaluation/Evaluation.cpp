#include "Evaluation.h"
#include "rapidxml.hpp"

#include <iostream>
#include <fstream>

using namespace std;
using namespace rapidxml;

Evaluation::Evaluation(int threshold)
{
	T = threshold;
	frameCounter = 0;
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

			for (vector<Object>::iterator j = groundTruth.at(frameCounter).begin(); j != groundTruth.at(frameCounter).end(); j++)
			{
				// Find Object id on current frame
				if (j->id == obID)
				{
					obX = j->x;
					obY = j->y;

					// Find hypothesis 
					hypID = correspondance.at(frameCounter)[obID];


				}
				
			}

			
		}
	}
	
	// Objects without correspondance 
		// Find matching hypothesis, allowing only 1-1 match

		// Minimize total distance(object, hypothesis), Munker's Alg

	// Check if objects has changed hypothesis
		// correspondance(Object, hypothesis) at t - 1 != correspondance(Object, hypothesis) at t
		// Replace old correspondance with new and add an error to mismatches

	frameCounter++;
}