function h = vectorfield(varargin)
%VECTORFIELD plot
%  
%   VECTORFIELD(VX,VY,CERT,S)
%
%   VECTORFIELD(VX,VY,CERT)
%
%   VECTORFIELD(VX,VY,S)
%
%   VECTORFIELD(VX,VY)
%
%   H = VECTORFIELD(...) returns handle to vectors
%
%   VX and VY are equal-size matrices containing 
%   x- and y-components of vectors.
%
%   CERT is a non-negative matrix with values
%   that determine the grey-value of each vector.
%
%   S is a non-negative scalar value that determines the scaling of the vectors.
%
%   Jorgen Karlholm 970429
%
alpha = 0.33; % Size of arrow head relative to the length of the vector
beta  = 0.33; % Width of the base of the arrow head relative to the length

if (nargin==2)
    VX=varargin{1};
    VY=varargin{2};
    S=1;
    CERT=ones(size(VX));
else 
    if (nargin==3)
	VX=varargin{1};
	VY=varargin{2};
	if (size(varargin{3},1) == 1 & size(varargin{3},2) == 1)
	    S=varargin{3};
	    CERT=ones(size(VX));
	else
	    S=1;
	   CERT=varargin{3};
	   CERT=CERT/max(max(CERT));
       end
   else
       if (nargin==4)
	      VX=varargin{1};
	      VY=varargin{2};
	      CERT=varargin{3};
	      CERT=CERT/max(max(CERT));
	      S=varargin{4};
	  else
	      error(sprintf('Wrong number of arguments'));
	      return;
	  end
      end
  end

[nrows,ncols] = size(VX);
if (nrows ~= size(VY,1) | nrows ~= size(CERT,1) ...
	| ncols ~= size(VY,2) | ncols ~= size(CERT,2))
    error(sprintf('Matrices have different sizes'));
    return;
end;

h = newplot;
for i=1:nrows
    for j=1:ncols
	xfrom=(j-0.5);
	dx=S*VX(i,j);
	dy=S*VY(i,j);
	xto=xfrom+dx;
	yfrom=(i-0.5);
	yto=yfrom+dy;
	hl = line([xfrom xto],[yfrom yto]);
	set(hl,'color',(1-CERT(i,j))*[1 1 1]);
	ha = line([(xto-alpha*(dx+beta*(dy+eps))) ...
		xto ...
		(xto-alpha*(dx-beta*(dy+eps)))], ...
	    [(yto-alpha*(dy-beta*(dx+eps))) ...
		yto ...
		(yto-alpha*(dy+beta*(dy+eps)))]);
	set(ha,'color',(1-CERT(i,j))*[1 1 1]);
    end;
end;
axis('equal');
axis ij;
set(h,'xtick',[]);
set(h,'ytick',[]);
axis([0 ncols 0 nrows]);
box on;



