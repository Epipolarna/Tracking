function thresharr=mthresh(X, value) 
% 
% thresharr=mthresh(X, value) 
%
% Threshold on magnitude. The thresholded points are set to  NaN
% (Not a Number) in order to make them invisible when plotted.
%
% Parameter  Range      Default      Description
% =========  =========  ===========  =================================
% X             -           -        Any array. 
% value         -       0.1*max(X)   Threshold value.
% ====================================================================
%
% See also SURFDFT, SURFSIG 
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se

if nargin < 1
   help mthresh
   return
end;

if nargin < 2
   value=0.1*max(max(abs(X)));
end

thresharr=X;

[x,y]=find(abs(X)<value);

for i=1:length(x)
   thresharr(x(i),y(i))=NaN;
end
