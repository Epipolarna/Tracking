function filter=WFTsig(size,M,c0) 
% 
% filter=WFTsig(size,M,c0) 
%
% Generates a spatial representation of a Windowed Fourier Transform
% filter in the interval -M/2 < x < M/2.
%
% NOTE! For even sized arrays the point x=1+N/2, i.e the rightmost 
% element of the two center elements, is considered to correspond to 
% the origin.
% 
% Parameter  Range      Default      Description
% =========  =========  ===========  =================================
% size        1 - inf      65        Number of samples. 
% M           1 - inf      65        Spatial window size
% c0          0 - inf      2PI/M     Center frequency.
% ====================================================================
%
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se

if nargin < 1
   size=65;
end;

if nargin < 2
   M=size;
end;

if nargin < 3
   c0=2*pi/M;
end;

if (size <= 0)|(c0 < 0)|(M <= 0)
   fprintf(1,'  ERROR! Illegal parameter value!!!!!!!!!!!!')
   help WFTsig
   return
end;

x=sigplane([1 size]);
filter=(abs(x)<=M/2).*exp(i*c0*x);






