function [b]= dftop2D(m,n)
%
%DFT2OP generates a dft operator matrix B such that the 2D-Fourier
%	transform of a spatial vector x (m^2x1) is given by y= B*x 
%       (n^2x1) using the 1D-equivalent dftop.
%
%       [B]= DFT2OP(M,N)
%        M - Square root of spatial vector length. (N^2x1).
%        N - Square root of Fourier vector length. (N^2x1).

% B= xF if xf= I

%b=  zeros(n*n,m*m);	% Init 2D operator (for speed gain)
%I=  eye(m*m,m*m);	% init unit vectors

b1= dftop(m,n);		% 1D operator
b=  kron(b1,b1);	% 2D operator as kron prod of 1D operators

%for j= 1:m*m		% transform each spatial unit vector, put in b2
%	xj= reshape(I(:,j),m,m);	% j:th unit image
%	bj2= b1*(b1*xj.').';		% 2D as 2 1D
%	b(:,j)= reshape(bj2,n*n,1);	% j:th transform as vector
%b(:,j)= reshape(b1*(b1*reshape(I(:,j),m,m).').',n*n,1);	% faster?
%end;
end
