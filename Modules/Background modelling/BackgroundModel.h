#ifndef _BACKGROUND_MODEL_H_
#define _BACKGROUND_MODEL_H_

#include "../Frame.h"

namespace BackgroundModelling
{
	class BackgroundModel
	{
	public:
		void update(std::list<Frame> & frames);

	};
	// Additional function-/datastructuredeclarations here
}

#endif