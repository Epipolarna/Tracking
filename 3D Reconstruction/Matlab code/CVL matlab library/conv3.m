% CONV3
% 
% Perform convolution in 3 dimensions. Only real signals and kernels
% are supported. conv3 is a mex function.
% 
% Call:
% result=conv3(signal,kernel)
% or
% result=conv3(signal,kernel,region_of_interest)
% 
% signal             - signal values
% kernel             - filter kernel
% region_of_interest - where to compute the convolution
% result             - filter output
% 
% Formats:
% signal is a 3-dimensional array.
% kernel is a 3-dimensional array.
% region of interest is an Nx2 matrix, where each row gives start
%         and end indices for each signal dimension. N must be the
%         same as the number of non-trailing singleton dimensions
%         for signal.
% result is a 3-dimensional array. The size is the same as for
%         signal, unless region_of_interest is specified. The
%         signal is assumed to be surrounded by zeros.
% 
% Note 1: Only double, nonsparse arrays are currently supported.
% Note 2: If signal or kernel has fewer than 3 dimensions,
%         trailing singleton dimensions are added.
% Note 3: Actually, the name of this function is misleading since it
%         doesn't do a proper convolution. The kernel is not mirrored.
% 
% 
% Author: Gunnar Farnebäck
%         Computer Vision Laboratory
%         Linköping University, Sweden
%         gf@isy.liu.se
