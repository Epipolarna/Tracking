function RS = orient2rotsym(z, fsz)

% RS = orient2rotsym(z, fsz)
%
% Calculates rotational symmetry responses RS from an orient image z in
% double angle representation. The responses are made more selective
% using lateral inhibition.
%
% The filters are made separable using SVD (four 1D-filters for zeroth-
% and first order symmetries and six 1D-filters for second order).
%
%    z - M/N matrix of complex numbers
%  fsz - feature size
%
%   RS - M/N/3 matrix where RS(:,:,k) is the filter
%        response from the (k-1):th order symmetry filter
%
%
%
% bjojo 991109


[M1 M2] = size(z);

%--- Create signal and certainty ---

c = abs(z)+eps;
zhat = z./c;

%--- Create filters ---

a = 0.5;
s = fsz/2/sqrt(log(a^2)/(1-1/a^2));

dN = ceil(s*sqrt(log(100)));
N = 2*dN+1;

x = ones(N,1)*(-dN:dN);
y = (dN:-1:-dN)'*ones(1,N);;
r = sqrt(x.^2+y.^2);
fi = atan2(y,x);

appl = exp(-r.^2/s^2) - exp(-r.^2/(s*a)^2);

b0 = appl.*exp(i*0*fi);
b1 = appl.*exp(i*1*fi);
b2 = appl.*exp(i*2*fi);

b0 = conj(b0(end:-1:1,end:-1:1));
b1 = conj(b1(end:-1:1,end:-1:1));
b2 = conj(b2(end:-1:1,end:-1:1));

%--- Convolve ---

[u,s,v] = svd(b0);
R_1 = conv2(z, u(:,1), 'same');
R_1 = conv2(R_1, v(:,1)', 'same');
R_2 = conv2(z, u(:,2), 'same');
R_2 = conv2(R_2, v(:,2)', 'same');
R0 = s(1,1)*R_1 + s(2,2)*R_2;

[u,s,v] = svd(b1);
R_1 = conv2(z, u(:,1), 'same');
R_1 = conv2(R_1, v(:,1)', 'same');
R_2 = conv2(z, u(:,2), 'same');
R_2 = conv2(R_2, v(:,2)', 'same');
R1 = s(1,1)*R_1 + s(2,2)*R_2;

[u,s,v] = svd(b2);
R_1 = conv2(z, u(:,1), 'same');
R_1 = conv2(R_1, v(:,1)', 'same');
R_2 = conv2(z, u(:,2), 'same');
R_2 = conv2(R_2, v(:,2)', 'same');
R_3 = conv2(z, u(:,3), 'same');
R_3 = conv2(R_3, v(:,3)', 'same');
R2 = s(1,1)*R_1 + s(2,2)*R_2 + s(3,3)*R_3;

[u,s,v] = svd(appl);
R_1 = conv2(c, u(:,1), 'same');
R_1 = conv2(R_1, v(:,1)', 'same');
R_2 = conv2(c, u(:,2), 'same');
R_2 = conv2(R_2, v(:,2)', 'same');
cout = s(1,1)*R_1 + s(2,2)*R_2 + eps;

S0 = abs(R0)./cout;
S1 = abs(R1)./cout;
S2 = abs(R2)./cout;

RS = zeros(M1,M2,3);
RS(:,:,1) = R0.*(1 - abs(S1)).*(1 - abs(S2));
RS(:,:,2) = R1.*(1 - abs(S0)).*(1 - abs(S2));
RS(:,:,3) = R2.*(1 - abs(S0)).*(1 - abs(S1));

