function transform=dft(signal,pvec) 
% 
% transform=dft(signal,[M N]) 
%
% Discrete Fourier Transform with -PI <= u <= PI.
%
% NOTE! For even sized arrays the point (row,col)=(1+M/2,1+N/2), 
% i.e the lowest rightmost element of the four center elements,
% is considered to correspond to the origin.
% 
% Parameter  Range      Default         Description
% =========  =========  ==============  ==============================
% signal        -                       Any 1D or 2D array. 
% M           1 - inf   same as signal  Number of frequency points.
% N           1 - inf   same as signal  Number of frequency points.
% ====================================================================
%
% See also IDFT
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se

if nargin < 1
   help dft;
   return
end;

[rows,cols]=size(signal);

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
          fprintf('Wrong dimension on the dft size vector')
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
   transform = ...
   fftshift(fft( ...
   [signal(hr+1:rows);zeros(1,nrows-rows);signal(1:hr)],nrows));
elseif rows == 1
      transform = ...
      fftshift(fft( ...
      [signal(hc+1:cols) zeros(1,ncols-cols) signal(1:hc)],ncols));
    else
      transform = ...
      fftshift(fft2(...
      [signal( hr+1:rows,hc+1:cols) zeros(rows-hr,ncols-cols) ...
      signal(hr+1:rows,1:hc);...
      zeros(nrows-rows,ncols);...
      signal(1:hr,hc+1:cols)  zeros(hr,ncols-cols) ...
      signal(1:hr,1:hc)],nrows,ncols));
end


