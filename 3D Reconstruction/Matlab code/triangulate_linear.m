%function X=triangulate_linear(C1,C2,x1,x2)
%
% Linear 3D triangulation
%
%Per-Erik Forssen, March 2008

function X=triangulate_linear(C1,C2,x1,x2)

%x1~C1X  x2~C2X


% cross_matrix(x1)*C1*X=0

M=[cross_matrix(x1)*C1; cross_matrix(x2)*C2];

[U,D,V]=svd(M);
X=V(:,4);
