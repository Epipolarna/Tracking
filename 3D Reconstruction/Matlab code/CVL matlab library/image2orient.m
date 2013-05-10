function z = image2orient(Im, fsz, th)

% z = image2orient(Im, th)
%
% Calculates an orient image z in double angle
% representation from a gray-level 2D image Im. 
%
%  Im - MxN matrix
% fsz - size of orient filter
%       (default fsz = 7)
%  th - sensitivity threshold (relative to max energy)
%       (default th = 0.05*'max energy')
%
%   z - MxN matrix of complex numbers
%
%
%
% bjojo 991103

if nargin<2,
    fsz = 7;
end

%------------------------------------
%----- Calculate orient tensors -----
%------------------------------------

T = make_tensors_fast(Im, fsz);

T = tensor_aver(T,max(5,fsz/2));

% Remove 'false' border responses 
[M N] = size(Im);
dfsz = floor(fsz/2)+1;

T(1:dfsz,:,:,:) = 0;
T(M-dfsz+1:M,:,:,:) = 0;
T(:,1:dfsz,:,:) = 0;
T(:,N-dfsz+1:N,:,:) = 0;

%-----------------------------------
%----- tensors -> double angle -----
%-----------------------------------

T1 = squeeze(T(:,:,1,1));
T3 = squeeze(T(:,:,2,2));
T2 = squeeze(T(:,:,1,2));

%----- Calculate eigenvalues ----------

sqrnormT = T1.*T1 + T3.*T3 + 2*T2.*T2;
normT = sqrt(sqrnormT);

if nargin<3,
    th = 0.05*max(max(normT));
end
if th<0, th = 0; end

detT = T1.*T3 - T2.*T2;

a = abs(sqrt(sqrnormT + 2*detT));
b = abs(sqrt(sqrnormT - 2*detT));

l1 = (a+b)/2;
l2 = (a-b)/2;

%----- Calculate certainty measure -----

absz = (abs(l1)-abs(l2))./(abs(l1)+eps);
absz = absz.*(l2>=0).*(l1>=0);   % if negative eigenvalue then something 
                                 % is wrong -> certainty = 0
absz = absz.^2;
absz = absz.*(1-exp(-normT/th));

%absz = abs(l1)-abs(l2);

%----- Calculate eigenvector with largest eigenvalue ----------

% T - l2*I = (l1-l2)*[e1*e1']

e1x = sqrt(T1 - l2);  % (e1x)^2 = k*[e1*e1']*[(1,0)*(1,0)']
e1y = sqrt(T3 - l2);  % (e1y)^2 = k*[e1*e1']*[(0,1)*(0,1)']
 
argz = angle(e1x+i*e1y);
argz = argz.*(sign(T2)>=0) + (pi-argz).*(sign(T2)<0);

%----- Calculate double angle ----------

z = absz.*exp(i*2*argz);

z = -z;  % ad hoc...


