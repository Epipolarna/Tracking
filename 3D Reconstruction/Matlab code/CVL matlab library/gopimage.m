function h=gopimage(x,y,z)
    
% GOPIMAGE(Z) displays a colour image of the complex matrix Z.
% The argument is encoded by the gop color table and the magnitude 
% by the intensity. The magnitude is scaled so that the maximum
% magnitude corresponds to intensity one.
% GOPIMAGE(Z), where Z is an MxNx2 real array displays a vector field
% analogously.
% GOPIMAGE(X,Y) does the same thing, but here the components are split
% into two matrices.
% GOPIMAGE(Z,M) and GOPIMAGE(X,Y,M), where M is a scalar, amplifies
% the intensity a factor M.
% H=GOPIMAGE(...) returns a handle to the image.
%
% This function requires true-color capabilities.
%
% Author: Gunnar Farnebäck
%         Computer Vision Laboratory
%         Linköping University, Sweden

% Check the input parameters.
error(nargchk(1,3,nargin))

% Is an amplification factor provided?
M=1;
nin=nargin;
switch nargin
    case 2
	if prod(size(y))==1
	    M=y;
	    nin=1;
	end
    case 3
	if prod(size(z))==1
	    M=z;
	    nin=2;
	else
	    error('The third parameter must be a scalar.')
	end
end

if nin==1
    if ndims(x)==2
	u=real(x);
	v=imag(x);
    elseif ndims(x)==3
	if size(x,3)~=2
	    error('Z must have a third dimension of size 2.')
	else
	    u=x(:,:,1);
	    v=x(:,:,2);
	end
    else
	error('Too many dimensions.')
    end
else % nin==2
    if size(x)~=size(y)
	error('X and Y does not have the same size')
    elseif ndims(x)>2
	error('Too many dimensions.')
    else
	u=x;
	v=y;
    end
end

w=u+i*v;
gtab=goptab; % Load gop color table
gtab=[gtab;gtab(1,:)]; % Make it cyclic to allow interpolation for all angles.
tabangles=2*pi*(0:256)/256;
max_mag=max(max(abs(w)));
if max_mag~=0
    w=w/max_mag;
end
magw=abs(w);
magw=min(1,M*magw);
argw=pi+angle(-w);
argim=reshape(interp1(tabangles,gtab,argw(:)),[size(argw) 3]);
gopim=argim.*repmat(magw,[1 1 3]);
hh=image(gopim);

if nargout>0
    h=hh;
end
%axis xy;   % For lab3