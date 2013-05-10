function result=normconv(signal,certainty,basis,applicability)
% RESULT=NORMCONV(SIGNAL,CERTAINTY,BASIS,APPLICABILITY)
% or
% RESULT=NORMCONV(SIGNAL,CERTAINTY,BASIS,APPLICABILITY,REGION_OF_INTEREST)
% 
% Perform normalized convolution. Only real images and basis functions are
% supported, but at arbitrary dimensionality.
% 
% SIGNAL             - signal values
% CERTAINTY          - signal certainty (pointwise)
% BASIS              - subspace basis
% APPLICABILITY      - applicability function for the basis
% REGION_OF_INTEREST - where to compute the normalized convolution
% RESULT             - local basis coefficients
% 
% Formats: (Also see note 2 below for special cases.)
% SIGNAL is an n-dimensional array.
% CERTAINTY is the same size as signal.
% BASIS is an (n+1)-dimensional array. The first n dimensions correspond
%         to the signal dimensions. The size of the last dimension gives
%         the number of basis functions.
% APPLICABILITY is an n-dimensional array, with the same size as the first
%         n dimensions of basis.
% REGION_OF_INTEREST is an nx2 matrix, where each row gives start
%         and end indices for each dimension.
% RESULT is an (n+1)-dimensional array. The size of the first n dimensions
%         are the same as for signal, unless region_of_interest is
%         specified. The size of the last dimension equals the number of
%         basis functions.
% 
% Note 1: Only double, nonsparse arrays are currently supported.
% Note 2: Trailing singleton dimensions cannot exist in matlab 5.
%         If there is a mismatch in the number of dimensions of the
%         parameters it will be assumed that there are additional
%         singleton dimensions, if that turns out to make sense.
%         Particularly, in the case of a single basis function, basis
%         will normally be n-dimensional instead of (n+1)-dimensional.
% 
%         In the case of column vectors, the second singleton dimension
%         will be ignored, although it is included by matlab 5.
% Note 3: This implementation is quite general and therefore less
%         efficient than would be possible
%
% Author: Gunnar Farnebäck
%         Computer Vision Laboratory
%         Linköping University, Sweden

error('NORMCONV is implemented as a mex-file. It does not seem to be available on this platform.')
