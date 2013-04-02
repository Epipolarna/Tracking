#ifndef _IDENTIFICATION_H_
#define _IDENTIFICATION_H_

#include "../Frame.h"
#include <list>
#include <cmath>
#include <queue>
#include <algorithm>

namespace Identification
{
	
	class ProbabilityContainer;
	class Error;
	enum Algorithm
	{
		NearestFit = 0,
		Ultimate
	};

	template <typename T> int sign(T val) {
		return (T(0) < val) - (val < T(0));
	}
	float errorFunction(Object * a, Object * b);


	//////////     Module     ///////////
	/////////////////////////////////////

	class Identifier
	{
	public:
		Identifier() {uniqueIDPool = 1; algorithm = &Identifier::algorithm_nearestFit; acceptedSizeChange = 10; maxError = 5000;}
		void identify(std::list<Frame> & frames);
		
		void init(Algorithm algorithmName, float maxError = 5000, float acceptedSizeChange = 10);
		
	private:
		// Algorithm manager
		Algorithm algorithmName;
		void (Identifier::*algorithm)(std::list<Frame> & frames);
				
		// Algorithms
		void algorithm_nearestFit(std::list<Frame> & frames);
		void algorithm_ultimate(std::list<Frame> & frames);
				
		// Internal methods
		int uniqueIDPool;
		int newID() {return uniqueIDPool++;}
		void saturate_size(Object * child, Object * parent);		// Adjust width & height of child to fit inside of the parent
		void saturate_position(Object * child, Object * parent);	// Adjust position of child to fit inside of parent
		void saturate_sizeChange(Object * c, Object * p);			// Adjust width & height so they do not change faster than 'acceptedSizeChange'
		void draw_parentText(Frame * frame, Object * parent, std::string & text);
		void print_objectsAndStatus(std::list<Object> & objects);

		// Internal containers
		std::list<Object> decidedPrevious, decidedCurrent;
		std::list<Object*> parents, children;
		std::list<Object*> undecidedPrevObject, undecidedCurrObject;
		std::list<std::list<Error>> errorMap;
		std::list<Error> parentError;
		int acceptedSizeChange;
		int maxError;
	};
	

	//////////  Internal structures  ///////////
	////////////////////////////////////////////

	class Error
	{
	public:
		Object * current;
		Object * previous;
		int previousIndex;
		float error;
		Error(Object * current, Object * previous, int previousIndex, float error)
			: current(current),previous(previous), previousIndex(previousIndex), error(error) {}
		bool operator<(const Error & e) { return error < e.error; }
	};	



	////////// TEST GENERATION ///////////
	//////////////////////////////////////

	void generate_testdata(std::list<Frame> & frameList, std::string test = "simple1");
	float randf();
}

#endif