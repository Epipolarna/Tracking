function [H,dIm] = harris(Im,fsz,std_g)

% [H,dIm] = harris(Im,fsz,std_g)
% 
% Detects feature points (e.g. corners) using Harris (Plessey) feature
% detector. 
%
%    Im - M/N matrix
%   fsz - Approximate feature size (2*std of a Gaussian)
% std_g - Gradient scale (default std_g = 1)
%
%     H - M/N matrix where high values indicate feature points
%   dIm - M/N/2 matrix with image gradients in Matlabs own
%         coordinate system
%
%
% H is calculated as
%                                  2
%           H = det(A) - 0.04*trace (A)
%
% where A is a local orientation tensor, calculated as
%                           T
%           A = w * [dIm dIm ], w = gaussian with standard
%                                   deviation fsz/2
%
% (an average of the outer product of the image gradient with itself)
% The gradient image dIm is calculated using derivative filters
%
%          x*g, y*g    where g is a Gaussian with standard deviation
%                      std_g
%
%
%
% Author: Björn Johansson
%         Computer Vision Laboratory
%         Linköping University, Sweden
%         bjorn@isy.liu.se
% 
% autumn 1998

if nargin<3,
    std_g = 1;
end
delta = 0.005;

% Create gradient filter

dN = ceil(std_g*sqrt(-2*log(delta)));

x = -dN:dN;
gx = exp(-0.5*(x/std_g).^2)/sqrt(2*pi)/std_g;
dx = x.*gx;

% Calculate gradient image 
%    dIm = [dImx; dImy]

dImx = conv2(conv2(double(Im), dx, 'same'), gx', 'same');
dImy = conv2(conv2(double(Im), gx, 'same'), dx', 'same');

% Create feature filter

std_f = fsz/2;
dN = ceil(std_f*sqrt(-2*log(delta)));

x = -dN:dN;
gx = exp(-0.5*(x/std_f).^2)/sqrt(2*pi)/std_f;

% Calculate feature tensor A
%                   T
%    A = w * (dIm dIm )   where w is a 2D-gauss

A11 = dImx.^2;
A12 = dImx.*dImy;
A22 = dImy.^2;

A11 = conv2(conv2(double(A11), gx, 'same'), gx', 'same');
A12 = conv2(conv2(double(A12), gx, 'same'), gx', 'same');
A22 = conv2(conv2(double(A22), gx, 'same'), gx', 'same');

% Calculate Harris value

k = 0.04;
detA = A11.*A22 - A12.^2;
traceA = A11+A22;

H = detA - k*traceA.^2;
H = H.*(H>0);

% Cut edge
H(:,[1:dN end-dN+1:end]) = 0;
H([1:dN end-dN+1:end],:) = 0;

if nargout>1
  dIm = zeros([size(Im) 2]);
  dIm(:,:,1) = -dImy;
  dIm(:,:,2) = -dImx;
end
