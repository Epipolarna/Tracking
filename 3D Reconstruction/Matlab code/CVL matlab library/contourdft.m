function out=contourdft(dftarray, n, d, viewmatrix) 
% 
% out=contourdft(dftarray, n, d, viewmatrix) 
%
% Contourdft interprets a 2D array as a frequency representation
% with -PI < omega < PI for odd sized arrays and -PI <= omega < PI
% for even sized arrays. The contour lines corresponds to the 
% isovalues of the magnitude of the dft.
%
% A global variable FD is expected to define which figure window to
%  use.
%
% NOTE! For even sized arrays the point (row,col)=(1+M/2,1+N/2), 
% i.e the lowest rightmost element of the four center elements,
% is considered to correspond to the origin.
% 
% Parameter  Range      Default      Description
% =========  =========  ===========  =================================
% dftarray      -           -        Any 2D array. 
% n           0 - inf       5        Number of contour lines.
% d           2 - 3         2        2D or 3D contour plot
% viewmatrix    -           -        Viewpoint spec. see Contour
% ====================================================================
%
% See also CONTOURSIG, VIEWMTX
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se

if nargin < 1
   help contourdft
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


[rows,cols]=size(dftarray);

if (rows == 1)|(cols == 1)
   fprintf(1,'  ERROR! Only two dimensional arrays!!!!!!!!!!!!')
   help contourdft
   return
end;

x=dftplane([rows cols]);
y=imag(x);
x=real(x);
if (max(max(abs(imag(dftarray))))==0)
   z=dftarray;
else
   z=abs(dftarray);
end

global FD
figure(FD)
h=subplot(1,1,1);
if d==2
   contour(x,y,z,n);
   axis('xy')	
   axis('square')	
   set(h,'XTick',[-3.14 -2.36 -1.57 -0.79 0 0.79 1.57 2.36 3.14])
   set(h,'YTick',[-3.14 -2.36 -1.57 -0.79 0 0.79 1.57 2.36 3.14])
   xlabel('Horizontal Frequency')   
   ylabel('Vertical Frequency')   
else
   contour3(x,y,z,n);
   axis('xy')
   axis('square')
   set(h,'XTick',[-3.14 -2.36 -1.57 -0.79 0 0.79 1.57 2.36 3.14])
   set(h,'YTick',[-3.14 -2.36 -1.57 -0.79 0 0.79 1.57 2.36 3.14])
   xlabel('Horizontal Frequency')   
   ylabel('Vertical Frequency')   
   zlabel('Magnitude')
   view(viewmatrix)
end   
grid;
out=dftarray;


