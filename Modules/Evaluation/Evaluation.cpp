#include "Evaluation.h"
#include <iostream>
#include <vector>
#include <fstream>

#include "rapidxml.hpp"

using namespace std;
using namespace rapidxml;


void readXML2FrameList()
{
	cout << "Dick" << endl;

	ifstream myfile("groundTruth.xml");
	xml_document<> doc;

	vector<char> buffer((istreambuf_iterator<char>(myfile)), istreambuf_iterator<char>( ));
	buffer.push_back('\0');
	doc.parse<0> (&buffer[0]);

	xml_node<>* framePointer;
	xml_node<>* objectListPointer;
	xml_node<>* objectPointer;
	xml_node<>* objectPropertyPointer;

	framePointer = doc.first_node()->first_node();

	cout << framePointer->first_attribute()->value() << endl;

	// Loop untill no frames are left
	while(framePointer != 0)
	{
		cout << "Frame number: " << framePointer->first_attribute()->value() << endl;

		objectListPointer = framePointer->first_node();
		objectPointer = objectListPointer->first_node();

		// Loop untill no objects are left
		while(objectPointer != 0)
		{
			// Extract information about objects
			cout << "Object id: " << objectPointer->first_attribute()->value() << endl;

			objectPropertyPointer = objectPointer->first_node("box");
			cout << "x: " << objectPropertyPointer->first_attribute("xc")->value() << endl;
			cout << "y: " << objectPropertyPointer->first_attribute("yc")->value() << endl;
			cout << "h: " << objectPropertyPointer->first_attribute("h")->value() << endl;
			cout << "w: " << objectPropertyPointer->first_attribute("w")->value() << endl;
			
			objectPointer = objectPointer->next_sibling();
		}

		framePointer = framePointer->next_sibling();
	}
}