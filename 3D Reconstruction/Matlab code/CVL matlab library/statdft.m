function ms=statsig(array)
%
% [mean stddev]=statsig(array)
%
% Calculate the mean and standard deviation of an array in the spatial
% domain.
%
% NOTE! For even sized arrays the point (row,col)=(1+M/2,1+N/2), 
% i.e the lowest rightmost element of the four center elements,
% is considered to correspond to the origin.

%
% Parameter  Range      Default      Description
% =========  =========  ===========  =================================
% array         -           -        Any 1D or 2d array
% ====================================================================
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se

if nargin < 1
   help statsig
   return
end

[rows cols]=size(array);

hr=floor(rows/2);
hc=floor(cols/2);

norm=sum(sum(conj(array).*array));

if rows == 1
   r =1;
   mr=0;
   sr=-1;
else
   if rem(rows,2) == 1
      r= -hr:hr;
   else
      r= (-hr+1):hr;
   end
   r= r' * ones(1,cols);
   mr=sum(sum(r.*conj(array).*array))/norm;
   sr=sqrt(sum(sum(((r-mr).^2).*conj(array).*array))/norm);
end

if cols == 1
   c=1;
   mc=0;
   sc=-1;
else
   if rem(cols,2) == 1
      c= -hc:hc;
   else
      c= (-hc+1):hc;
   end
   c= ones(rows,1) * c;
   mc=sum(sum(c.*conj(array).*array))/norm;
   sc=sum(sum(((c-mc).^2).*conj(array).*array))/norm;
end


ms=[];

if sr >=0
   ms = [mr;sr];
end

if sc >= 0
   ms = [ ms [mc ;sc]];
end

