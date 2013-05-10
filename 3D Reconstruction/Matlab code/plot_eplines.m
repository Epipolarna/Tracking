%function h=plot_eplines(F,P,imbox)
%
% The fundamental matrix F defines
% an epipolar line l'*q_h=0 in the other image
% for each point p. i.e. l=F*p_h.
%
% F:         Fundamental matrix defining lines l=F*P
% points:    List of points (Nx2) or (2xN).
% imbox:     image bounding box [yl yh xl xh]
%
%Per-Erik Forssen, Jan 2004

function h = plot_eplines(F, points, imbox)


% transpose points matrix if size is (2xN)
if size(points,1) == 2
    points = points';
end

Nl=size(points,1); % Number of lines

h=zeros(Nl,1);
for k=1:Nl,
  l=F*[points(k,:) 1]';
  m=[0 0 0 0];      % yl yh xl xh
  if(l(1)~=0)
    x1=-(l(2)*imbox(1)+l(3))/l(1);
    if((x1<imbox(4))&(x1>imbox(3))) m(1)=1;end
    x2=-(l(2)*imbox(2)+l(3))/l(1);
    if((x2<imbox(4))&(x2>imbox(3))) m(2)=1;end
  end
  if(l(2)~=0)
    y1=-(l(1)*imbox(3)+l(3))/l(2);
    if((y1<imbox(2))&(y1>imbox(1))) m(3)=1;end
    y2=-(l(1)*imbox(4)+l(3))/l(2);
    if((y2<imbox(2))&(y2>imbox(1))) m(4)=1;end
  end
  switch([8 4 2 1]*m')
   case 12,
    h(k)=plot([x1 x2],[imbox(1) imbox(2)]);
   case 10,
    h(k)=plot([x1 imbox(3)],[imbox(1) y1]);
   case 9,
    h(k)=plot([x1 imbox(4)],[imbox(1) y2]);
   case 6,
    h(k)=plot([x2 imbox(3)],[imbox(2) y1]);
   case 5,
    h(k)=plot([x2 imbox(4)],[imbox(2) y2]);
   case 3,
    h(k)=plot([imbox(3) imbox(4)],[y1 y2]);
   otherwise
    disp('WARNING:Line falls outside image.');
    m
  end
end
h=h(h>0);
