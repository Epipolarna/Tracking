function filter=binomsig(alpha)
% 
% filter=binomsig(alpha) 
%
% Generates a spatial representation of a binomial coefficient
% approximation of a quadrature filter. 
%
%
% Parameter  Range      Default      Description
% =========  =========  ===========  =================================
% alpha       0 - 1        0.3660    filt= alpha*(-d2x)+i*(1-alpha)*dx 
% ====================================================================
%
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se


if nargin < 1
   alpha=0.3660; % Optimal
end

if alpha > 1.0 
   alpha=1.0
end

if alpha < 0.0 
   alpha=0.0
end

d2x=[  1   0  -2   0   1];
dx= [-eps -1   0   1  eps];

filter= -alpha.*d2x+i*(1-alpha).*dx;

filter= filter/sqrt(sum(abs(filter.^2)));




