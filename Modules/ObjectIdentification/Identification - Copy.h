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
	enum Algorithm
	{
		Naive = 0,
		Test,
		Experimental,
		NearestFit,
		Ultimate
	};


	//////////     Module     ///////////
	/////////////////////////////////////

	class Identifier
	{
	public:
		Identifier() {uniqueIDPool = 1; algorithm = &Identifier::algorithm_naive;}
		void identify(std::list<Frame> & frames);

		void init(Algorithm algorithmName);

	private:
		Algorithm algorithmName;
		void (Identifier::*algorithm)(std::list<Frame> & frames);

		int uniqueIDPool;
		int newID() {return uniqueIDPool++;}

		std::vector<std::list<ProbabilityContainer> > mostProbable;
		std::list<ProbabilityContainer> mostProbable_;
		std::list<int> undecidedObjects;
		std::vector<bool> isDecided;

		void algorithm_naive(std::list<Frame> & frames);
		void algorithm2(std::list<Frame> & frames);
		void algorithm3(std::list<Frame> & frames);
		void algorithm_nearestFit(std::list<Frame> & frames);
		void algorithm_ultimate(std::list<Frame> & frames);
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

	/*
	template<class T>
	class List
	{
	public:
		List * next;
		List * previous;
		T data;
		int listSize;

		List<T>() { next = previous = this; listSize = 0; }
		List<T>(T data) : data(data) {}
		void push_back(T data) { add(new List<T>(data), previous, this); }
		T front() { return next->data; }
		void add(List<T> * newNode, List<T> * leftNode, List<T> * rightNode)
		{
			newNode->next = rightNode;
			newNode->previous = leftNode;
			leftNode->next = newNode;
			rightNode->previous = newNode;
			listSize++;
		}		
		bool operator<(const List & l) const { return data < l.data; }
		
		class iterator
		{
		public:
			List * list;
		public:
			iterator() { list = 0; }
			iterator(List * list) : list(list) {}
			iterator & operator=(const iterator & i) { list = i.list; return *this; }
			bool operator==(const iterator & i) const { return list == i.list; }
			bool operator!=(const iterator & i) const { return list != i.list; }
			iterator & operator++() { list = list->next; return *this; }
			iterator operator++(int) { list = list->next; return *this; }
			T operator*() const { return list->data; }
			T operator->() const { return list->data; }
		};

		iterator begin() { return iterator(next); }
		iterator end() { return iterator(this); };
		void swap(const List & list)
		{ 
			List<T> * temp = next;
			next = list.next;
			list.next = temp;
			temp = previous;
			previous = list.previous;
			list.previous = temp;
		}
		int size() { return listSize; }
		bool empty() { return next == this; }
	};
	*/
	
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