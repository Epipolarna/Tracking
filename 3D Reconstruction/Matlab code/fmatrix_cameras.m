%function [C1,C2]=fmatrix_cameras(F)
%
% Compute two possible cameras from the fundamental matrix F.
%
% C1=[(e')x*F e'] C2=[I 0] 
%
% Result 9.14 in H&Z
%
%Per-Erik Forssen, March 2008

function [C1,C2]=fmatrix_cameras(F)

[U,D,V]=svd(F);

e1=U(:,3);
%e2=V(:,3);

C1=[cross_matrix(e1)*F e1];
C2=[eye(3) zeros(3,1)];
