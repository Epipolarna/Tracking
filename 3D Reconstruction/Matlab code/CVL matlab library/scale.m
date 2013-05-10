function scaled_array=scale(array, scale_vec)
%
% scaled_array=scale(array, [Ms Ns])
%
% Scales and resamples a one or two dimensional array according to the
% horizontal and vertical scale factors. 
%
% NOTE! For even sized arrays the point (row,col)=(1+M/2,1+N/2), 
% i.e the lowest rightmost element of the four center elements,
% is considered to correspond to the origin.
%
% Parameter  Range      Default      Description
% =========  =========  ===========  ===================================
% array         -           -        Any 2D array. 
% Ms            -           2        Vertical scale factor.
% Ns            -           2        Horizontal scale factor.
% ================================== ===================================
%
% See also CIRCSHIFT, ROTATE
%
% BUGS:
%
% Does not work properly on even sized arrays
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se

if nargin < 1
   help scale
   return
end

if nargin < 2
   scale_vec = [2 2];
else
   [rows cols] = size(scale_vec);
   if ((rows == 1) & (cols == 2)) | ((rows == 2) & (cols == 1))
      % Do nothing
   elseif (rows == 1) & (cols == 1)
  	   scale_vec =[ scale_vec  scale_vec];
       else
          fprintf('Wrong dimension on the scale vector')
	  scale_vec
	  help scale
          return
       end
end

if scale_vec(1) <= 0 | scale_vec(2) <= 0
   fprintf('Scale factors must be positive')
   scale_vec
   help scale
   return
end

[rows cols] = size(array);

c0 =ceil(0.5*cols);
r0 =ceil(0.5*rows);

% scale vertical direction
if rows == 1
   tmp = array;
else
   r  = (1:rows)';
   or = (r-r0)/scale_vec(1);
   a  = abs(or-fix(or));
   b  = 1-a;

   bind = r0+fix(or);
   mask = (bind>0).*(bind<=rows);
   b = (b .*mask)*ones(1,cols);
   bind = bind.*mask + 1-mask;

   aind = bind+sign(or);
   mask = (aind>0).*(aind<=rows);

   a = (a.*mask)*ones(1,cols);
   aind=aind.*mask + 1-mask;
   tmp(r,:)=b.*array(bind,:) + a.*array(aind,:);
end

%Scale horisontal direction
if cols == 1
   scaled_array =tmp;
else
   r  = 1:cols;
   or = (r-c0)/scale_vec(2);
   a  = abs(or-fix(or));
   b  = 1-a;

   bind = c0+fix(or);
   mask = (bind>0).*(bind<=cols);
   b = ones(rows,1)*(b .*mask);
   bind = bind.*mask + 1-mask;

   aind = bind+sign(or);
   mask = (aind>0).*(aind<=cols);
   a = ones(rows,1)*(a.*mask);
   aind=aind.*mask + 1-mask;
   scaled_array(:,r)=b.*tmp(:,bind) + a.*tmp(:,aind);
end


