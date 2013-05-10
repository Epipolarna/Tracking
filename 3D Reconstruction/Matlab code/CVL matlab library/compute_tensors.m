function T = compute_tensors(sequence, width, sigma)
% COMPUTE_TENSORS - Compute 3D tensors for the middle frame of a sequence.
% 
% Input parameters:
% sequence         -  Image sequence represented by a 3D array. If the
%                     size is MxNxD, M is the height, N the width, and
%                     D the number of frames in the sequence.
% width            -  Width of the filters used to compute the tensors.
%                     Must be odd.
% sigma (optional) -  Standard deviation of the gaussian applicability.
%
% Output parameters:
% T          -  Tensor field of size MxNx3x3.
%
% Description:
% The tensor field is computed for the middle frame of the sequence, i.e.
% frame number (D+1)/2, unless D is even when it's computed for frame D/2.
% The tensors are computed by an algorithm designed in the spatial domain
% using normalized convolution and a Gaussian applicability. The sigma
% parameter can be used to vary the standard deviation of the
% applicability and thus the scale of the structures for which the
% orientation is estimated. It's important that sigma isn`t too large
% compared to width, in which case the applicability will be truncated,
% causing a directional bias in the estimated tensors. If sigma is omitted
% it will be set so that the applicability has the value 0.005 for the
% outermost pixel along an axis.
%
% Please notice that the computation has been optimized for speed. This
% means that the certainty has been set to 1 everywhere, including off
% the edges. Thus there will usually be severe border effects within a
% distance of width/2 from the edges. This also applies along the time
% dimension. Never use larger filters than the length of the sequence.
%
% All convolutions are fully separable, causing the computational
% complexity to be proportional to the number of pixels in a frame and 
% the width of the filters.
%
% Author: Gunnar Farnebäck
%         Computer Vision Laboratory
%         Linköping University, Sweden
%         gf@isy.liu.se

if nargin<2
    error('Missing arguments.')
end

if nargin>3
    error('Too many arguments.')
end

if nargout>1
    error('Too many output arguments.')
end

if mod((width+1)/2, 1) ~= 0
    error('Width must be an odd integer.')
end

if nargin<3
    sigma = ((width-1)/2) / sqrt(-2*log(0.005));
end

if ndims(sequence) ~= 3
    error('Sequence must be a 3D array.')
end

sides = size(sequence);
if width > sides(3)
    warning('Filter width is larger than the length of the sequence.')
end

if exp(-((width-1)/2)^2 / (2*sigma^2)) > 0.05
    warning('Applicability seems to be severely truncated. Consider increasing width or decreasing sigma.')
end

midframe = floor((sides(3)+1)/2);
roi = [1 sides(1);1 sides(2);midframe midframe];  % Region of interest

options.sigma = sigma;
T = make_tensors_fast(sequence, width, roi, options);

% Transform tensors to compacted six components representation.
%T = T(:,:,[1 4 5 7 8 9]);
