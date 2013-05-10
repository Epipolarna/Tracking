function filterfft=WFTdft(size,M,c0) 
% 
% filterfft=WFTdft(size,M,c0) 
%
% Generates a frequency representation of a Windowed Fourier Transform
% filter in the frequency interval -PI < u < PI.
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
   help WFTdft
   return
end;

u=dftplane([1 size]);
filterfft= M*sinc(M*(u-c0)/(2*pi));



