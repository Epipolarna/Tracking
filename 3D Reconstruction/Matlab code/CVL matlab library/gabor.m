function filterfft=gabor(sizevec,c0vec,sigmavec) 
% 
% filterfft=gabor([M N],[Mcf Ncf],[Ms Ns]) 
%
% Generates a 1D or 2D frequency representation of a gabor filter.
%
% NOTE! For even sized arrays the point (row,col)=(1+M/2,1+N/2), 
% i.e the lowest rightmost element of the four center elements,
% is considered to correspond to the origin.
% 
% Parameter  Range      Default      Description
% =========  =========  ===========  =================================
% M          0 - inf      11         Number of rows. 
% N          0 - inf      11         Number of columns. 
% Mcf          -           0         Vertical center frequency.
% Ncf          -           0         Horizontal center frequency.
% Ms         0 - inf       1         Vertical standard deviation.
% Ns         0 - inf       1         Horizontal standard deviation.
% ====================================================================
%
% See also GAUSSIAN
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se

twoDflag=0;

if nargin < 2
   c0r=0;
   c0c=0;
   twoDflag=twoDflag|0;
else
   [rows cols] = size(c0vec);
   if ((rows == 1) & (cols == 2)) | ((rows == 2) & (cols == 1))
      c0r=c0vec(1);
      c0c=c0vec(2);
      twoDflag=twoDflag|1;
   elseif (rows == 1) & (cols == 1)
         c0r=c0vec;
         c0c=c0vec;
         twoDflag=twoDflag|0;
       else
          fprintf('Wrong dimension on the center frequency vector')
	  c0vec
	  help gabor
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
	  help gabor
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
	  help gabor
          return
       end
end

if (cols <= 0)(rows <= 0)|(sigmar <= 0)|(sigmac <= 0)
   fprintf(1,'  ERROR! Illegal parameter value!!!!!!!!!!!!')
   help gabor
   return;
end;

if rows ~= 1 & twoDflag == 1
   if 2*floor(rows/2) < floor(rows) % odd size
      step=2*pi/rows;
      x=-(pi-step/2):step:(pi-step/2); 
   else 
      step=2*pi/rows;
      x=-pi:2*pi/rows:pi-step; 
   end
   filterfft = exp(-0.5*(((x-c0r)./sigmar).^2))';
else
   filterfft = 1;
end


if cols ~=1
   if 2*floor(cols/2) < floor(cols) % odd size
      step=2*pi/cols;
      x=-(pi-step/2):step:(pi-step/2); 
   else 
      step=2*pi/cols;
      x=-pi:2*pi/cols:pi-step; 
   end
   filterfft = filterfft * exp(-0.5*(((x-c0c)./sigmac).^2));
end



