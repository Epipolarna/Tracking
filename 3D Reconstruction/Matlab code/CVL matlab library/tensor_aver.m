function Taver = tensor_aver(T,sz)

% Taver = tensor_aver(T,sz)
%
% Avering of tensors
%
%
%
% bjojo 991102

dsz = floor(sz/2);
sz = 2*dsz+1;
x = (-dsz:dsz);

sigma = dsz/sqrt(log(100));
gx = exp(-x.^2/sigma^2);

Taver = zeros(size(T));

Taver(:,:,1,1) = conv2(T(:,:,1,1), gx, 'same');
Taver(:,:,1,1) = conv2(Taver(:,:,1,1), gx', 'same');

Taver(:,:,1,2) = conv2(T(:,:,1,2), gx, 'same');
Taver(:,:,1,2) = conv2(Taver(:,:,1,2), gx', 'same');

Taver(:,:,2,1) = conv2(T(:,:,2,1), gx, 'same');
Taver(:,:,2,1) = conv2(Taver(:,:,2,1), gx', 'same');

Taver(:,:,2,2) = conv2(T(:,:,2,2), gx, 'same');
Taver(:,:,2,2) = conv2(Taver(:,:,2,2), gx', 'same');

Taver = Taver/sum(sum(conv2(gx,gx')));