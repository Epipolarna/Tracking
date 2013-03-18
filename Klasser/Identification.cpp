#include "Identification.h"


namespace Identification
{
	
	//////////     Module     ///////////
	/////////////////////////////////////

	void Identifier::identify(std::list<Frame> & frames)
	{
		// First time no objects are identified
		if(frames.size() == 1)
		{
			for(int i = 0; i < frames.front().objects.size(); i++)
			{
				frames.front().objects[i].id = newID();
			}
			return;
		}


		Frame * current = &frames.front();
		Frame * last = &(*(++frames.begin()));
		
		float distance, probability;
		std::vector<std::list<ProbabilityContainer> > mostProbable;
		std::list<int> undecidedObjects;


		for(int i = 0; i < current->objects.size(); i++)
		{
			undecidedObjects.push_back(i);
			mostProbable.push_back(std::list<ProbabilityContainer>());
			
			for(int j = 0; j < last->objects.size(); j++)
			{
				distance = std::pow(current->objects[i].x - last->objects[j].x, 2) + std::pow(current->objects[i].y - last->objects[j].y, 2);
				probability = distance;
				
				mostProbable[i].push_back(ProbabilityContainer(i,last->objects[j].id,probability));
			}
			mostProbable[i].sort();
		}

		float min;
		std::list<int>::iterator bestMatch;
		for(int candidate = 0; candidate < last->objects.size(); candidate++)
		{
			min = 1000000;
			for(std::list<int>::iterator i = undecidedObjects.begin(); i != undecidedObjects.end(); i++)
			{
				if(mostProbable[*i].front().probability < min)
				{
					min = mostProbable[*i].front().probability;
					bestMatch = i;
				}
			}

			//A most probable candidate found!
			current->objects[mostProbable[*bestMatch].front().index].id = mostProbable[*bestMatch].front().probableId;

			for(std::list<int>::iterator i = undecidedObjects.begin(); i != undecidedObjects.end(); i++)
			{
				std::list<ProbabilityContainer>::iterator it = mostProbable[*i].begin();
				while(it != mostProbable[*i].end())
				{
					if(it->probableId == last->objects[candidate].id)
						mostProbable[*i].erase(it++);
					else
						it++;
				}
				mostProbable[*i].sort();
			}
			undecidedObjects.erase(bestMatch);
		}

		
		//Take care of the unidentified:

	}





	////////// TEST GENERATION ///////////
	//////////////////////////////////////

	const int cTEST_FRAME_WIDTH = 480;
	const int cTEST_FRAME_HEIGHT = 360;

	#define NEW_FRAME() frameList.push_back(Frame(cv::Mat(cTEST_FRAME_HEIGHT, cTEST_FRAME_WIDTH, CV_8UC1), cv::Mat(cTEST_FRAME_HEIGHT, cTEST_FRAME_WIDTH, CV_8UC1)));
	#define INSERT_OBJECT(x,y) frameList.back().objects.push_back(Object(x, y, 0, 0, 20, 60));

	void generate_testdata(std::list<Frame> & frameList)
	{
		int stepLength = 10;
		for(int i = 0; i < cTEST_FRAME_WIDTH; i+=stepLength)
		{
			NEW_FRAME(); INSERT_OBJECT(i, 200); INSERT_OBJECT(i, 100);
		}
	}

};