function [b]= dftop(m,n)
%
%DFTOP generates a dft operator matrix B (nxm) such that the 
%      1D-Fourier transform of a spatial vector x (mx1) is 
%      given by the vector y= B*x (nx1).
%
%       [B]= DFTOP(M,N)
%        M - Length of spatial vector.
%        N - Length of Fourier vector.

% B= xF if xf= I

x= -floor(m/2):floor((m+1)/2)-1;
u= 2*pi/n*(-floor(n/2):floor((n+1)/2)-1);
w= exp(-i.*u'*x);
b= 1/m*w;
end
