function z=tensorprod(x,y,varargin)
% TENSORPROD - Multiply two arrays and sum over common dimensions.
%
% Author: Gunnar Farnebäck
%         Computer Vision Laboratory
%         Linköping University, Sweden
%         gf@isy.liu.se

xsize=size(x);
ysize=size(y);

xsize=[xsize ones(1,length(ysize)-length(xsize))];
ysize=[ysize ones(1,length(xsize)-length(ysize))];

if any(xsize>1 & ysize>1 & xsize~=ysize)
    error('The arrays don''t match.')
end

xx=repmat(x,max(ysize./xsize,1));
yy=repmat(y,max(xsize./ysize,1));

z=xx.*yy;

for i=find(xsize>1 & ysize>1)
    z=sum(z,i);
end

if length(varargin)>0
    z=tensorprod(z,varargin{:});
end
