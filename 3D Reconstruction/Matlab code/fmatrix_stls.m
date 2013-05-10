%function F=fmatrix_stls(Xl,Xr)
%
% Estimate fundamental matrix that relates Xl to Xr using scaled TLS.
%  i.e. Xl'*F*Xr=0
%
% 1. Scale data
% 2. Estimate F
% 3. Enforce rank 2
% 4. Undo scaling
%
% XL   Left  image coordinate list (2xN) [x; y]
% XR   Right image coordinate list (2xN) [x; y]
%
%Per-Erik Forssen, Oct 2003

function F=fmatrix_stls(Xl,Xr)
  
% Solve for coeffs in F where Xl'*F*Xr=0
npts=size(Xl,2);  % Number of points

X=Xl(1,:);
Y=Xl(2,:);
Z=ones(1,npts);

x=Xr(1,:);
y=Xr(2,:);
%z=ones(npts,1);

% Generate scaling homography S
Xlm=mean(Xl,2);
l=sqrt(1/2/npts*sum(sum((Xl-Xlm*ones(1,npts)).^2)));
S=[1/l  0  -Xlm(1)/l;
   0   1/l -Xlm(2)/l;
   0    0         1];

% Map X,Y through homography S

Xh=X*S(1,1)+Y*S(1,2)+S(1,3);
Yh=X*S(2,1)+Y*S(2,2)+S(2,3);
X=Xh; % Last row of S is [0 0 1], so h=1
Y=Yh;

% Generate scaling homography T

Xrm=mean(Xr,2);
l=sqrt(1/2/npts*sum(sum((Xr-Xrm*ones(1,npts)).^2)));
T=[1/l  0  -Xrm(1)/l;
   0   1/l -Xrm(2)/l;
   0    0         1];

% Map x,y through homography T

xh=x*T(1,1)+y*T(1,2)+T(1,3);
yh=x*T(2,1)+y*T(2,2)+T(2,3);
x=xh; % Last row of T is [0 0 1], so h=1
y=yh;

% Generate matrix A for which A*vec(F)=0

A=[X'.*x' X'.*y' X' Y'.*x' Y'.*y' Y' x' y' ones(npts,1)];

[U,D,V]=svd(A);

% we don't need U then this will be faster if more than 9 points
% /Johan hedborg 2008
% tic;
% [U,D,V]=svd(A,'econ');
%toc

Fs=reshape(V(:,9),[3 3])';  % Fs is column-catenated

% Enforce rank 2
[U,D,V]=svd(Fs);
D(3,3)=0;
Fs=U*D*V';

% Undo scaling
F=S'*Fs*T;

