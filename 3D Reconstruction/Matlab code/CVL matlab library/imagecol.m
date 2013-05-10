%function h=imagecol(img,stretch_flag)
%
% Displays the RGB image IMG
% in current figure.
% If the optional argument STRETCH_FLAG
% is non-zero the image will be contrast
% stretched before it is displayed.
%
%Per-Erik Forssen, 1998

function h=imagecol(img,stretch_flag)

if nargin==1
    stretch_flag=0;
end;

subplot(1,1,1)
colormap('default')
if stretch_flag
    imagesc(img)
else
    image(img)
end
axis image
