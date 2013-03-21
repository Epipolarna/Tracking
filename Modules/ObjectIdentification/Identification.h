#ifndef _IDENTIFICATION_H_
#define _IDENTIFICATION_H_

#include "../Frame.h"
#include <list>
#include <cmath>
#include <queue>

namespace Identification
{
	
	class ProbabilityContainer;
	enum Algorithm
	{
		Naive = 0,
		Test,
		Experimental
	};


	//////////     Module     ///////////
	/////////////////////////////////////

	class Identifier
	{
	public:
		Identifier() {uniqueIDPool = 1; algorithm = &Identifier::algorithm1;}
		void identify(std::list<Frame> & frames);

		void init(Algorithm algorithmName);

	private:
		Algorithm algorithmName;
		void (Identifier::*algorithm)(std::list<Frame> & frames);

		int uniqueIDPool;
		int newID() {return uniqueIDPool++;}

		std::vector<std::list<ProbabilityContainer> > mostProbable;
		std::list<int> undecidedObjects;
		std::vector<bool> isDecided;

		void algorithm1(std::list<Frame> & frames);
		void algorithm2(std::list<Frame> & frames);
		void algorithm3(std::list<Frame> & frames);
	};
	

	//////////  Internal structures  ///////////
	////////////////////////////////////////////

	class ProbabilityContainer
	{
	public:
		int index;
		int probableId;
		float error;
		ProbabilityContainer(int index, int probableId, float error) : index(index),probableId(probableId),error(error) {}
		bool operator<(const ProbabilityContainer & pc) { return error < pc.error; }
	};

	struct Condition
	{
		int indexValue;
		Condition(int indexValue) : indexValue(indexValue) {}
		bool operator() (const ProbabilityContainer& pc) { return pc.index == indexValue; }
	};


	////////// TEST GENERATION ///////////
	//////////////////////////////////////

	void generate_testdata(std::list<Frame> & frameList, std::string test = "simple1");
	float randf();
}

#endif