%function r=fmatrix_residuals(F,Xt,Xi)
%
% Compute the residuals of a set of points
% Wrt the fundamental matrix F.
% i.e. the signed distances to the epipolar lines
% in both images:
%
%    r(k) = [diff(Xt(k),F*Xi(k)); diff(Xi(k),Xt(k)'*F)]
%
% XT     2xN 
% XI     2xN point lists
%
%Per-Erik Forssen, Oct 2003

function r=fmatrix_residuals(F,Xt,Xi)

npts=size(Xi,2);

Xih=[Xi; ones(1,npts)];
Xth=[Xt; ones(1,npts)];

l1=F*Xih;
l2=F'*Xth;

%l1s=l1(1,:).^2+l1(2,:).^2;
%l2s=l2(1,:).^2+l2(2,:).^2;

%r=sqrt(sum(l1.*Xth,1).^2./l1s+sum(l2.*Xih,1).^2./l2s);

l1s=sqrt(l1(1,:).^2+l1(2,:).^2);
l2s=sqrt(l2(1,:).^2+l2(2,:).^2);

r=[sum(l1.*Xth,1)./l1s;sum(l2.*Xih,1)./l2s];
