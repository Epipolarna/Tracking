function shifted=circshift(signal,shiftarr) 
% 
% shifted=circshift(array,[Ms Ns]) 
%
% Circular shift of 1D or 2D arrays. If Ms and/or Ns are non-integer a 
% bilinear interpolation between nearest neighbors is used.
%
% Parameter  Range      Default      Description
% =========  =========  ===========  =================================
% array         -                    Any array. 
% Ms            -           1        Number of row shift steps.
% Ns            -           1        Number of column shift steps.
% ====================================================================
%
% See also ROTATE, SCALE
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se

if nargin < 1
   help circshift;
   return
end;

if nargin < 2
   nrows=1;
   ncols=1;
else
   [rows cols] = size(shiftarr);
   if ((rows == 1) & (cols == 2)) | ((rows == 2) & (cols == 1))
      nrows=shiftarr(1);
      ncols=shiftarr(2);
   elseif (rows == 1) & (cols == 1)
          nrows=shiftarr;
          ncols=shiftarr;
       else
          fprintf('Wrong dimension on the shift array')
	  shiftarr
	  help scale
          return
       end
end

[rows,cols]=size(signal);

shifted=signal;

if (rows > 1)&(nrows ~= 0)
   if nrows < 0
      while nrows < -rows
         nrows = nrows+rows;
      end
      nrows=abs(nrows);
   else
      while nrows > rows
         nrows = nrows-rows;
      end
      nrows=rows-nrows;
   end
   N=floor(nrows);	
   alpha=nrows-N;
   shifted = (1-alpha)*[ shifted(N+1:rows,:); shifted(1:N,:)]+...
                 alpha*[ shifted(N+2:rows,:); shifted(1:N+1,:)];
end


if (cols > 1)&(ncols ~= 0)
   if ncols < 0
      while ncols < -cols
         ncols = ncols+cols;
      end
      ncols=abs(ncols);
   else
      while ncols > cols
         ncols = ncols-cols;
      end
      ncols=cols-ncols;
   end
   N=floor(ncols);	
   alpha=ncols-N;
   shifted = (1-alpha)*[ shifted(:,N+1:cols) shifted(:,1:N)]+...
                 alpha*[ shifted(:,N+2:cols) shifted(:,1:N+1)];
end

