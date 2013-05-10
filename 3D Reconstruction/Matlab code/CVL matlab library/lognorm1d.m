function filterfft=lognorm(size,c0,bw) 
% 
% filterfft=lognorm1d(N,c0,bw) 
%
% Generates a frequency representation of a lognorm filter in the 
% frequency interval -PI < u < PI.
%
% NOTE! For even sized arrays the point x=1+N/2, i.e the rightmost 
% element of the two center elements, is considered to correspond to 
% the origin.
% 
% Parameter  Range      Default      Description
% =========  =========  ===========  =================================
% N           1 - inf      11        Number of samples. 
% c0          0 - inf      PI/2      Center frequency.
% BW          0 - inf      1         Relative bandwidth.
% ====================================================================
%
% See also lognorm2d
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se

if nargin < 1
   size=11;
end;

if nargin < 2
   c0=pi/2;
end;

if nargin < 3
   bw=1;
end;

if (size <= 0)|(c0 < 0)|(bw < 0)
   fprintf(1,'  ERROR! Illegal parameter value!!!!!!!!!!!!')
   help lognorm1d
   return
end;

alpha=4/(log(2)*bw^2);
x=dftplane([1 size])+realmin;
filterfft=(x>0).*exp( -alpha*log(x/c0).^2);




