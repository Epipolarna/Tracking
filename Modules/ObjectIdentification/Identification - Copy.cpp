#include "Identification.h"


namespace Identification
{
	
	//////////     Module     ///////////
	/////////////////////////////////////

	void Identifier::init(Algorithm algorithmName)
	{
		switch(algorithmName)
		{
		case Naive:
			algorithm = &Identifier::algorithm_naive;
			break;
		case Test:
			algorithm = &Identifier::algorithm2;
			break;
		case Experimental:
			algorithm = &Identifier::algorithm3;
			break;
		case Ultimate:
			algorithm = &Identifier::algorithm_ultimate;
			break;
		case NearestFit:
			algorithm = &Identifier::algorithm_nearestFit;
			break;			
		}
	}

	void Identifier::identify(std::list<Frame> & frames)
	{
		if(frames.size() == 1)	// First time no objects are identified
		{
			for(int i = 0; i < frames.front().objects.size(); i++)
			{
				frames.front().objects[i].id = newID();
			}
		}
		else
		{
			(this->*algorithm)(frames);
		}
	}

	void Identifier::algorithm_naive(std::list<Frame> & frames)
	{
		Frame * current = &frames.front();
		Frame * previous = &(*(++frames.begin()));
		
		// Find the previous object which is probably the current object
		float distanceError, error;
		int pIndex, prevpIndex;
		std::list<ProbabilityContainer> mostProbable;

		isDecided.clear();
		for(std::vector<Object>::iterator p = previous->objects.begin(); p != previous->objects.end(); p++)
			isDecided.push_back(false);

		for(std::vector<Object>::iterator c = current->objects.begin(); c != current->objects.end(); c++)
		{
			mostProbable.push_back(ProbabilityContainer(-1, -1, 1000000));
			pIndex = 0;
			prevpIndex = -1;
			for(std::vector<Object>::iterator p = previous->objects.begin(); p != previous->objects.end(); p++)
			{
				/*
				 * distanceError = (x-x0-vx0)^2 + (y-y0-vy0)^2
				 */
				distanceError = std::pow(c->x - p->x - p->dx, 2) + std::pow(c->y - p->y - p->dy, 2);

				error = distanceError;
				if(!isDecided[pIndex] && mostProbable.back().error > error && error < 5000)
				{
					mostProbable.back().error = error;
					mostProbable.back().index = pIndex;
					if(prevpIndex >= 0)
						isDecided[prevpIndex] = false;
					isDecided[pIndex] = true;
					prevpIndex = pIndex;
				}
				pIndex++;
			}
		}

		pIndex = 0;
		for(std::list<ProbabilityContainer>::iterator p = mostProbable.begin(); p != mostProbable.end(); p++)
		{
			if(p->index >= 0)
			{
				current->objects[pIndex].id = previous->objects[p->index].id;
				current->objects[pIndex].model = previous->objects[p->index].model;
			}
			else
				current->objects[pIndex].id = newID();
			pIndex++;
		}
		
	}

