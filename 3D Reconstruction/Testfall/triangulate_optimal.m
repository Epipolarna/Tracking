%function X=triangulate_optimal(C1,C2,x1,x2)
%
% C1,C2  Camera projection matrices 3x4
% X1,X2  a point pair in the two images 2x1
%
%Per-Erik Forssen, March 2008

function X=triangulate_optimal(C1,C2,x1,x2)


% Transformation that moves projected points to origin
T1i=[1 0 x1(1);0 1 x1(2);0 0 1];
T2i=[1 0 x2(1);0 1 x2(2);0 0 1];

% Find and transform F
F=fmatrix_from_cameras(C1,C2);
F=T1i'*F*T2i;

% Extract epipoles
[e1,e2]=fmatrix_epipoles(F);

% Normalise epipoles as lines for rotation matrix construction
e1=e1/sqrt(e1(1).^2+e1(2).^2);
e2=e2/sqrt(e2(1).^2+e2(2).^2);

R1=[e1(1) e1(2) 0;-e1(2) e1(1) 0;0 0 1];
R2=[e2(1) e2(2) 0;-e2(2) e2(1) 0;0 0 1];

% Test: R1*e1, R2*e2

F=R1*F*R2';

f1=e1(3);
f2=e2(3);
a=F(2,2);
b=F(2,3);
c=F(3,2);
d=F(3,3);

% Build polynomial, code from Klas Nordberg
k1=b*c-a*d;

g=[a*c*k1*f2^4 ...                        %coefficient for t^6
  (a^2+c^2*f1^2)^2+k1*(b*c+a*d)*f2^4 ... %coefficient for t^5
  4*(a^2+c^2*f1^2)*(a*b+c*d*f1^2)+2*a*c*k1*f2^2+b*d*k1*f2^4 ...
  2*(4*a*b*c*d*f1^2+a^2*(3*b^2+d^2*(f1-f2)*(f1+f2))+c^2*(3*d^2*f1^4+b^2*(f1^2+f2^2))) ...
  -a^2*c*d+a*b*(4*b^2+c^2+4*d^2*f1^2-2*d^2*f2^2)+2*c*d*(2*d^2*f1^4+b^2*(2*f1^2+f2^2)) ...
  b^4-a^2*d^2+d^4*f1^4+b^2*(c^2+2*d^2*f1^2) ... %coefficient for t^1
  b*d*k1];


% Find roots
r = real(roots(g));

% Check each point
s=zeros(length(r)+1,1);

for k=1:length(r),
    t=r(k);
    s(k) = t^2/(1+f2^2*t^2)+(c*t+d)^2/((a*t+b)^2+f1^2*(c*t+d)^2);
end

% Add value at asymptotic point
s(end)= 1/f2^2+c^2/(a^2+f1^2*c^2);

% Sort ascending
[val,ind]=sort(s);

if ( ind(1) <= length(r) ),
    % Not point at infinity
    tmin = r(ind(1));
    l1 = [-f1*(c*tmin+d);a*tmin+b;c*tmin+d];
    l2 = [tmin*f2;1;-tmin];
else
    % Special case: tmin=inf
    l1 = [-f1*c;a;c];
    l2 = [f2;0;-1];
end

% Find closest points to origin
x1new = [-l1(1)*l1(3); -l1(2)*l1(3); l1(1)^2+l1(2)^2];
x2new = [-l2(1)*l2(3); -l2(2)*l2(3); l2(1)^2+l2(2)^2];

% Transfer back to original CS
x1new = T1i*R1'*x1new;
x2new = T2i*R2'*x2new;

% Find 3D point with linear method on new coordinates
X=triangulate_linear(C1,C2,x1new,x2new);
