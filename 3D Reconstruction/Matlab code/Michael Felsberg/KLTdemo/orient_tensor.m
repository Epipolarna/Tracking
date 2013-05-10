function orient = orient_tensor(I, sigma, kernelsize)

[R,K] = size(I);

orient = zeros([R K 3]);

[x,y] = meshgrid(-(kernelsize-1)/2:(kernelsize-1)/2);

dxg = -x/sigma^2 .* 1/(2*pi*sigma^2) .* exp(-(x.^2 + y.^2)/(2*sigma^2));
dyg = -y/sigma^2 .* 1/(2*pi*sigma^2) .* exp(-(x.^2 + y.^2)/(2*sigma^2));
g =                 1/(2*pi*sigma^2) .* exp(-(x.^2 + y.^2)/(2*sigma^2));

dx = conv2(I, dxg, 'same');
dy = conv2(I, dyg, 'same');

orient(:,:,1) = dx.^2;
orient(:,:,2) = dx.*dy;
orient(:,:,3) = dy.^2;

orient(:,:,1) = conv2(orient(:,:,1), g, 'same');
orient(:,:,2) = conv2(orient(:,:,2), g, 'same');
orient(:,:,3) = conv2(orient(:,:,3), g, 'same');