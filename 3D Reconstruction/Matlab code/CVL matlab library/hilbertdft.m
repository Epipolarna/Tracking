function transform=hilbertdft(dftarray, phi) 
% 
% transform=hilbertdft(dftarray, phi) 
%
% Generate the Hilbert transform in the Fourier domain with 
% -PI <= u <= PI. Phi determines the reference direction for two 
% dimensional arrays.
%
% NOTE! For even sized arrays the point (row,col)=(1+M/2,1+N/2), 
% i.e the lowest rightmost element of the four center elements,
% is considered to correspond to the origin.
% 
% Parameter  Range      Default         Description
% =========  =========  ==============  ============================
% dftarray        -                     Any 1D or 2D array. 
% phi         0 - 2PI        0          Reference direction for 2D.
% ==================================================================
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se

if nargin < 1
   help hilbertdft;
   return
end;

[rows,cols]=size(dftarray);

if nargin < 2
   phi=0;
end

if cols == 1
  phi = pi/2;
elseif rows == 1
  phi = 0;
end

z=dftplane([rows cols]);

transform=i*dftarray.*sign(real(z)*cos(phi)+imag(z)*sin(phi));
