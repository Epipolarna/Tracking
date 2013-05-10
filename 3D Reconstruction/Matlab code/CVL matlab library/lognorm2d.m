function filterfft=lognormal2d(N,c0,bw,phi0,cosexp) 
% 
% filterfft=lognorm2d(N,c0,bw,phi0,cosexp) 
%
% Generates a 2D frequency representation of a lognormal filter on a
% square grid.
%
% NOTE! For even sized arrays the point (row,col)=(1+N/2,1+N/2), 
% i.e the lowest rightmost element of the four center elements,
% is considered to correspond to the origin.
% 
% Parameter  Range      Default      Description
% =========  =========  ===========  =================================
% N          1 - inf      11         Filter size.
% c0         0 - inf       0         Center frequency.
% bw         0 - inf       1         Relative bandwidth.
% phi0       0 - 2*PI      0         Filter direction. 
% cosexp     0 - inf       2         Angular function exponent.
% ====================================================================
%
% See also lognorm1d
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se

if nargin < 1
   N=11;
end;

if nargin < 2
   c0=pi/2;
end;

if nargin < 3
   bw=1;
end;

if nargin < 4
   phi0=0;
end;

if nargin < 5
   cosexp=2;
end;

if (N <= 0)|(c0 < 0)|(bw < 0)|(cosexp < 0)
   fprintf(1,'  ERROR! Illegal parameter value!!!!!!!!!!!!')
   help lognorm2d
   return
end;

alpha=4/(log(2)*bw^2);

z=dftplane([N N])+realmin;

angfunc=cos(angle(z)-phi0);

filterfft=(angfunc>=0).*exp(-alpha*log(abs(z)/c0).^2).*angfunc.^cosexp;








