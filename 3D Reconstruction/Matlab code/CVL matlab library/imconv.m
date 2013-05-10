function out=imconv(image,kernel,borderflg)
% out=imconv(image,kernel,borderflg)
%
%Convolve the image with the kernel. The output image is trucated 
%to the same size as the input image. Borderflg controls which type
%of border extenstion to use:
%
%                     0    Black border
%                     1    Copy edge value
%                     2    Cyclic 
%                     3    Mirror
%
% Parameter  Range      Default         Description
% =========  =========  ==============  ==============================
% image         -                       Any 1D or 2D array. 
% kernel        -                       Any 1D or 2D array. 
% borderflg   0 - 3          1          Border extension type
% ====================================================================
%
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se


if nargin < 2
 help imconv
end

if nargin < 3
  borderflg=1;
end
borderflg=max([0 min([borderflg 3])]);

[imhgh imwid] = size(image);
[krnhgh krnwid] = size(kernel);

if (borderflg>1)&((imhgh<krnhgh)|(imwid<krnwid))
   fprintf(1,'  Error! Kernel larger than image.\n');
   fprintf(1,'         Chosen border extension unvalid\n')
   return
end

rl=fix((krnhgh-1)/2);
rh=fix(krnhgh/2);

cl=fix((krnwid-1)/2);
ch=fix(krnwid/2);

tmp =image;
[tmphgh tmpwid] = size(tmp);

if borderflg == 0
   convtype='same';
elseif borderflg == 1                                    %*** copy border ***
   convtype='valid';
   tmp=[ ones([rl 1])*tmp(1,:) ; tmp ; ones([rh 1])*tmp(tmphgh,:)];
   [tmphgh tmpwid] = size(tmp);
   tmp=[ tmp(:,1)*ones([1 cl])   tmp   tmp(:,tmpwid)*ones([1 ch])];
elseif borderflg == 2                                    %*** cyclic border ***
   convtype='valid';
   tmp=[ tmp(tmphgh-rl+1:tmphgh,:) ; tmp ; tmp(1:rh,:)];
   [tmphgh tmpwid] = size(tmp);
   tmp=[ tmp(:,tmpwid-cl+1:tmpwid)   tmp   tmp(:,1:ch)];
elseif borderflg == 3                                    %*** mirror border ***
   convtype='valid';
   tmp=[ tmp(rl+1:-1:2,:); tmp ; tmp(tmphgh-1:-1:tmphgh-rh,:)];
   [tmphgh tmpwid] = size(tmp);
   tmp=[ tmp(:,cl+1:-1:2)  tmp   tmp(:,tmpwid-1:-1:tmpwid-ch)];
end

out=conv2(tmp,kernel,convtype);




