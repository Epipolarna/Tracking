function [q,lp] = lpqtower(f, c, quadfilter1, lpfilterx1, lpfiltery1);

% [q, lp] = lpqtower(f, c, quadfilter1, lpfilterx1, lpfiltery1);
%
% 3 resolution levels (octave base) of quadrature responses on image f using
% spread kernels. 
%
% q(:,:,i)    : Quadrature-filter response on lp(:,:,i) where
% lp(:,:,1)   : Highest resolution of f, no lowpassfilter
% lp(:,:,2:3) : Lowpass-filtered version of f.
%
% f           : Real image
% c           : Input certainty           
%               Default: c = ones(size(f))
% quadfilter1 : One dimensional quadrature-filter on level 1 (highest res)
%             : default : lognorm quadrature-filter w=pi/2,b=4
% lpfilterx1  : Lowpass function in x-dim, default gaussian with s=5/pi
% lpfiltery1  : Lowpass function in y-dim, default gaussian with s=1
% 
% Morgan Ulvklo 
% FOA3 & CVL
% 971108

[ys xs]=size(f);

if nargin<2
    c=ones([ys xs]);
end

if nargin<3
   quadfilter1 = idft(lognorm1d(21,pi/2,4));
end

if nargin<4
   lpfilterx1=gaussian([1 11],[0 0],[1 6/pi]);
end

if nargin<5
  lpfiltery1=gaussian([5 1],[0 0],[1.0 1]);
end

[slaskx filter_xsize] = size(lpfilterx1);
[filter_ysize slasky] = size(lpfiltery1);

if (slaskx>1)|(slasky>1)     % Separable filters only
    return
end    

lpfilterx2 = spread(lpfilterx1);         % Zeros inserted
lpfilterx3 = spread(lpfilterx2);
lpfilterx4 = spread(lpfilterx3);

lpfiltery2 = spread(lpfiltery1);
lpfiltery3 = spread(lpfiltery2);
lpfiltery4 = spread(lpfiltery3);

quadfilter2 = spread(quadfilter1);
quadfilter3 = spread(quadfilter2);
quadfilter4 = spread(quadfilter3);

x = floor(filter_xsize/2);                % Border expansion in x-direction
y = floor(filter_ysize/2);                % Border expansion in y-direction
%                                           x-dim            y-dim
lp_f1 = conv2(f.*c,  lpfilterx1, 'full'); % 1*x
lp_f1 = conv2(lp_f1, lpfiltery1, 'full'); % 1*x              1*y           
lp_f2 = conv2(lp_f1, lpfilterx2, 'full'); % 1*x+2*x          1*y
lp_f2 = conv2(lp_f2, lpfiltery2, 'full'); % 1*x+2*x          1*y+2*y

lp_c1 = conv2(c,     lpfilterx1, 'full'); % 1*x
lp_c1 = conv2(lp_c1, lpfiltery1, 'full'); % 1*x              1*y           
lp_c2 = conv2(lp_c1, lpfilterx2, 'full'); % 1*x+2*x          1*y
lp_c2 = conv2(lp_c2, lpfiltery2, 'full'); % 1*x+2*x          1*y+2*y

x1 =  1 + x;                    % Start with inner part of signal -> +1
x2 =  2*x + x1;            
wx  = xs-1;

y1 =  1 + y;                    % Start with inner part of signal -> +1
y2 =  2*y + y1;
wy  = ys-1;

lp2 =lp_f2./(lp_c2 + eps);      % Normalized 
lp1 =lp_f1./(lp_c1 + eps);      

lp(:,:,3) = lp2(y2:y2+wy,x2:x2+wx);        
lp(:,:,2) = lp1(y1:y1+wy,x1:x1+wx);
lp(:,:,1) = f;

q3 = conv2(lp2, quadfilter3, 'same');
q2 = conv2(lp1, quadfilter2, 'same');
q(:,:,1) = conv2(f,   quadfilter1, 'same');

q(:,:,3) = q3(y2:y2+wy,x2:x2+wx);
q(:,:,2) = q2(y1:y1+wy,x1:x1+wx);

