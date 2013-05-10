%function [Fa,Fb,Fc]=fmatrix_7point(Xl,Xr)
%
% Estimate fundamental matrices that relate Xl to Xr using 
% seven point algorithm.
%
%  i.e. Xl'*F*Xr=0 and det(l*F1+(1-l)*F2)=0
%
% 1. Estimate F1 and F2 using SVD
% 2. Solve cubic polynomial
% 3. Generate three possible F
%
% XL   Left  image coordinate list (2xN) [x; y]
% XR   Right image coordinate list (2xN) [x; y]
%
%
%Per-Erik Forssen, Oct 2003

function [Fa,Fb,Fc]=fmatrix_7point(Xl,Xr)
  
% Solve for coeffs in F where Xl'*F*Xr=0

npts=size(Xl,2);  % Number of points

X=Xl(1,:)';
Y=Xl(2,:)';
x=Xr(1,:)';
y=Xr(2,:)';

% Generate matrix A for which A*vec(F)=0

A=[X.*x X.*y X Y.*x Y.*y Y x y ones(npts,1)];
[U,D,V]=svd(A);

F1=reshape(V(:,8),[3 3])';  % F1 is column-catenated
F2=reshape(V(:,9),[3 3])';  % F2 is column-catenated

% F = l*F1+(1-l)*F2
% a*l^3+b*l^2+c*l+d=0

d110=diag([1 1 0]);
d101=diag([1 0 1]);
d011=diag([0 1 1]);
d100=diag([1 0 0]);
d010=diag([0 1 0]);
d001=diag([0 0 1]);

G=F1-F2;
a=det(G);
b=det(d110*G+d001*F2)+det(d101*G+d010*F2)+det(d011*G+d100*F2);
c=det(d001*G+d110*F2)+det(d010*G+d101*F2)+det(d100*G+d011*F2);
d=det(F2);

% Solve polynomial
%l=roots([a b c d]);
l=eig([0 0 -d/a;1 0 -c/a;0 1 -b/a]);
l=l(imag(l)==0);

% Verify with: a*l.^3+b*l.^2+c*l+d

Fa=l(1)*G+F2;
if length(l)>1,
  Fb=l(2)*G+F2;
  Fc=l(3)*G+F2;
else
  Fb=0;
  Fc=0;
end


