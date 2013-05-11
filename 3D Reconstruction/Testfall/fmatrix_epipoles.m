%function [e1,e2]=fmatrix_epipoles(F)
%
% Extract left and right epipoles from
% Fundamental matrix F.
%
%Per-Erik Forssen, March 2008

function [e1,e2]=fmatrix_epipoles(F)

[U,D,V]=svd(F);
e1=U(:,3);
e2=V(:,3);

e1=e1/e1(3);
e2=e2/e2(3);
