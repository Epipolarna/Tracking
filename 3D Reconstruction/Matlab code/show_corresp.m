%function h=show_corresp(imageLeft, imageRight,lc,rc,cl)
%
% Visualise a list of correspondences by
% drawing a line between the points in
% image1 and in image2.
%
% imageLeft:    Left image
% imageRight:   Right image
% lc:    List of points in image1 [x1(:) y1(:)] or [x1(:) y1(:)]'
% rc:    List of points in image2 [x2(:) y2(:)] or [x2(:) y2(:)]'
% cl:    Correspondence list [ind1 ind2]
%        (default [1 1;2 2;3 3;...])
%
% i.e. points lc(cl(k,1),:) and rc(cl(k,2),:)
% correspond.
%
%Per-Erik Forssen, Nov 2004, johan hedborg, 2013

function h=show_corresp(imageLeft, imageRight, lc, rc, cl)

if nargin < 5
    cl=[1:size(lc,1)]'*[1 1];
end

% transpose point array if ls is [x1(:) y1(:)]'
if size(lc,1) == 2
    lc = lc';
end

% same transpose for rc
if size(rc,1) == 2
    rc = rc';
end


Nc=size(cl,1);

hold off;
image([imageLeft imageRight]);axis image
xoff=size(imageLeft,2);
h=zeros(Nc,1);
hold on
for k=1:Nc,
  li=cl(k,1);
  ri=cl(k,2);
  h(k)=plot([lc(li,1) rc(ri,1)+xoff],[lc(li,2) rc(ri,2)],'ro-');
end

if nargout==0,
  clear('h');
end