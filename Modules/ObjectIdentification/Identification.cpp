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
			for(std::vector<Object>::iterator i = frames.front().objects.begin(); i != frames.front().objects.end(); i++)
			{
				i->id = newID();
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
		
		
		
	}

	
	bool operator<(const std::list<Error> & l, const std::list<Error> & r) { return !l.empty() && !r.empty() && l.front().error < r.front().error; }

	void Identifier::algorithm_nearestFit(std::list<Frame> & frames)
	{
		Frame * current = &frames.front();
		Frame * previous = &(*(++frames.begin()));

		static std::list<std::list<Error>> errorMap;
		errorMap.clear();
		
		float error, distanceError;
		int pIndex;

		// Calculate the error between each previous object and each current object
		for(std::vector<Object>::iterator c = current->objects.begin(); c != current->objects.end(); c++)
		{
			errorMap.push_back(std::list<Error>());
			pIndex = 0;
			for(std::vector<Object>::iterator p = previous->objects.begin(); p != previous->objects.end(); p++)
			{
				distanceError = std::pow(c->x - p->x - p->dx, 2) + std::pow(c->y - p->y - p->dy, 2);
				error = distanceError;

				errorMap.back().push_back(Error(&(*c), &(*p), pIndex, error));
				pIndex++;
			}
			errorMap.back().sort();
		}

		// Those previous-current object pair with smallest error is assumed to be the same object
		for(int i = 0; i < std::min(current->objects.size(), previous->objects.size()); i++)
		{
			errorMap.sort();
			errorMap.front().front().current->id = errorMap.front().front().previous->id;
			errorMap.front().front().current->model = errorMap.front().front().previous->model;
			errorMap.front().front().current->isDecided = true;

			// Remove all occurances of this currentObject
			for(std::list<std::list<Error>>::iterator errorMapI = ++(errorMap.begin()); errorMapI != errorMap.end(); errorMapI++)
			{
				std::list<Error>::iterator errorI = errorMapI->begin();
				while(errorI != errorMapI->end())
				{
					if(errorI->current == errorMap.front().front().current)
					{
						errorMapI->erase(errorI);
						break;
					}
					errorI++;
				}
			}

			// Remove all occurances of this previousObject
			errorMap.erase(errorMap.begin());
		}

		// Any current object not decided is assumed to be a new object
		for(std::vector<Object>::iterator c = current->objects.begin(); c != current->objects.end(); c++)
		{
			if(!c->isDecided)
				c->id = newID();
		}

	}


	bool object_compare(Object & l, Object & r) { return l.id == r.id; }
	
	void Identifier::algorithm_ultimate(std::list<Frame> & frames)
	{
		Frame * current = &frames.front();
		Frame * previous = &(*(++frames.begin()));
				
		static int i = 0;

		i++;
		//if(i == 294)
		//	std::cout << "ojo\n";

		std::cout << std::to_string(i) << " ------------------\n";

		static std::list<Object> decidedPrevious, decidedCurrent;

		static std::list<Object*> parents, children;
		static std::list<Object*> undecidedPrevObject, undecidedCurrObject;
		static std::list<std::list<Error>> errorMap;
		static std::list<Error> parentError;
		static int acceptedSizeChange = 10;
		static int maxError = 5000;
		float error, distanceError, areaError;
		int pIndex;

		decidedPrevious.clear();
		decidedCurrent.clear();
		parents.clear();
		children.clear();
		undecidedPrevObject.clear();
		undecidedCurrObject.clear();
		errorMap.clear();
		/*
		for(std::vector<Object>::iterator i = current->objects.begin(); i != current->objects.end(); i++)
		{
			undecidedCurrObject.push_back(&(*i));
		}
		
		for(std::vector<Object>::iterator i = previous->objects.begin(); i != previous->objects.end(); i++)
		{
			undecidedPrevObject.push_back(&(*i));
		}*/
		
		for(std::vector<Object>::iterator p = previous->objects.begin(); p != previous->objects.end(); p++)
		{
			p->isChild = false;
			p->isParent = false;
			//p->isLost = false;
			p->isDecided = false;
			p->children.clear();
			p->parents.clear();
		}
		
		// Find parents and children
		for(std::vector<Object>::iterator c = current->objects.begin(); c != current->objects.end(); c++)
		{
			for(std::vector<Object>::iterator p = previous->objects.begin(); p != previous->objects.end(); p++)
			{
				if(!p->isLost && p->containedAreaQuotient(*c) > 0.2)	// p (previous object) is a child of c (current object)
				{
					parents.push_back(&(*c));
					children.push_back(&(*p));
					c->children.push_back(&(*p));
					p->parents.push_back(&(*c));
					c->isParent = true;
					p->isChild = true;

					//if(c->children.size() > 2)
						//std::cout << "ohnoes\n";

					//std::cout << ">Parent with " << c->children.size() << " children\n";
					int index = 1;
					for(std::list<Object*>::iterator child = c->children.begin(); child != c->children.end(); child++)
					{
						//std::cout << "\t Child " << std::to_string(index++) << ": " << (*child)->id << "\n";
					}
				}
			}
			if(!c->isParent)
				undecidedCurrObject.push_back(&(*c));
		}

		// Remove duplicates in parent list (possible speed up: Use bool array for check and dynamic range of memset:false before anything)
		parents.sort();
		parents.unique();

		// Find undecided Previous objects (those who are not children)
		for(std::vector<Object>::iterator p = previous->objects.begin(); p != previous->objects.end(); p++)
		{
			if(!p->isChild)
				undecidedPrevObject.push_back(&(*p));
		}

		// Does a child have a single parent?
		//   Yes -> It is probably its parent
		//   No  -> Find out which is the most probable parent
		for(std::list<Object*>::iterator child = children.begin(); child != children.end(); child++)
		{
			if((*child)->parents.size() == 1)
			{

			}
			else
			{
				//std::cout << ">>A child has multiple parents<<\n";

				// Which parent is the most likely?
				parentError.clear();
				for(std::list<Object*>::iterator p = (*child)->parents.begin(); p != (*child)->parents.end(); p++)
				{
					distanceError = std::pow((*child)->x - (*p)->x - (*p)->dx, 2) + std::pow((*child)->y - (*p)->y - (*p)->dy, 2);
					areaError = std::pow(std::abs((*child)->width - (*p)->width) + std::abs((*child)->width - (*p)->width), 2);
					error = distanceError + areaError;

					parentError.push_back(Error(0, *p, 0, error));
				}
				parentError.sort();


				for(std::list<Error>::iterator errorI = ++(parentError.begin()); errorI != parentError.end(); errorI++)
				{
					for(std::list<Object*>::iterator pI = (errorI->previous)->children.begin(); pI != (errorI->previous)->children.end(); pI++)
					{
						if(*pI == *child)
						{
							(errorI->previous)->children.erase(pI);
							break;
						}
					}

				    // If the parent that isn't most likely only had one child, remove it from parents and add it to undecidedPrevObjects
					if((errorI->previous)->children.empty())
					{
						for(std::list<Object*>::iterator pI = parents.begin(); pI != parents.end(); pI++)
						{
							if(errorI->previous == *pI)
							{
								parents.erase(pI);
								break;
							}
						}
					}
				}
			}
		}


		// Does the parent only have one child?
		//	 Yes -> They are the same object (probably)
		//   No  -> The parent is the composition of its children
		for(std::list<Object*>::iterator parent = parents.begin(); parent != parents.end(); parent++)
		{
			if((*parent)->children.size() == 1)
			{
				(*parent)->id = (*parent)->children.front()->id;
				(*parent)->model = (*parent)->children.front()->model;
				(*parent)->children.front()->isChild = false;
				(*parent)->isParent = false;

				// Force slow width/height changes
				Object * c = (*parent);
				Object * p = (*parent)->children.front();
				if(std::abs(c->width - p->width) > acceptedSizeChange)
					c->width = p->width + sign(c->width - p->width)*acceptedSizeChange;
				if(std::abs(c->height - p->height) > acceptedSizeChange)
					c->height = p->height + sign(c->height - p->height)*acceptedSizeChange;

				decidedCurrent.push_back(**parent);
			}
			else
			{
				// Debug
				//std::cout << "Parent with " << (*parent)->children.size() << " children\n";
				//std::cout << "\t Child 1: " << (*parent)->children.front()->id << "\n";
				
				// Visualise Parent
				std::string objectText = "Parent (";

				for(std::list<Object*>::iterator child = (*parent)->children.begin(); child != (*parent)->children.end(); child++)
				{
					/*
					// Force slow width/height changes
					Object * c = (*child);
					Object * p = (*parent);
					if(std::abs(c->width - p->width) > acceptedSizeChange)
						c->width = p->width + sign(c->width - p->width)*acceptedSizeChange;
					if(std::abs(c->height - p->height) > acceptedSizeChange)
						c->height = p->height + sign(c->height - p->height)*acceptedSizeChange;
					*/

					// Adjust width & height of child to fit inside of the parent
					float widthDiff =  (*child)->width - (*parent)->width;
					float heightDiff = (*child)->height - (*parent)->height;
					if(widthDiff > 0)
						(*child)->width -= widthDiff;
					if(heightDiff > 0)
						(*child)->height -= heightDiff;

					// Adjust position of child to fit inside of parent
					float leftDiff   = ((*parent)->x-(*parent)->width/2)  - ((*child)->x-(*child)->width/2);
					float rightDiff  = ((*child)->x+(*child)->width/2)    - ((*parent)->x+(*parent)->width/2);
					float topDiff    = ((*parent)->y-(*parent)->height/2) - ((*child)->y-(*child)->height/2);
					float bottomDiff = ((*child)->y+(*child)->height/2)   - ((*parent)->y+(*parent)->height/2);
					if(leftDiff > 0)
						{(*child)->x += leftDiff;	(*child)->xHat += leftDiff; (*child)->model.xHat.ptr<float>()[0] += leftDiff; }
					if(rightDiff > 0)
						{(*child)->x -= rightDiff;	(*child)->xHat -= rightDiff; (*child)->model.xHat.ptr<float>()[0] -= rightDiff; }
					if(topDiff > 0)
						{(*child)->y+= topDiff;		(*child)->yHat += topDiff; (*child)->model.xHat.ptr<float>()[1] += topDiff; }
					if(bottomDiff > 0)
						{(*child)->y-= bottomDiff;	(*child)->yHat -= bottomDiff; (*child)->model.xHat.ptr<float>()[1] -= bottomDiff; }
					
					// Add child as decided 'current'.
					decidedCurrent.push_back(**child);

					// Visualise Parent
					objectText += std::to_string((*child)->id) + ",";
				}

				// Visualise Parent
				objectText[objectText.size()-1] = ')';
				rectangle(current->image, Point((*parent)->x-(*parent)->width/2, (*parent)->y-(*parent)->height/2), 
											Point((*parent)->x+(*parent)->width/2,   (*parent)->y+(*parent)->height/2), 
											Scalar(250, 0, 250), 1, 8);
				int fontFace = CV_FONT_HERSHEY_COMPLEX;
				double fontScale = 0.3;
				int thickness = (int)0.3;
				putText(current->image, objectText, Point((*parent)->x+(*parent)->width/2, (*parent)->y-(*parent)->height/2+10), fontFace, fontScale, Scalar::all(255), thickness, 8);
			}
		}
		
		
		// Calculate the error between each previous object and each current object
		for(std::list<Object*>::iterator c = undecidedCurrObject.begin(); c != undecidedCurrObject.end(); c++)
		{
			errorMap.push_back(std::list<Error>());
			pIndex = 0;
			for(std::list<Object*>::iterator p = undecidedPrevObject.begin(); p != undecidedPrevObject.end(); p++)
			{
				distanceError = std::pow((*c)->x - (*p)->x - (*p)->dx, 2) + std::pow((*c)->y - (*p)->y - (*p)->dy, 2);
				areaError = std::pow(std::abs((*c)->width - (*p)->width) + std::abs((*c)->width - (*p)->width), 2);
				error = distanceError + areaError;

				errorMap.back().push_back(Error(*c, *p, pIndex, error));
				pIndex++;
			}
			errorMap.back().sort();
		}

		// Those previous-current object pair with smallest error is assumed to be the same object
		for(int i = 0; i < std::min(undecidedCurrObject.size(), undecidedPrevObject.size()); i++)
		{
			errorMap.sort();
									
			if(errorMap.front().front().error > maxError)
				break;
			
			Object * c = errorMap.front().front().current;
			Object * p = errorMap.front().front().previous;
			c->id = p->id;
			c->model = p->model;
			c->isDecided = true;
			c->isLost = false;
						
			// Force slow width/height changes
			if(std::abs(c->width - p->width) > acceptedSizeChange)
				c->width = p->width + sign(c->width - p->width)*acceptedSizeChange;
			if(std::abs(c->height - p->height) > acceptedSizeChange)
				c->height = p->height + sign(c->height - p->height)*acceptedSizeChange;

			decidedCurrent.push_back(*c);
			decidedCurrent.back().model = p->model;

			// Remove from undecidedPrevious
			for(std::list<Object*>::iterator up = undecidedPrevObject.begin(); up != undecidedPrevObject.end(); up++)
				if((*up)->id == p->id)
				{
					undecidedPrevObject.erase(up);
					break;
				}

			// Remove all occurances of this currentObject
			for(std::list<std::list<Error>>::iterator errorMapI = ++(errorMap.begin()); errorMapI != errorMap.end(); errorMapI++)
			{
				std::list<Error>::iterator errorI = errorMapI->begin();
				while(errorI != errorMapI->end())
				{
					if(errorI->current == c)
					{
						errorMapI->erase(errorI);
						break;
					}
					errorI++;
				}
			}

			// Remove all occurances of this previousObject
			errorMap.erase(errorMap.begin());
		}

		// Any current object not decided is assumed to be a new object
		for(std::list<Object*>::iterator c = undecidedCurrObject.begin(); c != undecidedCurrObject.end(); c++)
		{
			if(!(*c)->isDecided)
			{
				(*c)->id = newID();
				decidedCurrent.push_back(**c);
			}
		}

		
		// Any previous object not decided is assumed to be lost
		for(std::list<Object*>::iterator c = undecidedPrevObject.begin(); c != undecidedPrevObject.end(); c++)
		{
			if(!(*c)->isDecided)
			{
				(*c)->isLost = true;
				decidedCurrent.push_back(**c);
			}
		}

		// Any object outside of the image or on their way out should be removed!
		if(!decidedCurrent.empty())
		{
			std::list<Object>::iterator c = decidedCurrent.begin();
			while(c != decidedCurrent.end())
			{
				if(c->x - c->width/2 + c->dx*2 < 0 || c->x + c->width/2 + c->dx*2 > current->image.size().width || c->y - c->height/2 + c->dy*2 < 0 || c->y + c->height/2 + c->dy*2 > current->image.size().height)
					decidedCurrent.erase(c++);
				else if(c->x - c->width/2 < 0 || c->x + c->width/2 > current->image.size().width || c->y - c->height/2 < 0 || c->y + c->height/2 > current->image.size().height)
					decidedCurrent.erase(c++);
				else
					c++;
			}
		}


		// Duplicates can still occure, why?
		decidedCurrent.sort();
		decidedCurrent.unique();
		
		// Set which the actual current objects are
		current->objects.clear();
		std::copy(decidedCurrent.begin(), decidedCurrent.end(), back_inserter(current->objects));

		//Debug: print all objects and their status
		int n;
		for(std::list<Object>::iterator c = decidedCurrent.begin(); c != decidedCurrent.end(); c++)
		{
			std::cout << "Object " << std::to_string(c->id) << ": ";
			if(c->isLost) std::cout << "LOST";
			if(c->isChild) std::cout << "CHILD";
			if(c->isParent) std::cout << "PARENT";
			std::cout << "\n";
			n = 0;
			if(c->isParent)
				for(std::list<Object*>::iterator child = c->children.begin(); child != c->children.end(); c++, n++)
					std::cout << "\tChild " << std::to_string(n) << ": " << std::to_string((*child)->id) << "\n";
		}
		std::cout << std::to_string(decidedCurrent.size()) << " Objects in total\n";
				
		
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