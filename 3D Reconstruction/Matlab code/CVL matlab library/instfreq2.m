function [freq, c_freq] = instfreq(ql, qr, freqlim);
% [freq, c_freq] = instfreq(ql, qr, freqlim);
% 
% freq  : local frequency                        (real)
% cfreq : certainty of local frequency           (real)
% ql    : quadrature responce from left image    (complex)
% qr    : quadrature responce from right image   (complex)
%
% Estimation of local frequency using a combination of left and right
% phase derivates of the quadrature filter responces ql and qr.
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


%freq = angle(fl_pos.*(angle(fl_pos)>0) + fl_neg.*(angle(fl_neg)>0) + ...
%             fr_pos.*(angle(fr_pos)>0) + fr_neg.*(angle(fr_neg)>0));
%
%lpos = unwrap(angle(ql(:,2:xsize,:))-angle(ql(:,1:xsize-1,:)));
%lpos(:,xsize) = 0;
%lneg(:,2:xsize,  1:scalesize)  = unwrap(angle(...
%                                 ql(:,2:xsize,:))-angle(ql(:,1:xsize-1,:)));
%lneg(:,1) = 0;
%
%rpos = unwrap(angle(qr(:,2:xsize,:))-angle(qr(:,1:xsize-1,:)));
%rpos(:,xsize) = 0;
%rneg(:,2:xsize,  1:scalesize)  = unwrap(angle(...
%                                 qr(:,2:xsize,:))-angle(qr(:,1:xsize-1,:)));
%rneg(:,1) = 0;
%
%f = (lpos+lneg +rpos+rneg)/4;