	void Identifier::algorithm2(std::list<Frame> & frames)
	{
		
		Frame * current = &frames.front();
		Frame * previous = &(*(++frames.begin()));
		
		mostProbable.clear();
		undecidedObjects.clear();
		float distanceError, error;
		
		for(int i = 0; i < current->objects.size(); i++)
		{
			undecidedObjects.push_back(i);
			mostProbable.push_back(std::list<ProbabilityContainer>());
			
			for(int j = 0; j < previous->objects.size(); j++)
			{
				/*
				 * distanceError = (x-x0-vx0)^2 + (y-y0-vy0)^2
				 */
				distanceError = std::pow(current->objects[i].x - previous->objects[j].x - previous->objects[j].dx, 2) + std::pow(current->objects[i].y - previous->objects[j].y - previous->objects[j].dy, 2);
				error = distanceError;
				
				mostProbable[i].push_back(ProbabilityContainer(j,previous->objects[j].id,error));
			}
			mostProbable[i].sort();
		}

		//std::cout << "\nFind most probable previous object:\n";
		std::list<int>::iterator bestMatch;
		int matchingPrevious;
		float min;
		for(int candidate = 0; candidate < std::min(previous->objects.size(), current->objects.size()); candidate++)
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
			current->objects[*bestMatch].id = previous->objects[matchingPrevious].id;

			//std::cout << "\tObject " << mostProbable[*bestMatch].front().probableId << " found with minError " << mostProbable[*bestMatch].front().error << "\n";

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
	/*
		// Select the objects in the previous frame that are candidates to the unidentified in the current frame
		std::vector<Object *> candidates;
		candidates.clear();
		for(std::vector<Object>::iterator c = previous->objects.begin(); c != previous->objects.end(); c++)
		{
			if(!probablyOutsideOfImage(*c))
				candidates.push_back(&(*c));
		}
		*/
	void Identifier::algorithm3(std::list<Frame> & frames)
	{
		/* 
		   * Objects overlapp
		   * 
		 */

		Frame * current = &frames.front();
		Frame * previous = &(*(++frames.begin()));

		
		// Move lost objects
		
		std::cout << "\n---------------------\n";
		for(std::vector<Object>::iterator p = previous->objects.begin(); p != previous->objects.end(); p++)
		{
			std::cout << "(" << p->id << ") (dx,dy):(x,y):(xHat,yHat) = (" << p->dx << ", " << p->dy << "):(" << p->x << ", " << p->y << "):(" << p->xHat << ", " << p->yHat << ")\n";
			if(p->isLost)
			{
				
			}
		}
		
		// Find the previous object which is probably the current object
		float distanceError, error;
		int pIndex, prevpIndex;
		mostProbable_.clear();
		undecidedObjects.clear();

		isDecided.clear();
		for(std::vector<Object>::iterator p = previous->objects.begin(); p != previous->objects.end(); p++)
			isDecided.push_back(false);

		for(std::vector<Object>::iterator c = current->objects.begin(); c != current->objects.end(); c++)
		{
			mostProbable_.push_back(ProbabilityContainer(-1, -1, 1000000));
			pIndex = 0;
			prevpIndex = -1;
			for(std::vector<Object>::iterator p = previous->objects.begin(); p != previous->objects.end(); p++)
			{
				
				float area1 = c->containedAreaQuotient(*p);
				float area2 = p->containedAreaQuotient(*c);
				//std::cout << "(id1,id2)=(" << c->id << "," << p->id << "): " << area1 << " | " << area2 << "\n";

				/*
				 * distanceError = (x-x0-vx0)^2 + (y-y0-vy0)^2
				 */
				distanceError = std::pow(c->x - p->x - p->dx, 2) + std::pow(c->y - p->y - p->dy, 2);

				error = distanceError;
				if(!isDecided[pIndex] && mostProbable_.back().error > error && error < 10000)
				{
					mostProbable_.back().error = error;
					mostProbable_.back().index = pIndex;
					if(prevpIndex >= 0)
						isDecided[prevpIndex] = false;
					isDecided[pIndex] = true;
					prevpIndex = pIndex;
				}
				pIndex++;
			}
		}

		pIndex = 0;
		for(std::list<ProbabilityContainer>::iterator p = mostProbable_.begin(); p != mostProbable_.end(); p++)
		{
			if(p->index >= 0)
			{
				current->objects[pIndex].id = previous->objects[p->index].id;
				current->objects[pIndex].model = previous->objects[p->index].model;
			}
			else // Unidentified
			{
				current->objects[pIndex].id = newID();
			}				
			current->objects[pIndex].isLost = false;
			pIndex++;
		}

		
		// Find objects from last frame that was not "found" (lost) at the current frame
		bool lost;
		pIndex = 0;
		for(std::vector<Object>::iterator p = previous->objects.begin(); p != previous->objects.end(); p++)
		{
			lost = true;
			for(std::list<ProbabilityContainer>::iterator pr = mostProbable_.begin(); pr != mostProbable_.end(); pr++)
			{
				if(pr->index == pIndex)
				{
					lost = false;
					break;
				}
			}
			if(lost)
			{
				undecidedObjects.push_back(pIndex);
			}
			pIndex++;
		}

		int lostAmount = 0, lostAmount2 = 0;
		for(std::vector<Object>::iterator p = previous->objects.begin(); p != previous->objects.end(); p++)
		{
			if(p->isLost)
				lostAmount++;
		}
		 
		for(std::vector<Object>::iterator c = current->objects.begin(); c != current->objects.end(); c++)
		{
			if(c->isLost)
				lostAmount2++;
		}
		//std::cout << "previous: " << previous->objects.size() << " prevlost: " << lostAmount << " currlost: " << lostAmount2 << "\n";		

		int decidedAmount = current->objects.size();
		Object * cObject, * pObject;
		// Manage objects from last frame that was not "found" at the current frame
		for(std::list<int>::iterator l = undecidedObjects.begin(); l != undecidedObjects.end(); l++)
		{
			pObject = &previous->objects[*l];
			// Did it probably leave the screen?
			//...

			
			// Is it contained within a currently identified object?
			for(int cIndex = 0; cIndex < decidedAmount; cIndex++)
			{
				cObject = &current->objects[cIndex];
				float area1 = cObject->containedAreaQuotient(*pObject);
				float area2 = pObject->containedAreaQuotient(*cObject);
				
				//std::cout << "(id1,id2)=(" << cObject->id << "," << pObject->id << "): " << area1 << " | " << area2 << "\n";
				if(area2 > 0.1)	//How much of the later that 'was' inside of what *c now is
				{
					isDecided[*l] = true;
					pObject->isLost = true;
					current->objects.push_back(*pObject);
					break;
				}
			}
			/*
			// Is it just hidden behind something?
			if(!pObject->lost)
			{
				pObject->lost = true;
				current->objects.push_back(*pObject);
			}*/

		}

		current->profileData["#objectsID'd"] = decidedAmount;
		current->profileData["#objects lost"] = current->objects.size()-decidedAmount;

	}
	
	bool operator<(const std::list<Error> & l, const std::list<Error> & r) { return !l.empty() && !r.empty() && l.front().error < r.front().error; }

	void Identifier::algorithm_nearestFit(std::list<Frame> & frames)
	{
		Frame * current = &frames.front();
		Frame * previous = &(*(++frames.begin()));
		
		static std::list<std::list<Error>> errorMapping;
		static std::vector<std::list<std::list<Error>::iterator>> errorMapIterators;
		errorMapping.clear();
		errorMapIterators.clear();

		float distanceError, areaError, error;
		std::list<Error>::iterator errorMapIteratorIterator;
		std::list<std::list<Error>>::iterator errorMapIterator;
		int pIndex;
		for(std::vector<Object>::iterator c = current->objects.begin(); c != current->objects.end(); c++)
		{
			errorMapIterator = errorMapping.insert(errorMapping.end(), std::list<Error>());
			errorMapIterators.push_back(std::list<std::list<Error>::iterator>());
			pIndex = 0;
			for(std::vector<Object>::iterator p = previous->objects.begin(); p != previous->objects.end(); p++)
			{
				distanceError = std::pow(c->x - p->x - p->dx, 2) + std::pow(c->y - p->y - p->dy, 2);
				error = distanceError;

				errorMapIteratorIterator = errorMapping.back().insert(errorMapping.back().end(),Error(&(*p), &(*c), pIndex, error));
				errorMapIterators[pIndex].push_back(errorMapIteratorIterator);
				pIndex++;
			}
			errorMapping.back().sort();
		}

		for(int i = 0; i < std::min(current->objects.size(), previous->objects.size()); i++)
		{
			errorMapping.sort();
			errorMapping.front().front().current->id = errorMapping.front().front().previous->id;
			errorMapping.front().front().current->model = errorMapping.front().front().previous->model;
			errorMapping.front().front().current->isDecided = true;
			
			while(it != errorMapIterator->end())
				.erase(it)
			
		}

		for(std::vector<Object>::iterator c = current->objects.begin(); c != current->objects.end(); c++)
		{
			if(!c->isDecided)
				c->id = newID();
		}

		

	}

	void Identifier::algorithm_ultimate(std::list<Frame> & frames)
	{
		Frame * current = &frames.front();
		Frame * previous = &(*(++frames.begin()));

		const int MAX_OBJECT_AMOUNT = 100;

		bool isDecided[MAX_OBJECT_AMOUNT];	// isDecided[Index] == true  -->  current->objects[Index] is decided
		std::list<ProbabilityContainer> mostProbableObject[MAX_OBJECT_AMOUNT];  //
		//std::list<Object *> undecidedObjects, hiddenObjects, lostObjects;
		

		std::cout << "\n---------------------\n";
		for(std::vector<Object>::iterator p = previous->objects.begin(); p != previous->objects.end(); p++)
		{
			//std::cout << "(" << p->id << ") (dx,dy):(x,y):(xHat,yHat) = (" << p->dx << ", " << p->dy << "):(" << p->x << ", " << p->y << "):(" << p->xHat << ", " << p->yHat << ")\n";
			std::cout << "(" << p->id << ") [Parent|Child|Hidden|Lost]: [" << p->isParent << "|" << p->isChild << "|" << p->isHidden << "|" << p->isLost << "]";
		}
		
		//** **//		

		// Is a previous objects contained within a current object?
		//   Yes -> pObject is possibly child of cObject
		//   No  -> pObject is undecided
		std::vector<Object*> parents, children;
		std::vector<Object*> undecidedPrevObject, undecidedCurrObject;
		for(std::vector<Object>::iterator pObject = previous->objects.begin(); pObject != previous->objects.end(); pObject++)
		{
			for(std::vector<Object>::iterator cObject= current->objects.begin(); cObject != current->objects.end(); cObject++)
			{
				if(pObject->containedAreaQuotient(*cObject) > 0.1)	// p is a child of c which now is a parent
				{
					cObject->children.push_back(&(*pObject));
					pObject->parents.push_back(&(*cObject));
					parents.push_back(&(*cObject));
					children.push_back(&(*pObject));
				}
			}
			if(!pObject->isChild)
			{
				undecidedPrevObject.push_back(&(*pObject));
			}
		}

		// Does a child have a single parent?
		//   Yes -> It is probably its parent
		//   No  -> Find out which is the most probable parent
		for(std::vector<Object*>::iterator child = children.begin(); child != children.end(); child++)
		{
			if((*child)->parents.size() == 1)
			{

			}
			else
			{
				std::cout << ">>A child has multiple parents<<\n";
				// Which parent is the most likely?
				// If the parent that isn't most likely only had one child, remove it from parents and add it to undecidedPrevObjects
			}
			
			// The child is now decided, remove it from undecidedPrevObjects
			// ...
		}

		// Does the parent only have one child?
		//	 Yes -> They are the same object (probably)
		//   No  -> The parent is the composition of its children
		for(std::vector<Object*>::iterator parent = parents.begin(); parent != parents.end(); parent++)
		{
			if((*parent)->children.size() == 1)
			{
				(*parent)->children.front()->id = (*parent)->id;
				(*parent)->children.front()->model = (*parent)->model;
			}
			else
			{
				(*parent)->isParent = true;
				for(std::list<Object*>::iterator child = (*parent)->children.begin(); child != (*parent)->children.end(); child++)
				{
					(*child)->isChild = true;
					// Adjust width & height of child to fit inside of the parent
					// ...

					// Add child to current. Reset the child-iterator-value to this new address
					// ...
					current->objects.push_back(**child);
				}
			}
		}




		// Remove children (as they are already identified) from undecidedCurrendObjecets
		for(std::vector<Object>::iterator cObject= current->objects.begin(); cObject != current->objects.end(); cObject++)
		{
			if(!cObject->isChild)
				undecidedCurrObject.push_back(&(*cObject));
		}


		for(std::vector<Object*>::iterator pObject = undecidedPrevObject.begin(); pObject != undecidedPrevObject.end(); pObject++)
		{
			for(std::vector<Object*>::iterator cObject = undecidedCurrObject.begin(); cObject != undecidedCurrObject.end(); cObject++)
			{

			}
		}

		//** **//

		// for each previous object "p"
			// for each child "c" of p
				// add p to previous and remove child from p
			// for each current object "c"
				// if(c->contains(p, 0.1)) c.addChild(p); c.isChild = true; c.squeezeToFit(p); current->objects.add(c);
				// else
					// error = <estimated distanceError, area difference>
					// mostProbableObject[p].add(ProbabilityContainer(p, c, error))
			// mostProbableObject[p].sort()
		// mostProbableObject.sort()
		
		// for(int o = 0; o < previous->objects.size() - children.size(); o++)
			// Find the most probable object (the one with the lowest error) that is not already decided (isDecided)
				// current->objects[].id = previous->objects[].id
				// current->objects[].model = previous->objects[].model
				// isDecided[] = true
				// ... Magics!

		// for all undecided objects "o"
			// if (velocity < 0.001) velocity = 0
			// o.isLost = true
			// current->objects.add(o)

		// for all parent objects "p"
			// if(children.size() == 1)
				// p.id = p.child.id
				// p.model = p.child.model
			// else
				// p.isContainer = true

		// for all new undecided objects "o"
			// o.id = newID()
			// current->objects.add(o)

	}



	////////// TEST GENERATION ///////////
	//////////////////////////////////////

	const int cTEST_FRAME_WIDTH = 480;
	const int cTEST_FRAME_HEIGHT = 360;

	#define NEW_FRAME() frameList.push_back(Frame(cv::Mat(cTEST_FRAME_HEIGHT, cTEST_FRAME_WIDTH, CV_8UC3), cv::Mat(cTEST_FRAME_HEIGHT, cTEST_FRAME_WIDTH, CV_8UC3))); frameList.back().image = Scalar(0,0,0);
	#define INSERT_OBJECT(x,y,dx,dy) frameList.back().objects.push_back(Object(x, y, dx, dy, 0, 0, 20, 60));
	//#define INSERT_OBJECT(x,y) frameList.back().objects.push_back(Object(x, y, 0, 0, 20, 60));

	void generate_testdata(std::list<Frame> & frameList, std::string test)
	{
		if(test == "simple1")
		{
			int stepLength = 10;
			float var = 10;	// Variance
			for(int i = 0; i < cTEST_FRAME_WIDTH; i+=stepLength)
			{
				NEW_FRAME();
				INSERT_OBJECT(i, 200+var*randf(), stepLength+var*randf(), var*randf());
				INSERT_OBJECT(i, 100+10*randf(), stepLength+var*randf(), var*randf());
			}
		}
		else if(test == "complex1")
		{
			float var = 20;	// Variance
			for(int t = 0; t < cTEST_FRAME_WIDTH; t++)
			{
				NEW_FRAME();
				INSERT_OBJECT(t*10, 200+var*randf(), 10+var*randf(), var*randf());
				INSERT_OBJECT(20+t*10+10*randf(), 20+t*10+10*randf(), 10+var*randf(), var*randf());
			}
		}
		else if(test == "testing1")
		{
			float var = 20;	// Variance
			for(int t = 0; t < cTEST_FRAME_WIDTH; t++)
			{
				NEW_FRAME();
				INSERT_OBJECT(150, 100+t, 0, 0);
				INSERT_OBJECT(150, 100, 0, 0);
			}
		}
	}

	float randf()
	{
		return (float(rand()) - float(RAND_MAX)/2.0)/RAND_MAX;
	}

};