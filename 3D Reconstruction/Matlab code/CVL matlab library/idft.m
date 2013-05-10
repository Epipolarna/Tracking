function signal=idft(transform, pvec) 
% 
% signal=idft(transform,[M N]) 
%
% Inverse Discrete Fourier Transform with -PI <= u <= PI.
%
% NOTE! For even sized arrays the point (row,col)=(1+M/2,1+N/2), 
% i.e the lowest rightmost element of the four center elements,
% is considered to correspond to the origin.
% 
% Parameter  Range      Default            Description
% =========  =========  =================  ==========================
% Transform     -                          Any 1D or 2D array. 
% M           1 - inf   same as transform  Number of signal points.
% N           1 - inf   same as transform  Number of signal points.
% ===================================================================
%
% See also DFT
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se

if nargin < 1
   help idft;
   return
end;

[rows,cols]=size(transform);

if nargin < 2
   nrows=rows;
   ncols=cols;
else
   [rs cs] = size(pvec);
   if ((rs == 1) & (cs == 2)) | ((rs == 2) & (cs == 1))
      nrows=pvec(1);
      ncols=pvec(2);
   elseif (rs == 1) & (cs == 1)
      nrows=pvec;
      ncols=pvec;
       else
          fprintf('Wrong dimension on the signal size vector')
	  pvec
	  help dft
          return
       end
end

if nrows < rows
   nrows=rows;
end;

if ncols < cols
   ncols=cols;
end;

hr=floor(rows/2);
hc=floor(cols/2);

if cols == 1
   signal = ...
   fftshift(ifft( ...
   [transform(hr+1:rows);zeros(1,nrows-rows);transform(1:hr)],nrows));
elseif rows == 1
      signal = ...
      fftshift(ifft( ...
      [transform(hc+1:cols) zeros(1,ncols-cols) transform(1:hc)],ncols));
    else
      signal = ...
      fftshift(ifft2(...
      [transform( hr+1:rows,hc+1:cols) zeros(rows-hr,ncols-cols) ...
      transform(hr+1:rows,1:hc);...
      zeros(nrows-rows,ncols);...
      transform(1:hr,hc+1:cols)  zeros(hr,ncols-cols) ...
      transform(1:hr,1:hc)],nrows,ncols));
end
