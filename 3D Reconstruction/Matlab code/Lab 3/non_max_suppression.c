#include "mex.h"
#include "math.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double *signal;
    const int *sdim;
    double windowsize=3;
    int displacement;
    mxArray *result;
    double *result_re;
    int ind0,ind1,wind0,wind1,sind0,sind1;
    int signalindex,middleind;
    double middlevalue,value;
    int next;
    
    sdim=mxGetDimensions(prhs[0]);
    signal=mxGetPr(prhs[0]);

    result=mxCreateDoubleMatrix(sdim[0],sdim[1],mxREAL);
    result_re=mxGetPr(result);

    if(nrhs<2)
	windowsize=3;
    else
	windowsize=mxGetScalar(prhs[1]);
    
    displacement=(windowsize-1)/2;

    /* Loop over the signal dimensions */
    for(ind0=0;ind0<sdim[0];ind0++)
    {
	for(ind1=0;ind1<sdim[1];ind1++)
	{
	    middleind=sdim[0]*ind1+ind0;
	    middlevalue=signal[middleind];

	    next=0;
	    /* Loop over the dimensions for the window. */
	    for(wind0=0;wind0<windowsize;wind0++)
	    {
		if(next==1)
		    continue;
		sind0=ind0+wind0-displacement;
		if(sind0<0 || sind0>=sdim[0])
		    continue;
		for(wind1=0;wind1<windowsize;wind1++)
		{
		    if(next==1)
			continue;
		    sind1=ind1+wind1-displacement;
		    if(sind1<0 || sind1>=sdim[1])
			continue;
		    signalindex=sdim[0]*sind1+sind0;
		    value=signal[signalindex];
		    if(middlevalue<value)
		    {
			next=1;
			middlevalue=0.0;
			continue;
		    }
		}
	    }
	    result_re[middleind]=middlevalue;
	}
    }
    plhs[0]=result;
}
