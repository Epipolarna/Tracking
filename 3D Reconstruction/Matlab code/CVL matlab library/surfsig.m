function out=surfsig(sigarray, mthresh, viewmatrix)
% 
% out=surfsig(sigarray, mthresh,viewmatrix) 
%
% Surfsig interprets a 2D array as a spatial signal representation.
% For complex valued arrays, the surface height corresponds to the 
% magnitude and the color corresponds to the argument. Real valued
% array surfaces have a signed height and the color is green for
% positive and red for negative values. 
%
% Values below mthresh are not included.
% 
% A global variable SD is expected to define which figure window to
% use.
%
% NOTE! For even sized arrays the point (row,col)=(1+M/2,1+N/2), 
% i.e the lowest rightmost element of the four center elements,
% is considered to correspond to the origin.
% 
% Parameter  Range      Default     Description
% =========  =========  ==========  ==================================
% sigarray      -           -       Any 2D array. 
% mthresh       -          -Inf     Threshold value.
% viewmatrix    -           -       Viewpoint spec. see Surf
% ====================================================================
%
% See also SURFDFT, VIEWMTX
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se

out=[];

if nargin < 1
   help surfsig
   return
end;

[rows,cols]=size(sigarray);

if (rows == 1)|(cols == 1)
   fprintf(1,'  ERROR! Only two dimensional arrays!!!!!!!!!!!!')
   help surfsig
   return
end;

if nargin < 2
   mthresh= -Inf;
end;

if nargin < 3
   viewmatrix=viewmtx(-37.5,30,0);
end;

hs=floor(cols/2);
if 2*hs < cols
   x=-hs:hs; 
else
   x=-hs:hs-1;
end

hs=floor(rows/2);
if 2*hs < rows
   y=-hs:hs; 
else
   y=-hs:hs-1;
end

if (min(min(imag(sigarray)))==0)&(max(max(imag(sigarray)))==0)
   z=sigarray;
else
   z=abs(sigarray);
end

c=angle(sigarray);
c=c+2*pi*(c < 0);

[tx,ty]=find(z<mthresh);
for i=1:length(tx)
   c(tx(i),ty(i))=NaN; %remove thresholded points
end

global SD
figure(SD)
subplot(1,1,1)
surf(x,y,z,c);
axis([x(1) x(cols) y(1) y(rows)]);
axis('xy')
axis('square')
caxis([0 2*pi])
xlabel('Horizontal Position')   
ylabel('Vertical Position')   
zlabel('Magnitude')
view(viewmatrix);
out=sigarray;

