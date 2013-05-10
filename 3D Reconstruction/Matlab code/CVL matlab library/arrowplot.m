function out=arrowplot(zarr,headsize,S)
%
% out=arrowplot(zarr,headsize,S) 
%
% Plots complex matrixes as a grid with arrows.
%
% Parameter  Range      Default      Description
% =========  =========  ===========  ======================
% zarray        -           -        Any 2D array. 
% Headsize    0 - inf       0.2      Size of arrowhead.
% S                        w-        Line type and color 
%                                    (see PLOT )
% =========================================================
%
% See also QUIVER
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se


out=zarr;

i=sqrt(-1);

if nargin < 1
  help arrowplot;
  return;
end

if nargin < 2
   headsize=0.2;
end

if nargin < 3
   S='w-';
end

[hgh,wid]=size(zarr);

z1=[];
c1=[];

orirow=(hgh+1)/2;
oricol=(wid+1)/2;

for row=1:hgh        
   for col=1:wid
      c1=[c1 row-orirow+i*(col-oricol)];    % make a referens zarr
      z1=[z1 zarr(row,col)];          % make a vector from matrix 
   end
end

% convert input data to polar
phi=angle(z1+(z1 == 0)); % Note the trick for avoiding: error log of zero
r=abs(z1);

theta=angle(c1+(c1 == 0));

col=abs(c1).*cos(theta);
row=abs(c1).*sin(theta);

cosinus=cos(phi);
sinus=sin(phi);

% define arrowhead
dr=headsize .*r;
dphi=0.9*pi;

row0=row-0.5*r.*cosinus;        % Tail
col0=col-0.5*r.*sinus;

row1=row0+r.*cosinus;  		% Head
col1=col0+r.*sinus;

row2=row1+dr.*cos(phi-dphi);    % one barb
col2=col1+dr.*sin(phi-dphi);

row3=row1+dr.*cos(phi+dphi);	% the other barb
col3=col1+dr.*sin(phi+dphi);

rowplot=[row0 ; row1 ; row2 ; row3 ; row1];
colplot=[col0 ; col1 ; col2 ; col3 ; col1];

plot(rowplot,colplot,S)
axis([-oricol oricol -orirow orirow])






