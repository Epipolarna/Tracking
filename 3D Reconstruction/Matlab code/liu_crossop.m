function x=liu_crossop(y),

% For y that is a 3D vector: liu_crossop returns the cross product operator
% corresponding to y, i.e., liu_crossop(y)*z is the same as cross(y,z).  In
% this case liu_crossop(y) is always an anti-summetric 3x3 matrix
%
% For y that is a 3x3 matrix: liu_crossop returns the
% corresponding 3D vector x such that liu_crossop(x)=(y-y')/2.
%
% For y that is either a 3D column vector or a 3x3 anti-symmetric matrix it
% is the case that liu_crossop(liu_crossop(y))=y.

s=size(y);

if (prod(s)==3),
  x=[0 -y(3) y(2);
     y(3) 0 -y(1);
     -y(2) y(1) 0];
  return
end

if (prod(s)==9),
  x=[y(3,2)-y(2,3);y(1,3)-y(3,1);y(2,1)-y(1,2)]/2;
  return
end

error('liu_crossop: argument is not 3x1, 1x3, or 3x3');

end
