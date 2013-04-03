/* TSBB15 Tracking project 2013
 * - - - - - - - - - - - - - - -
 * Principal contributer: Mattias Tiger
 */

#include <time.h>

#ifndef _PROFILER_H_
#define _PROFILER_H_

class ProfilerClock
{
public:
	int totalTime, lapTime;

	ProfilerClock() { totalTime = lapTime = clock(); }

	void reset() { totalTime = lapTime = clock(); }
	void lap() { lapTime = clock(); }

	double getTime() { return double(clock()-lapTime)/CLOCKS_PER_SEC; }
	double getTotalTime() { return double(clock()-totalTime)/CLOCKS_PER_SEC; }
	double getFPS() { return 1 / (double(clock()-totalTime)/CLOCKS_PER_SEC); }
};

#endif