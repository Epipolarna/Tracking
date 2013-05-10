function [dx, dy, Ig, Jg] = gradient_function(I, J, sigma, kernelsize)

[x,y] = meshgrid(-(kernelsize-1)/2:(kernelsize-1)/2);

dxg = -x/sigma^2 .* 1/(2*pi*sigma^2) .* exp(-(x.^2 + y.^2)/(2*sigma^2));
dyg = -y/sigma^2 .* 1/(2*pi*sigma^2) .* exp(-(x.^2 + y.^2)/(2*sigma^2));
g =                 1/(2*pi*sigma^2) .* exp(-(x.^2 + y.^2)/(2*sigma^2));

%figure;surf(x,y,g);figure;surf(x,y,dyg);figure;
%sum(dxg(:))

dx = conv2(J, dxg, 'same');
dy = conv2(J, dyg, 'same');
Ig = conv2(I, g, 'same');
Jg = conv2(J, g, 'same');