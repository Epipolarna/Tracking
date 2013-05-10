function rotated=rotate(signal,phi) 
% 
% rotated=rotate(array,phi) 
%
% Rotates the array phi radians. Values are interpolated between
% nearest neighbors.
%
% NOTE! Rotation of even sized arrays is performed around the point
% (row,col)=(1+m/2,1+n/2) i.e the lowest rightmost element of the
% four center elements.
% 
% Parameter  Range      Default      Description
% =========  =========  ===========  =================================
% array          -                   Any array. 
% phi            -        pi/4       Rotation angle.
% ====================================================================
%
% See also SCALE, CIRCSHIFT
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se

if nargin < 1
   help rotate;
   return
end;

if nargin < 2
  phi=pi/4;
end

[rows,cols]=size(signal);

% Calculate coordinates of where to fetch data
% column=real(z)
% row   =imag(z)

z=sigplane(rows,cols);
z=(1+floor(cols/2))+i*(1+floor(rows/2))+z.*exp(-i*phi);

rl=floor(imag(z));
ra=(1-imag(z)+rl);% interpolation weights

ru=rl+1;
rb=1-ra;

tmp=(rl > 0).*(rl <= rows); %zero unvalid coordinates
rl=rl.*tmp+(1-tmp);
ra=ra.*tmp;

tmp=(ru > 0).*(ru <= rows); %zero unvalid coordinates
ru=ru.*tmp+(1-tmp);
rb=rb.*tmp;

cl=floor(real(z));
ca=(1-real(z)+cl);% interpolation weights

cu=cl+1;
cb=1-ca;

tmp=(cl > 0).*(cl <= cols); %zero unvalid coordinates
cl=cl.*tmp+(1-tmp);
ca=ca.*tmp;

tmp=(cu > 0).*(cu <= cols); %zero unvalid coordinates
cu=cu.*tmp+(1-tmp);
cb=cb.*tmp;

A=ra.*ca;
B=rb.*ca;
C=ra.*cb;
D=rb.*cb;

for r=1:rows,
  for c=1:cols,
    rotated(r,c)=A(r,c)*signal(rl(r,c),cl(r,c))+ ...
                 B(r,c)*signal(ru(r,c),cl(r,c))+ ...
                 C(r,c)*signal(rl(r,c),cu(r,c))+ ...
                 D(r,c)*signal(ru(r,c),cu(r,c));
  end
end

