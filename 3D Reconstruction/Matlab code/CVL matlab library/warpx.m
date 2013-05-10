function [out_im, c_out] = warpx(im, disp, border, method);

%  [out_im, c_out] = warpx(im, disp, border, method);
%
%  Warps an image IM using the x-disparity image DISP
%
%  out_im :  Warped image with border expansion for out of range positions.
%  c_out  :  Binary certainty for out_image (0 if position is out of range)
%  im     :  Input image                             (real or complex)
%  disp   :  x-disparity image                       (real)
%  border :  'black' for zero padding (default) or 'copy' for border expansion
%  method :  Interpolation type
%            'linear' (default), 'nearest', or 'cubic'
%
%  Morgan Ulvklo  
%  FOA 3 & CVL
%  971109

if nargin <4
  method = 'linear';
end

if nargin <3
  border = 'black';                                    % Zero pading
end

[ysize xsize] = size(im);
odd = ceil(xsize/2)~=floor(xsize/2);


if ysize>1
  xpos=ones([1 ysize])'*[1:xsize]+disp;
  ypos=[1:ysize]'* ones([1 xsize]);
  out_im=interp2(im,xpos, ypos, method);
  mask = isnan(out_im);                                % No Pixel value
  if all(border(1:4)== 'copy')
    borderim(:,1:floor(xsize/2)) = repmat(im(:,1), [1 floor(xsize/2)]);
    borderim(:,floor(xsize/2)+1:xsize) = repmat(im(:,xsize), ...
                                              [1 floor(xsize/2)+odd]);
    out_im(mask) = borderim(mask);
  else
    out_im(mask) = 0;                                  % Zero pading     
  end
else
  xpos=[1:xsize]+disp;
  out_im=interp1(im,xpos, method);  
  mask = isnan(out_im);                                % No Pixel value 
  if all(border(1:4)=='copy')
    borderim(1:floor(xsize/2)) =  im(1)*ones([1 floor(xsize/2)]);
    borderim(floor(xsize/2)+1:xsize)=  im(xsize)*ones([1 floor(xsize/2)+odd]);
    out_im(mask) = borderim(mask);
  else
    out_im(mask) = 0;                                  % Zero pading     
  end
end

c_out = 1-mask;                                        %  Not padding region