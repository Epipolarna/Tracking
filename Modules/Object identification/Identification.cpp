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
		
		float distance, error;
		std::vector<std::list<ProbabilityContainer> > mostProbable;
		std::list<int> undecidedObjects;


		for(int i = 0; i < current->objects.size(); i++)
		{
			undecidedObjects.push_back(i);
			mostProbable.push_back(std::list<ProbabilityContainer>());
			
			for(int j = 0; j < last->objects.size(); j++)
			{
				/*
				 * distanceError = (x-x0-vx0)^2 + (y-y0-vy0)^2
				 */
				distance = std::pow(current->objects[i].x - last->objects[j].x - last->objects[j].dx, 2) + std::pow(current->objects[i].y - last->objects[j].y - last->objects[j].dy, 2);
				error = distance;
				
				mostProbable[i].push_back(ProbabilityContainer(j,last->objects[j].id,error));
			}
			mostProbable[i].sort();
		}

		std::cout << "\nFind most probable previous object:\n";
		int matchingPrevious;
		float min;
		std::list<int>::iterator bestMatch;
		for(int candidate = 0; candidate < last->objects.size(); candidate++)
		{
			min = 1000000;
			for(std::list<int>::iterator i = undecidedObjects.begin(); i != undecidedObjects.end(); i++)
			{
				if(mostProbable[*i].front().error < min)
				{
					min = mostProbable[*i].front().error;
					bestMatch = i;
				}
			}

			//A most probable candidate found!
			matchingPrevious = mostProbable[*bestMatch].front().index;
			current->objects[*bestMatch].id = last->objects[matchingPrevious].id;

			std::cout << "\tObject " << mostProbable[*bestMatch].front().probableId << " found with minError " << mostProbable[*bestMatch].front().error << "\n";

			for(std::list<int>::iterator i = undecidedObjects.begin(); i != undecidedObjects.end(); i++)
			{
				std::list<ProbabilityContainer>::iterator it = mostProbable[*i].begin();
				while(it != mostProbable[*i].end())
				{
					if(it->index == matchingPrevious)
						mostProbable[*i].erase(it++);
					else
						it++;
				}
				mostProbable[*i].sort();
			}
			undecidedObjects.erase(bestMatch);
		}

		
		//Take care of the new objects detected (still undecided):
		for(std::list<int>::iterator i = undecidedObjects.begin(); i != undecidedObjects.end(); i++)
		{
			current->objects[*i].id = newID();
		}
	}





	////////// TEST GENERATION ///////////
	//////////////////////////////////////

	const int cTEST_FRAME_WIDTH = 480;
	const int cTEST_FRAME_HEIGHT = 360;

	#define NEW_FRAME() frameList.push_back(Frame(cv::Mat(cTEST_FRAME_HEIGHT, cTEST_FRAME_WIDTH, CV_8UC3), cv::Mat(cTEST_FRAME_HEIGHT, cTEST_FRAME_WIDTH, CV_8UC3))); frameList.back().rawFrame = Scalar(0,0,0);
	#define INSERT_OBJECT(x,y,dx,dy) frameList.back().objects.push_back(Object(x, y, dx, dy, 20, 60));
	//#define INSERT_OBJECT(x,y) frameList.back().objects.push_back(Object(x, y, 0, 0, 20, 60));

	void generate_testdata(std::list<Frame> & frameList, std::string test)
	{
		if(test == "simple1")
		{
			int stepLength = 10;
			float var = 10;	// Variance
			for(int i = 0; i < cTEST_FRAME_WIDTH; i+=stepLength)
			{
				NEW_FRAME(); INSERT_OBJECT(i, 200+var*randf(), stepLength+var*randf(), var*randf()); INSERT_OBJECT(i, 100+10*randf(), stepLength+var*randf(), var*randf());
			}
		}
		else if(test == "complex1")
		{

		}

	}

	float randf()
	{
		return (float(rand()) - float(RAND_MAX)/2.0)/RAND_MAX;
	}

};