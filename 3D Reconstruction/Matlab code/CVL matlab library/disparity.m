function [d, ctot] = disparity(ql, qr, scale);
% [d, ctot] = disparity(ql, qr, scale);
%
% Evaluates local phase differences, local frequency and disparity
%
% d     : disparity                            (real)
% ctot  : total certainty for disparity        (real)
% ql    : left quadrature signal               (complex)
% qr    : right quadrature signal              (complex)
% scale : scale parameter for local frequency limitations
%       : 1,..,N where finest scale is 1.      (real)
%
% Certainty ctot is a combination of
% * c1 : Magnitude difference between ql and qr, (energy dependent)
% * c2 : cos2(angle(qr) - angle(ql))  -> suppress near wrap around
% * c3 : Instant frequency variations [ {wl+, wl-, wr+, wr-} ]
% * c4 : Instant frequency > w0 (scale dependent)
%
% Methods from Carl-Johan Westelius CVL 1995
%
% Morgan Ulvklo
% FOA 3 & CVL
% 971109

freqlim = 0.1/2^(scale-1);     % Supress region with too low frequency
[freq, c_freq] = instfreq(ql, qr, freqlim);        % Instant frequency
% The certainty c_freq is c3.*c4

zlr = qr.*conj(ql);            
deltaphase = angle(zlr);       % Phase difference between right and left image
c1 = (abs(zlr)).^0.5.*(2*abs(zlr)./(abs(ql).^2 + abs(qr).^2 + eps)).^4; 
c2 = cos(deltaphase/2).^2;     % No wrap around please
ctot = c_freq.*c1.*c2;         % Energy dependent total certainty

d  = deltaphase./(freq+eps);   % Disparity estimate


