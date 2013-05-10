function y=strange_function(phase)
% 
% y=strange_function(phase)
%
% Generates a 1D function in the signal domain. The eveness and oddness
% is controlled with the parameter 'phase'. The other parameters
% are secret.
% 
% Parameter  Range      Default      Description
% =========  =========  ===========  =================================
% phase      -pi - pi      0         Phase between even and odd. 
% ====================================================================
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se
%


if nargin < 1
   phase=0;
end

x=sigplane([1 61]);

y=idft(exp(-x*0.2).*(x>0));

y=100*real(abs(y).*exp(i*(angle(y)+phase)));
