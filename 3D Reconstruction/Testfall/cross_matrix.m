%function Ex=cross_matrix(e)
%
%  Generate cross-product matrix Ex
%  from vector e, such that:
%
%    cross(e,x)=Ex*x
%
%Per-Erik Forssen, March 2008
function Ex=cross_matrix(e)

Ex=[0 -e(3) e(2);e(3) 0 -e(1);-e(2) e(1) 0];
