function h=segimage(grayimage,segmentation,mode)

% SEGIMAGE(GRAYIMAGE,SEGMENTATION,MODE)
%
% Visualize the segmentation of a grayscale image. Each pixel gets a
% value from the HSV color space, so that hue is computed from
% SEGMENTATION, value is given by GRAYIMAGE, and saturation is set to one.
%
% GRAYIMAGE is a grayscale image with values between 0 and 1 or
% between 0 and 255. The latter case is assumed iff any value is larger
% than 1 or GRAYIMAGE is a uint8 matrix.
% SEGMENTATION is a matrix of the same size as GRAYIMAGE, preferably
% labeled with consecutive integers. The values are scaled and possibly
% translated to the interval [0..1) and then interpreted as hue values.
% The matrix can be double or uint8.
% MODE may be omitted or have the value 'random'. In the latter case
% the order of the segments is randomly permuted before they are
% assigned colors. SEGMENTATION must be integer valued to allow this 
% option.
% H=SEGIMAGE(...) returns a handle to the image.
%
% Author: Gunnar Farnebäck
%         Computer Vision Laboratory
%         Linköping University, Sweden

randcolors=0;

if nargin<2
    error('Not enough input arguments.')
elseif nargin>3
    error('Too many input arguments.')
elseif nargin==3 
    if mode=='random'
	randcolors=1;
    else
	error('Unknown MODE.')
    end
end

if ndims(grayimage)>2
    error('More than two dimensions in GRAYIMAGE')
end
if ndims(segmentation)>2
    error('More than two dimensions in SEGMENTATION')
end

if size(grayimage)~=size(segmentation)
    error('GRAYIMAGE and SEGMENTATION must have the same size')
end

if isa(grayimage,'uint8')
    gr=double(grayimage)/255;
elseif max(max(grayimage))>1
    gr=grayimage/255;
else
    gr=grayimage;
end

seg=double(segmentation);
segmax=max(max(seg));
segmin=min(min(seg));
if randcolors
    permutation=randperm(segmax-segmin+1);
    seg=permutation(seg-segmin+1)+segmin-1;
end
seg=(seg-segmin)/(segmax-segmin+1);

[m,n]=size(grayimage);

hsvimage=[seg(:) ones(m*n,1) gr(:)];
rgbimage=reshape(hsv2rgb(hsvimage),m,n,3);
hh=image(rgbimage);

if nargout>0
    h=hh;
end
