#ifndef _FOREGROUND_PROCESSOR_H_
#define _FOREGROUND_PROCESSOR_H_

#include "../frame.h"

namespace ForegroundProcessing
{
	class ForegroundProcessor
	{
	public:
		void segmentForeground(Frame & frame);
	};
	// Additional function-/datastructuredeclarations here
}

#endif