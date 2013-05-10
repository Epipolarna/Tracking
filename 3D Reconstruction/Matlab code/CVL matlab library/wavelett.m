function y=wavelett1d(N)
% 
% y=wavelett1d(N)
%
% Generates a 1D wavelett in the signal domain. The other parameters
% are secret.
% 
% NOTE! For even sized arrays the point (row,col)=(1+M/2,1+N/2), 
% i.e the lowest rightmost element of the four center elements,
% is considered to correspond to the origin.
%
% Parameter  Range      Default      Description
% =========  =========  ===========  =================================
% N          0 - inf      11         Number of elements. 
% ====================================================================
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se
%


if nargin < 1
   N=61;
end

x=sigplane([1 N]);

y=gaussian(N,0,floor(N/10)+realmin).*cos(x*pi/max([1 floor(N/20)]));




