function odd_part=odd(array)
% 
% odd_part=odd(array)
%
% Calculates the odd part of an array where the middle element is 
% assumed to be the origin. 
%
% NOTE! For even sized arrays the point (row,col)=(1+M/2,1+N/2), 
% i.e the lowest rightmost element of the four center elements,
% is considered to correspond to the origin.
%
% Parameter  Range      Default      Description
% =========  =========  ===========  =================================
% array         -           -        Any 2D array. 
% ================================== =================================
%
% See also EVEN
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se

if nargin < 1
   help odd
   return
end

[rows cols]=size(array);

if rem(rows,2)==0
  array = [ array; zeros(1,cols)];
  rows=rows+1;
  rflag=1;
else
  rflag=0;
end

if rem(cols,2)==0
  array = [ array zeros(rows,1)];
  cols=cols+1;
  cflag=1;
else
  cflag=0;
end

odd_part = 0.5*(array(1:rows,1:cols)-array(rows:-1:1,cols:-1:1));

if rflag == 1
   odd_part=odd_part(1:rows-1,:);
end

if cflag == 1
   odd_part=odd_part(:,1:cols-1);
end



