function filter=nonringsig(size) 
% 
% filter=nonringsig(size) 
%
% Generates a spatial representation of a non-ringing filter
% with a squared cosine as magnitude envelope in the interval 
% size/2 < x < size/2.
%
% NOTE! For even sized arrays the point x=1+N/2, i.e the rightmost 
% element of the two center elements, is considered to correspond to 
% the origin.
% 
% Parameter  Range      Default      Description
% =========  =========  ===========  =================================
% size        1 - inf       9        Number of samples. 
% ====================================================================
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se

if nargin < 1
   size=9
end;

x=pi*sigplane([1 size])/size;
filter=cos(x).^2.*exp(i*(2*x+sin(2*x)));

norm=sum(abs(filter).^2);
filter=filter/sqrt(norm);



