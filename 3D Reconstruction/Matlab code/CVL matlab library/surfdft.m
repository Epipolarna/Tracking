function out=surfdft(dftarray, mthresh, viewmatrix) 
% 
% out=surfdft(dftarray, mthresh, viewmatrix) 
%
% Surfdft interprets a 2D array as a frequency representation with 
% -PI < u < PI for odd sized arrays and -PI <= u < PI for even sized
%  arrays. 
%
% For complex valued arrays, the surface height corresponds to the 
% magnitude and the color corresponds to the argument. Real valued
% array surfaces have a signed height and the color is green for
% positive and red for negative values. 
%
% Values below mthresh are not included.
% 
% A global variable FD is expected to define which figure window to 
% use.
%
% NOTE! For even sized arrays the point (row,col)=(1+M/2,1+N/2), 
% i.e the lowest rightmost element of the four center elements,
% is considered to correspond to the origin.
%
% Parameter  Range      Default      Description
% =========  =========  ===========  =================================
% dftarray      -           -        Any 2D array. 
% mthresh       -          -Inf      Threshold value.
% viewmatrix    -           -        Viewpoint spec. see Surf
% ====================================================================
%
% See also SURFSIG, VIEWMTX
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se

out=[];

if nargin < 1
   help surfdft
   return
end;

[rows,cols]=size(dftarray);

if (rows == 1)|(cols == 1)
   fprintf(1,'  ERROR! Only two dimensional arrays!!!!!!!!!!!!')
   help surfdft
   return
end;

if nargin < 2
   mthresh= -Inf;
end;

if nargin < 3
   viewmatrix=viewmtx(-37.5,30,0);
end;

step=2*pi/cols;

if 2*floor(cols/2) < cols
   x=-(pi-step/2):step:(pi-step/2); 
else
   x=-pi:step:pi-step; 
end

step=2*pi/rows;

if 2*floor(rows/2) < rows
   y=-(pi-step/2):step:(pi-step/2); 
else
   y=-pi:step:pi-step; 
end

if (min(min(imag(dftarray)))==0)&(max(max(imag(dftarray)))==0)
   z=dftarray;
else
   z=abs(dftarray);
end

c=angle(dftarray);
c=c+2*pi*(c < 0);

[tx,ty]=find(z<mthresh);
for i=1:length(tx)
   c(tx(i),ty(i))=NaN; %remove thresholded points
end

global FD
figure(FD)
h=subplot(1,1,1);
surf(x,y,z,c);
axis([-pi pi -pi pi]);
axis('xy')
axis('square')
caxis([0 2*pi])
set(h,'XTick',[-3.14 -2.36 -1.57 -0.79 0 0.79 1.57 2.36 3.14])	
set(h,'YTick',[-3.14 -2.36 -1.57 -0.79 0 0.79 1.57 2.36 3.14])	
xlabel('Horizontal Frequency')   
ylabel('Vertical Frequency')   
zlabel('Magnitude')
view(viewmatrix);
out=dftarray;

