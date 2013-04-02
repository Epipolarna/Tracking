#include "Identification.h"


namespace Identification
{
	
	//////////     Module     ///////////
	/////////////////////////////////////

	void Identifier::init(Algorithm algorithmName)
	{
		switch(algorithmName)
		{
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
	
	bool operator<(const std::list<Error> & l, const std::list<Error> & r) { return !l.empty() && !r.empty() && l.front().error < r.front().error; }

	void Identifier::algorithm_nearestFit(std::list<Frame> & frames)
	{
		Frame * current = &frames.front();
		Frame * previous = &(*(++frames.begin()));

		float error;
		int pIndex;
		
		errorMap.clear();

		// Calculate the error between each previous object and each current object
		for(std::vector<Object>::iterator c = current->objects.begin(); c != current->objects.end(); c++)
		{
			errorMap.push_back(std::list<Error>());
			pIndex = 0;
			for(std::vector<Object>::iterator p = previous->objects.begin(); p != previous->objects.end(); p++)
			{
				error = errorFunction(&(*c), &(*p));

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
		float error;
		int pIndex;

		// Clear all datastructures
		decidedPrevious.clear();
		decidedCurrent.clear();
		parents.clear();
		children.clear();
		undecidedPrevObject.clear();
		undecidedCurrObject.clear();
		errorMap.clear();
				
		for(std::vector<Object>::iterator p = previous->objects.begin(); p != previous->objects.end(); p++)
		{
			p->isChild = false;
			p->isParent = false;
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
			if((*child)->parents.size() != 1)
			{
				// Which parent is the most likely?
				parentError.clear();
				for(std::list<Object*>::iterator p = (*child)->parents.begin(); p != (*child)->parents.end(); p++)
				{
					error = errorFunction(*child, *p);
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
				Object * p = (*parent);
				Object * c = (*parent)->children.front();
				p->id = c->id;
				p->model = c->model;
				c->isChild = false;
				p->isParent = false;

				saturate_sizeChange(c, p);
				c->addPositionUncertainty(std::abs(c->x - p->x), std::abs(c->y - p->y));

				decidedCurrent.push_back(**parent);
			}
			else
			{				
				// Visualise Parent
				std::string objectText = "Parent (";

				for(std::list<Object*>::iterator child = (*parent)->children.begin(); child != (*parent)->children.end(); child++)
				{
					saturate_size(*child, *parent);					
					saturate_position(*child, *parent);
					
					// Add child as decided 'current'.
					decidedCurrent.push_back(**child);

					// Visualise Parent
					objectText += std::to_string((*child)->id) + ",";
				}

				// Visualise Parent
				objectText[objectText.size()-1] = ')';
				draw_parentText(current, *parent, objectText);
			}
		}
		
		
		// Calculate the error between each previous object and each current object
		for(std::list<Object*>::iterator c = undecidedCurrObject.begin(); c != undecidedCurrObject.end(); c++)
		{
			errorMap.push_back(std::list<Error>());
			pIndex = 0;
			for(std::list<Object*>::iterator p = undecidedPrevObject.begin(); p != undecidedPrevObject.end(); p++)
			{
				error = errorFunction(*c, *p);
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
			//saturate_sizeChange(c, p);

			// Add uncertainty
			c->addPositionUncertainty(std::abs(c->x - p->x), std::abs(c->y - p->y));

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
				if(c->isOutside(current->image.size().width, current->image.size().height) ||
				   c->isMovingOutside(current->image.size().width, current->image.size().height))
					decidedCurrent.erase(c++);
				else
					c++;
			}
		}


		// Duplicates can still occure...
		decidedCurrent.sort();
		decidedCurrent.unique();
		
		// Set which the actual current objects are
		current->objects.clear();
		std::copy(decidedCurrent.begin(), decidedCurrent.end(), back_inserter(current->objects));

		// Debug: print all objects and their status
		print_objectsAndStatus(decidedCurrent);
	}
	


//////// INTERNAL FUNCTIONS //////////
//////////////////////////////////////
	
	void Identifier::saturate_size(Object * child, Object * parent)
	{
		int widthDiff =  (int)std::floor(child->width - parent->width);
		int heightDiff = (int)std::floor(child->height - parent->height);
		if(widthDiff > 0)
			child->width -= widthDiff;
		if(heightDiff > 0)
			child->height -= heightDiff;

		// Add uncertainty
		child->addSizeUncertainty(std::max(widthDiff, 0), std::max(heightDiff, 0));
	}

	void Identifier::saturate_position(Object * child, Object * parent)
	{
		int leftDiff   = (int)std::floor((parent->x-parent->width/2)  - (child->x-child->width/2));
		int rightDiff  = (int)std::floor((child->x+child->width/2)    - (parent->x+parent->width/2));
		int topDiff    = (int)std::floor((parent->y-parent->height/2) - (child->y-child->height/2));
		int bottomDiff = (int)std::floor((child->y+child->height/2)   - (parent->y+parent->height/2));
		if(leftDiff > 0)
			{child->x += leftDiff;	child->xHat += leftDiff;	child->model.xHat.ptr<float>()[0] += float(leftDiff); }
		if(rightDiff > 0)
			{child->x -= rightDiff;	child->xHat -= rightDiff;	child->model.xHat.ptr<float>()[0] -= float(rightDiff); }
		if(topDiff > 0)
			{child->y+= topDiff;	child->yHat += topDiff;		child->model.xHat.ptr<float>()[1] += float(topDiff); }
		if(bottomDiff > 0)
			{child->y-= bottomDiff;	child->yHat -= bottomDiff;	child->model.xHat.ptr<float>()[1] -= float(bottomDiff); }
					
		// Add uncertainty
		child->addPositionUncertainty(std::min(std::abs(leftDiff) + std::abs(rightDiff), parent->width),
									  std::min(std::abs(topDiff)  + std::abs(bottomDiff), parent->height));
	}

	void Identifier::saturate_sizeChange(Object * c, Object * p)
	{
		float widthChange = 0;
		float heightChange = 0;
		if(std::abs(c->width - p->width) > acceptedSizeChange)
		{
			widthChange = sign(c->width - p->width)*acceptedSizeChange;
			c->width = p->width + widthChange;
		}
		if(std::abs(c->height - p->height) > acceptedSizeChange)
		{
			heightChange = sign(c->height - p->height)*acceptedSizeChange;
			c->height = p->height + heightChange;
		}

		// Add uncertainty
		c->addSizeUncertainty(widthChange, heightChange);
	}

	void Identifier::draw_parentText(Frame * frame, Object * parent, std::string & text)
	{
		rectangle(frame->image, Point(parent->x - parent->width/2,
									  parent->y - parent->height/2), 
								Point(parent->x + parent->width/2,
									  parent->y + parent->height/2), 
								Scalar(250, 0, 250), 1, 8);
		int fontFace = CV_FONT_HERSHEY_COMPLEX;
		double fontScale = 0.3;
		int thickness = (int)0.3;
		putText(frame->image, text, Point(parent->x+parent->width/2, parent->y-parent->height/2+10),
			    fontFace, fontScale, Scalar::all(255), thickness, 8);
	}

	void Identifier::print_objectsAndStatus(std::list<Object> & objects)
	{
		static int i = 0;	// Frame counter
		std::cout << std::to_string(i++) << " ------------------\n";
		int n;
		for(std::list<Object>::iterator c = objects.begin(); c != objects.end(); c++)
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
		std::cout << std::to_string(objects.size()) << " Objects in total\n";
	}


////////// TEST GENERATION ///////////
//////////////////////////////////////

	const int cTEST_FRAME_WIDTH = 480;
	const int cTEST_FRAME_HEIGHT = 360;

	#define NEW_FRAME() frameList.push_back(Frame(cv::Mat(cTEST_FRAME_HEIGHT, cTEST_FRAME_WIDTH, CV_8UC3))); frameList.back().image = Scalar(0,0,0);
	#define INSERT_OBJECT(x,y,dx,dy) frameList.back().objects.push_back(Object((int)x, (int)y, (float)dx, (float)dy, 0.0f, 0.0f, 20, 60));
	//#define INSERT_OBJECT(x,y) frameList.back().objects.push_back(Object(x, y, 0, 0, 20, 60));

	void generate_testdata(std::list<Frame> & frameList, std::string test)
	{
		if(test == "simple1")
		{
			int stepLength = 10;
			int var = 10;	// Variance
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
		return (float)(rand() - RAND_MAX/2.0)/RAND_MAX;
	}

	
	float errorFunction(Object * a, Object * b)
	{
		float distanceError = std::pow(a->x - b->x - b->dx, 2) + std::pow(a->y - b->y - b->dy, 2);
		float areaError = (float)std::pow(std::abs(a->width - b->width) + std::abs(a->width - b->width), 2);
		return distanceError + areaError;
	}

};