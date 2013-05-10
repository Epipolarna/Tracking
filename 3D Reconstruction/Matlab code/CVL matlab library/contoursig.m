function out=contoursig(sigarray, n, d, viewmatrix) 
% 
% out=contoursig(sigarray, n, d, viewmatrix) 
%
% Contoursig interprets a 2D array as a spatial signal representation.
% The contour lines corresponds to the isovalues of the magnitude of
% the signal.
% 
% A global variable SD is expected to define which figure window to 
% use.
%
% NOTE! For even sized arrays the point (row,col)=(1+M/2,1+N/2), 
% i.e the lowest rightmost element of the four center elements,
% is considered to correspond to the origin.
% 
% Parameter  Range      Default      Description
% =========  =========  ===========  =================================
% sigarray      -           -        Any 2D array. 
% n           0 - inf       5        Number of contour lines.
% d           2 - 3         2        2D or 3D contour plot
% viewmatrix    -           -        Viewpoint spec. see Contour
% ========================================== =========================
%
% See also CONTOURDFT, VIEWMTX
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se

if nargin < 1
   help contoursig
   return
end;

if nargin < 2
   n=5;
end;

if nargin < 3
   d=2;
end;

if nargin < 4
   viewmatrix=viewmtx(-37.5,30,0);
end;

[rows,cols]=size(sigarray);

if (rows == 1)|(cols == 1)
   fprintf(1,'  ERROR! Only two dimensional arrays!!!!!!!!!!!!')
   help contoursig
   return
end;

x=sigplane([rows cols]);
y=imag(x);
x=real(x);

if (min(min(imag(sigarray)))==0)&(max(max(imag(sigarray)))==0)
   z=sigarray;
else
   z=abs(sigarray);
end

global SD
figure(SD)
subplot(1,1,1)
if d==2
   contour(x,y,z,n);
   axis('xy')	
   axis('square')
   xlabel('Horizontal Position')   
   ylabel('Vertical Position')   
else
   contour3(x,y,z,n);
   axis('xy')
   axis('square')
   view(viewmatrix);
   xlabel('Horizontal Position')   
   ylabel('Vertical Position')   
   zlabel('Magnitude')
   view(viewmatrix)
end   
grid;
out=sigarray;


