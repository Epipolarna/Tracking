function [freq, c_freq] = instfreq(ql, qr, freqlim);
% [freq, c_freq] = instfreq(ql, qr, freqlim);
% 
% freq  : local frequency                        (real)
% cfreq : certainty of local frequency           (real)
% ql    : quadrature response from left image    (complex)
% qr    : quadrature response from right image   (complex)
%
% Estimation of local frequency using a combination of left and right
% phase derivates of the quadrature filter responses ql and qr.
%
% High certainty corresponds to consistent instant frequency in both images.
% Positions with negativ frequencies achieve zero certainty.
% Implemented method orginates from Carl-Johan Westelius, CVL 1995
%
% Morgan Ulvklo 
% CVL & FOA3
% 971106

if  nargin<3
  freqlim = 0.2;
end

[ysize xsize scalesize] = size(ql);

if xsize==1
    error('The width must be larger than one.')
end

fl_pos(:,1:xsize-1,1:scalesize) = ql(:,2:xsize,:).*conj(ql(:,1:xsize-1,:));
fl_neg(:,2:xsize,  1:scalesize) = fl_pos(:,1:xsize-1,:);
fl_pos(:,xsize,    1:scalesize) = 0;
fl_neg(:,1,        1:scalesize) = 0;

fr_pos(:,1:xsize-1,1:scalesize) = qr(:,2:xsize,:).*conj(qr(:,1:xsize-1,:));
fr_neg(:,2:xsize,  1:scalesize) = fr_pos(:,1:xsize-1,:);
fr_pos(:,xsize,    1:scalesize) = 0;
fr_neg(:,1,        1:scalesize) = 0;

freq = angle(fl_pos + fl_neg + fr_pos + fr_neg);

c_freq = freq.^2./(freq.^2 + freqlim^2).* ...
        abs(1/4 *(exp(i*angle(fl_pos)) + exp(i*angle(fl_neg)) + ...
                  exp(i*angle(fr_pos)) + exp(i*angle(fr_neg)))).^4;

c_freq(freq<=0) = 0;
