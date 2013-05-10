function filter=gaussian(sizevec,shiftvec,sigmavec) 
% 
% filter=gaussian([M N],[Mo No],[Ms Ns]) 
%
% Generates a 1D or 2D Gaussian in the signal domain.
% 
% NOTE! For even sized arrays the point (row,col)=(1+M/2,1+N/2), 
% i.e the lowest rightmost element of the four center elements,
% is considered to correspond to the origin.
%
% Parameter  Range      Default      Description
% =========  =========  ===========  =================================
% M          0 - inf      11         Number of rows. 
% N          0 - inf      11         Number of columns. 
% Mo           -           0         Vertical offset in pixels. 
% No           -           0         Horizontal offset in pixels. 
% Ms         0 - inf       1         Vertical standard deviation.
% Ns         0 - inf       1         Horizontal standard deviation.
% ====================================================================
%
% See also GABOR
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se

twoDflag=0;

if nargin < 2
   rshift=0;
   cshift=0;
   twoDflag=twoDflag|0;
else
   [rows cols] = size(shiftvec);
   if ((rows == 1) & (cols == 2)) | ((rows == 2) & (cols == 1))
      rshift=shiftvec(1);
      cshift=shiftvec(2);
      twoDflag=twoDflag|1;
   elseif (rows == 1) & (cols == 1)
         rshift=shiftvec;
         cshift=shiftvec;
         twoDflag=twoDflag|0;
       else
          fprintf('Wrong dimension on the shift vector')
	  shiftvec
	  help gaussian
          return
       end
end

if nargin < 3
   sigmar=1;
   sigmac=1;
   twoDflag=twoDflag|0;
else
   [rows cols] = size(sigmavec);
   if ((rows == 1) & (cols == 2)) | ((rows == 2) & (cols == 1))
      sigmar=sigmavec(1);
      sigmac=sigmavec(2);
      twoDflag=twoDflag|1;
   elseif (rows == 1) & (cols == 1)
         sigmar=sigmavec;
         sigmac=sigmavec;
         twoDflag=twoDflag|0;
       else
          fprintf('Wrong dimension on the sigma vector')
	  sigmavec
	  help gaussian
          return
       end
end

if nargin < 1
   rows=11;
   cols=11;
   twoDflag=twoDflag|0;
else
   [rows cols] = size(sizevec);
   if ((rows == 1) & (cols == 2)) | ((rows == 2) & (cols == 1))
      rows=sizevec(1);
      cols=sizevec(2);
      twoDflag=twoDflag|1;
   elseif (rows == 1) & (cols == 1)
         rows=sizevec;
         cols=sizevec;
         twoDflag=twoDflag|0;
       else
          fprintf('Wrong dimension on the size vector')
	  sizevec
	  help gaussian
          return
       end
end

if (cols <= 0)(rows <= 0)|(sigmar <= 0)|(sigmac <= 0)
   fprintf(1,'  ERROR! Illegal parameter value!!!!!!!!!!!!')
   help gaussian
   return;
end;

if rows ~= 1 & twoDflag == 1
   x=sigplane([1 rows]);
   filter= (exp(-0.5*(((x-rshift)./sigmar).^2))/(sqrt(2*pi)*sigmar))';
else
   filter= 1;
end


if cols ~=1
   x=sigplane([1 cols]);
   filter = filter * exp(-0.5*(((x-cshift)./sigmac).^2))/(sqrt(2*pi)*sigmac);;
end



