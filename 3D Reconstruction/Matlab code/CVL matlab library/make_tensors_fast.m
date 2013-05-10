function [T,params]=make_tensors_fast(signal,spatial_size,region_of_interest,options)
% T=MAKE_TENSORS_FAST(SIGNAL,SPATIAL_SIZE,REGION_OF_INTEREST,OPTIONS)
% or
% [T,PARAMS]=MAKE_TENSORS_FAST(SIGNAL,SPATIAL_SIZE,REGION_OF_INTEREST,OPTIONS)
% 
% Compute orientation tensors in two, three, or four dimensions.
% The tensors are computed according to the algorithm by Gunnar Farneb�ck.
% This implementation uses completely separable filters and is quite fast.
% 
% SIGNAL                        - Signal values. Must be real and nonsparse 
%                                 and the number of dimensions, N, must be
%                                 two, three, or four.
%
% SPATIAL_SIZE [optional]       - Size of the spatial support of the filters
%                                 along each dimension. Default value is 9.
%
% REGION_OF_INTEREST [optional] - An Nx2 matrix where each row contains
%                                 start and stop indices along the
%                                 corresponding dimensions. Default value
%                                 is all of the signal.
%
% OPTIONS [optional]            - Struct array that may contain various
%                                 parameters that affect the algorithm.
%                                 These are explained below.
%
% T                             - Computed tensor field. T has N+2
%                                 dimensions, where the first N indices
%                                 indicates the position in the signal and
%                                 the last two contains the tensor for each
%                                 point. In the case that REGION_OF_INTEREST
%                                 is less than N-dimensional, the singleton
%                                 dimensions are removed.
%
% PARAMS                        - Struct array containing the parameters
%                                 that has been used by the algorithm.
%
%
% The following fields may be specified in the OPTIONS parameter:
% OPTIONS.gamma - Relation between the contribution to the tensor from the
%                 linear and the quadratic parts of the signal. 0 means that
%                 only the quadratic part matters while a very large number
%                 means that only the linear part is used. Default value is
%                 1/(2*sigma^2).
% OPTIONS.sigma - The filters are designed via normalized convolution, using
%                 a gaussian applicability function. This value sets the
%                 standard deviation of the gaussian. The size of the
%                 gaussian should be chosen with respect to the scale of the
%                 interesting structures in the signal. Additionally it is
%                 important that it is isotropic, i.e. that the contribution
%                 from close to the corners of the supporting region is
%                 negligible. Therefore it may be advisable to use
%                 OPTIONS.delta instead.
% OPTIONS.delta - The value of the gaussian applicability when it reaches
%                 the end of the supporting region along an axis. If both
%                 OPTIONS.sigma and OPTIONS.delta are set, the value of the
%                 latter will be used. If neither is given the default value
%                 is delta=0.005;
%
% Author: Gunnar Farneb�ck
%         Computer Vision Laboratory
%         Link�ping University, Sweden
%         gf@isy.liu.se

N=ndims(signal);
if N==2 & size(signal,2)==1
    N=1;
end

if nargin<2
    spatial_size=9;
end

if nargin<3
    if N==1
	region_of_interest=[1 size(signal,1)];
    else
	region_of_interest=[ones(N,1),size(signal)'];
    end
end

delta=0.005;
gamma=1;

if nargin==4
    if isfield(options,'delta')
	delta=options.delta;
    elseif isfield(options,'sigma')
	delta=exp(-((spatial_size-1)/2)^2/(2*options.sigma^2));
    else
	delta=0.005;
    end
    if isfield(options,'gamma')
	gamma=options.gamma;
    else
	gamma=-log(delta)/((spatial_size-1)/2);
    end
end

if spatial_size<1
    error('What use would such a small kernel be?')
elseif mod(spatial_size,2)~=1
    spatial_size=2*floor((spatial_size-1)/2)+1;
    warning(sprintf('Only kernels of odd size are allowed. Changed the size to %d.',spatial_size))
end

n=(spatial_size-1)/2;
sigma=n/sqrt(-2*log(delta));
a=exp(-(-n:n).^2/(2*sigma^2))';

switch N
    case 1
	error('Tensors for 1D signals are pretty useless and have not been implemented.')
    
    case 2
	% Set up applicability and basis functions.
	applicability=a*a';
	[x,y]=ndgrid(-n:n);
	b=cat(3, ones(size(x)), x, y, x.*x, y.*y, x.*y);
	nb=size(b,3);

	% Compute the inverse metric.
	Q=zeros(nb,nb);
	for i=1:nb
	    for j=i:nb
		Q(i,j)=sum(sum(b(:,:,i).*applicability.*b(:,:,j)));
		Q(j,i)=Q(i,j);
	    end
	end
	clear b applicability x y
	Qinv=inv(Q);
	
	% Convolutions in the y-direction.
	kernely0=a';
	kernely1=(-n:n).*a';
	kernely2=(-n:n).^2.*a';
	roiy=region_of_interest+[-n n;0 0];
	roiy(:,1)=max(roiy(:,1),ones(2,1));
	roiy(:,2)=min(roiy(:,2),size(signal)');
	conv_y0=conv3(signal,kernely0,roiy);
	conv_y1=conv3(signal,kernely1,roiy);
	conv_y2=conv3(signal,kernely2,roiy);
	
	% Convolutions in the x-direction.
	kernelx0=kernely0(:);
	kernelx1=kernely1(:);
	kernelx2=kernely2(:);
	roix=region_of_interest;
	roix=roix(1:ndims(conv_y0),:);
	roix(2:end,:)=roix(2:end,:)+1-repmat(roix(2:end,1),[1 2]);
	conv_results=zeros([diff(region_of_interest')+1 6]);
	conv_results(:,:,1)=conv3(conv_y0,kernelx0,roix); % y0x0
	conv_results(:,:,2)=conv3(conv_y0,kernelx1,roix); % y0x1
	conv_results(:,:,4)=conv3(conv_y0,kernelx2,roix); % y0x2
	clear conv_y0
	conv_results(:,:,3)=conv3(conv_y1,kernelx0,roix); % y1x0
	conv_results(:,:,6)=conv3(conv_y1,kernelx1,roix)/2; % y1x1
	clear conv_y1
	conv_results(:,:,5)=conv3(conv_y2,kernelx0,roix); % y2x0
	clear conv_y2
	
	% Apply the inverse metric.
	conv_results(:,:,2:3)=Qinv(2,2)*conv_results(:,:,2:3);
	conv_results(:,:,4:5)=Qinv(4,4)*conv_results(:,:,4:5) + ...
	    Qinv(4,1)*repmat(conv_results(:,:,1),[1 1 2]);
	conv_results(:,:,6)=Qinv(6,6)*conv_results(:,:,6);
	
	% Build tensor components.
	%
	% It's more efficient in matlab code to do a small matrix
	% multiplication "manually" in parallell over all the points
	% than doing a multiple loop over the points and computing the
	% matrix products "automatically".
	% The tensor is of the form A*A'+gamma*b*b', where A and b are
	% composed from the convolution results according to
	% 
	%   [4  6]    [2]
	% A=[6  5], b=[3].
	%
	% Thus (excluding gamma)
	%
	%   [4*4+6*6+2*2 4*6+5*6+2*3]
	% T=[4*6+5*6+2*3 6*6+5*5+3*3]

	T=zeros([diff(region_of_interest')+1 2 2]);
	T(:,:,1,1)=conv_results(:,:,4).^2+...
	    conv_results(:,:,6).^2+...
	    gamma*conv_results(:,:,2).^2;
	T(:,:,2,2)=conv_results(:,:,6).^2+...
	    conv_results(:,:,5).^2+...
	    gamma*conv_results(:,:,3).^2;
	T(:,:,1,2)=(conv_results(:,:,4)+...
	    conv_results(:,:,5)).*...
	    conv_results(:,:,6)+...
	    gamma*conv_results(:,:,2).*...
	    conv_results(:,:,3);
	T(:,:,2,1)=T(:,:,1,2);
	
	T=squeeze(T);
	
    case 3
	% Set up applicability and basis functions.
	applicability=tensorprod(a,shiftdim(a,-1),shiftdim(a,-2));
	[x,y,t]=ndgrid(-n:n);
	b=cat(4,ones(size(x)),x,y,t,x.*x,y.*y,t.*t,x.*y,x.*t,y.*t);
	nb=size(b,4);

	% Compute the inverse metric.
	Q=zeros(nb,nb);
	for i=1:nb
	    for j=i:nb
		Q(i,j)=sum(sum(sum(b(:,:,:,i).*applicability.*b(:,:,:,j))));
		Q(j,i)=Q(i,j);
	    end
	end
	clear b applicability x y t
	Qinv=inv(Q);

	% Convolutions in the t-direction
	kernelt0=reshape(a,[1 1 spatial_size]);
	kernelt1=reshape((-n:n)'.*a,[1 1 spatial_size]);
	kernelt2=reshape(((-n:n).^2)'.*a,[1 1 spatial_size]);
	roit=region_of_interest+[-n n;-n n;0 0];
	roit(:,1)=max(roit(:,1),ones(3,1));
	roit(:,2)=min(roit(:,2),size(signal)');
	conv_t0=conv3(signal,kernelt0,roit);
	conv_t1=conv3(signal,kernelt1,roit);
	conv_t2=conv3(signal,kernelt2,roit);
	
	% Convolutions in the y-direction
	kernely0=reshape(kernelt0,[1 spatial_size]);
	kernely1=reshape(kernelt1,[1 spatial_size]);
	kernely2=reshape(kernelt2,[1 spatial_size]);
	roiy=region_of_interest+[-n n;0 0;0 0];
	roiy(:,1)=max(roiy(:,1),ones(3,1));
	roiy(:,2)=min(roiy(:,2),size(signal)');
	if diff(roiy(3,:))==0
	    roiy=roiy(1:2,:);
	else
	    roiy(3,:)=roiy(3,:)+1-roiy(3,1);
	end
	conv_t0y0=conv3(conv_t0,kernely0,roiy);
	conv_t0y1=conv3(conv_t0,kernely1,roiy);
	conv_t0y2=conv3(conv_t0,kernely2,roiy);
	clear conv_t0
	conv_t1y0=conv3(conv_t1,kernely0,roiy);
	conv_t1y1=conv3(conv_t1,kernely1,roiy);
	clear conv_t1
	conv_t2y0=conv3(conv_t2,kernely0,roiy);
	clear conv_t2
	
	% Convolutions in the x-direction
	kernelx0=reshape(kernelt0,[spatial_size 1]);
	kernelx1=reshape(kernelt1,[spatial_size 1]);
	kernelx2=reshape(kernelt2,[spatial_size 1]);
	roix=region_of_interest;
	roix=roix(1:ndims(conv_t0y0),:);
	roix(2:end,:)=roix(2:end,:)+1-repmat(roix(2:end,1),[1 2]);
	conv_results=zeros([diff(region_of_interest')+1 10]);
	conv_results(:,:,:,1)=conv3(conv_t0y0,kernelx0,roix); % t0y0x0
	conv_results(:,:,:,2)=conv3(conv_t0y0,kernelx1,roix); % t0y0x1
	conv_results(:,:,:,5)=conv3(conv_t0y0,kernelx2,roix); % t0y0x2
	clear conv_t0y0
	conv_results(:,:,:,3)=conv3(conv_t0y1,kernelx0,roix); % t0y1x0
	conv_results(:,:,:,8)=conv3(conv_t0y1,kernelx1,roix)/2; % t0y1x1
	clear conv_t0y1
	conv_results(:,:,:,6)=conv3(conv_t0y2,kernelx0,roix); % t0y2x0
	clear conv_t0y2
	conv_results(:,:,:,4)=conv3(conv_t1y0,kernelx0,roix); % t1y0x0
	conv_results(:,:,:,9)=conv3(conv_t1y0,kernelx1,roix)/2; % t1y0x1
	clear conv_t1y0
	conv_results(:,:,:,10)=conv3(conv_t1y1,kernelx0,roix)/2; % t1y1x0
	clear conv_t1y1
	conv_results(:,:,:,7)=conv3(conv_t2y0,kernelx0,roix); % t2y0x0
	clear conv_t2y0
	
	% Apply the inverse metric.
	conv_results(:,:,:,2:4)=Qinv(2,2)*conv_results(:,:,:,2:4);
	conv_results(:,:,:,5:7)=Qinv(5,5)*conv_results(:,:,:,5:7) + ...
	    Qinv(5,1)*repmat(conv_results(:,:,:,1),[1 1 1 3]);
	conv_results(:,:,:,8:10)=Qinv(8,8)*conv_results(:,:,:,8:10);

	% Build tensor components.
	%
	% It's more efficient in matlab code to do a small matrix
	% multiplication "manually" in parallell over all the points
	% than doing a multiple loop over the points and computing the
	% matrix products "automatically".
	% The tensor is of the form T=A*A'+gamma*b*b', where A and b are
	% composed from the convolution results according to
	% 
	%   [5  8  9]    [2]
	% A=[8  6 10], b=[3].
	%   [9 10  7]    [4]
	%
	% Thus (excluding gamma)
	%
	%   [5*5+8*8+9*9+2*2  5*8+6*8+9*10+2*3  5*9+8*10+7*9+2*4 ]
	% T=[5*8+6*8+9*10+2*3 8*8+6*6+10*10+3*3 8*9+6*10+7*10+3*4].
	%   [5*9+8*10+7*9+2*4 8*9+6*10+7*10+3*4 9*9+10*10+7*7+4*4]
	
	T=zeros([diff(region_of_interest')+1 3 3]);
	T(:,:,:,1,1)=conv_results(:,:,:,5).^2+...
	    conv_results(:,:,:,8).^2+...
	    conv_results(:,:,:,9).^2+...
	    gamma*conv_results(:,:,:,2).^2;
	T(:,:,:,2,2)=conv_results(:,:,:,8).^2+...
	    conv_results(:,:,:,6).^2+...
	    conv_results(:,:,:,10).^2+...
	    gamma*conv_results(:,:,:,3).^2;
	T(:,:,:,3,3)=conv_results(:,:,:,9).^2+...
	    conv_results(:,:,:,10).^2+...
	    conv_results(:,:,:,7).^2+...
	    gamma*conv_results(:,:,:,4).^2;
	T(:,:,:,1,2)=(conv_results(:,:,:,5)+...
	    conv_results(:,:,:,6)).*...
	    conv_results(:,:,:,8)+...
	    conv_results(:,:,:,9).*...
	    conv_results(:,:,:,10)+...
	    gamma*conv_results(:,:,:,2).*...
	    conv_results(:,:,:,3);
	T(:,:,:,2,1)=T(:,:,:,1,2);
	T(:,:,:,1,3)=(conv_results(:,:,:,5)+...
	    conv_results(:,:,:,7)).*...
	    conv_results(:,:,:,9)+...
	    conv_results(:,:,:,8).*...
	    conv_results(:,:,:,10)+...
	    gamma*conv_results(:,:,:,2).*...
	    conv_results(:,:,:,4);
	T(:,:,:,3,1)=T(:,:,:,1,3);
	T(:,:,:,2,3)=(conv_results(:,:,:,6)+...
	    conv_results(:,:,:,7)).*...
	    conv_results(:,:,:,10)+...
	    conv_results(:,:,:,8).*...
	    conv_results(:,:,:,9)+...
	    gamma*conv_results(:,:,:,3).*...
	    conv_results(:,:,:,4);
	T(:,:,:,3,2)=T(:,:,:,2,3);
	
	T=squeeze(T);
	
    case 4
	% Set up applicability and basis functions.
	applicability=tensorprod(a,shiftdim(a,-1),shiftdim(a,-2),...
	    shiftdim(a,-3));
	[x,y,z,t]=ndgrid(-n:n);
	b=cat(5,ones(size(x)),x,y,z,t,x.*x,y.*y,z.*z,t.*t,...
	    x.*y,x.*z,x.*t,y.*z,y.*t,z.*t);
	nb=size(b,5);

	% Compute the inverse metric.
	Q=zeros(nb,nb);
	for i=1:nb
	    for j=i:nb
		Q(i,j)=sum(sum(sum(sum(b(:,:,:,i).*applicability.*b(:,:,:,j)))));
		Q(j,i)=Q(i,j);
	    end
	end
	clear b applicability x y z t
	Qinv=inv(Q);

	% Convolutions in the t-direction
	kernelt0=reshape(a,[1 1 1 spatial_size]);
	kernelt1=reshape((-n:n)'.*a,[1 1 1 spatial_size]);
	kernelt2=reshape(((-n:n).^2)'.*a,[1 1 1 spatial_size]);
	roit=region_of_interest+[-n n;-n n;-n n;0 0];
	roit(:,1)=max(roit(:,1),ones(3,1));
	roit(:,2)=min(roit(:,2),size(signal)');
	conv_t0=conv3(signal,kernelt0,roit);
	conv_t1=conv3(signal,kernelt1,roit);
	conv_t2=conv3(signal,kernelt2,roit);
	
	% Convolutions in the z-direction
	kernelz0=reshape(kernelt0,[1 1 spatial_size]);
	kernelz1=reshape(kernelt1,[1 1 spatial_size]);
	kernelz2=reshape(kernelt2,[1 1 spatial_size]);
	roiz=region_of_interest+[-n n;-n n;0 0;0 0];
	roiz(:,1)=max(roiz(:,1),ones(4,1));
	roiz(:,2)=min(roiz(:,2),size(signal)');
	if diff(roiz(4,:))==0
	    roiz=roiz(1:2,:);
	else
	    roiz(4,:)=roiz(4,:)+1-roiz(4,1);
	end
	conv_t0z0=conv3(conv_t0,kernelz0,roiz);
	conv_t0z1=conv3(conv_t0,kernelz1,roiz);
	conv_t0z2=conv3(conv_t0,kernelz2,roiz);
	clear conv_t0
	conv_t1z0=conv3(conv_t1,kernelz0,roiz);
	conv_t1z1=conv3(conv_t1,kernelz1,roiz);
	clear conv_t1
	conv_t2z0=conv3(conv_t2,kernelz0,roiz);
	clear conv_t2
	
	% Convolutions in the y-direction
	kernely0=reshape(kernelt0,[1 spatial_size]);
	kernely1=reshape(kernelt1,[1 spatial_size]);
	kernely2=reshape(kernelt2,[1 spatial_size]);
	roiy=region_of_interest+[-n n;0 0;0 0;0 0];
	roiy(:,1)=max(roiy(:,1),ones(4,1));
	roiy(:,2)=min(roiy(:,2),size(signal)');
	roiy=roiy(1:ndims(conv_t0y0),:);
	roiy(3:end,:)=roiy(3:end,:)+1-repmat(roiy(3:end,1),[1 2]);
	conv_t0z0y0=conv3(conv_t0z0,kernely0,roiy);
	conv_t0z0y1=conv3(conv_t0z0,kernely1,roiy);
	conv_t0z0y2=conv3(conv_t0z0,kernely2,roiy);
	clear conv_t0z0
	conv_t0z1y0=conv3(conv_t0z1,kernely0,roiy);
	conv_t0z1y1=conv3(conv_t0z1,kernely1,roiy);
	clear conv_t0z1
	conv_t0z2y0=conv3(conv_t0z2,kernely0,roiy);
	clear conv_t0z2
	conv_t1z0y0=conv3(conv_t1z0,kernely0,roiy);
	conv_t1z0y1=conv3(conv_t1z0,kernely1,roiy);
	clear conv_t1z0
	conv_t1z1y0=conv3(conv_t1z1,kernely0,roiy);
	clear conv_t1z1
	conv_t2z0y0=conv3(conv_t2z0,kernely0,roiy);
	clear conv_t2z0
	
	% Convolutions in the x-direction
	kernelx0=reshape(kernelt0,[spatial_size 1]);
	kernelx1=reshape(kernelt1,[spatial_size 1]);
	kernelx2=reshape(kernelt2,[spatial_size 1]);
	roix=region_of_interest;
	roix=roix(1:ndims(conv_t0y0z0),:);
	roix(2:end,:)=roix(2:end,:)+1-repmat(roix(2:end,1),[1 2]);
	conv_results=zeros([diff(region_of_interest')+1 15]);
	conv_results(:,:,:,:,1)=conv3(conv_t0z0y0,kernelx0,roix); % t0z0y0x0
	conv_results(:,:,:,:,2)=conv3(conv_t0z0y0,kernelx1,roix); % t0z0y0x1
	conv_results(:,:,:,:,6)=conv3(conv_t0z0y0,kernelx2,roix); % t0z0y0x2
	clear conv_t0z0y0
	conv_results(:,:,:,:,3)=conv3(conv_t0z0y1,kernelx0,roix); % t0z0y1x0
	conv_results(:,:,:,:,10)=conv3(conv_t0z0y1,kernelx1,roix)/2; % t0z0y1x1
	clear conv_t0z0y1
	conv_results(:,:,:,:,7)=conv3(conv_t0y0y2,kernelx0,roix); % t0z0y2x0
	clear conv_t0z0y2
	conv_results(:,:,:,:,4)=conv3(conv_t0z1y0,kernelx0,roix); % t0z1y0x0
	conv_results(:,:,:,:,11)=conv3(conv_t0z1y0,kernelx1,roix)/2; % t0z1y0x1
	clear conv_t0z1y0
	conv_results(:,:,:,:,13)=conv3(conv_t0z1y1,kernelx0,roix)/2; % t0z1y1x0
	clear conv_t0z1y1
	conv_results(:,:,:,:,8)=conv3(conv_t0z2y0,kernelx0,roix); % t0z2y0x0
	clear conv_t0z2y0
	conv_results(:,:,:,:,5)=conv3(conv_t1z0y0,kernelx0,roix); % t1z0y0x0
	conv_results(:,:,:,:,12)=conv3(conv_t1z0y0,kernelx1,roix)/2; % t1z0y0x1
	clear conv_t1z0y0
	conv_results(:,:,:,:,14)=conv3(conv_t1z0y1,kernelx0,roix)/2; % t1z0y1x0
	clear conv_t1z0y1
	conv_results(:,:,:,:,15)=conv3(conv_t1z1y0,kernelx0,roix)/2; % t1z1y0x0
	clear conv_t1z1y0
	conv_results(:,:,:,:,9)=conv3(conv_t2z0y0,kernelx0,roix); % t2z0y0x0
	clear conv_t2z0y0
	
	% Apply the inverse metric.
	conv_results(:,:,:,:,2:5)=Qinv(2,2)*conv_results(:,:,:,:,2:5);
	conv_results(:,:,:,:,6:9)=Qinv(6,6)*conv_results(:,:,:,:,6:9) + ...
	    Qinv(6,1)*repmat(conv_results(:,:,:,:,1),[1 1 1 1 4]);
	conv_results(:,:,:,:,10:15)=Qinv(10,10)*conv_results(:,:,:,:,10:15);
	
	% Build tensor components.
	%
	% It's more efficient in matlab code to do a small matrix
	% multiplication "manually" in parallell over all the points
	% than doing a multiple loop over the points and computing the
	% matrix products "automatically".
	% The tensor is of the form T=A*A'+gamma*b*b', where A and b are
	% composed from the convolution results according to
	% 
	%   [6  10 11 12]    [2]
	%   [10  7 13 14]    [3] 
	% A=[11 13  8 15], b=[4].
	%   [12 14 15  9]    [5]
	%
	% Thus (excluding gamma)
	%
	%   [6*6+10*10+11*11+12*12+2*2 6*10+7*10+11*13+12*14+2*3
	%   [6*10+7*10+11*13+12*14+2*3 10*10+7*7+13*13+14*14+3*3
	% T=[6*11+10*13+8*11+12*15+2*4 10*11+7*13+8*13+14*15+3*4
	%   [6*12+10*14+11*15+9*12+2*5 10*12+7*14+13*15+9*14+3*5
	%
	%    6*11+10*13+8*11+12*15+2*4 6*12+10*14+11*15+9*12+2*5]
	%    10*11+7*13+8*13+14*15+3*4 10*12+7*14+13*15+9*14+3*5]
	%    11*11+13*13+8*8+15*15+4*4 11*12+13*14+8*15+9*15+4*5]
	%    11*12+13*14+8*15+9*15+4*5 12*12+14*14+15*15+9*9+5*5]
	
	T=zeros([diff(region_of_interest')+1 3 3]);
	T(:,:,:,:,1,1)=conv_results(:,:,:,:,6).^2+...
	    conv_results(:,:,:,:,10).^2+...
	    conv_results(:,:,:,:,11).^2+...
	    conv_results(:,:,:,:,12).^2+...
	    gamma*conv_results(:,:,:,:,2).^2;
	T(:,:,:,:,2,2)=conv_results(:,:,:,:,10).^2+...
	    conv_results(:,:,:,:,7).^2+...
	    conv_results(:,:,:,:,13).^2+...
	    conv_results(:,:,:,:,14).^2+...
	    gamma*conv_results(:,:,:,:,3).^2;
	T(:,:,:,:,3,3)=conv_results(:,:,:,:,11).^2+...
	    conv_results(:,:,:,:,13).^2+...
	    conv_results(:,:,:,:,8).^2+...
	    conv_results(:,:,:,:,15).^2+...
	    gamma*conv_results(:,:,:,:,4).^2;
	T(:,:,:,:,4,4)=conv_results(:,:,:,:,11).^2+...
	    conv_results(:,:,:,:,13).^2+...
	    conv_results(:,:,:,:,8).^2+...
	    conv_results(:,:,:,:,15).^2+...
	    gamma*conv_results(:,:,:,:,4).^2;
	T(:,:,:,:,1,2)=(conv_results(:,:,:,:,6)+...
	    conv_results(:,:,:,:,7)).*...
	    conv_results(:,:,:,:,10)+...
	    conv_results(:,:,:,:,11).*...
	    conv_results(:,:,:,:,13)+...
	    conv_results(:,:,:,:,12).*...
	    conv_results(:,:,:,:,14)+...
	    gamma*conv_results(:,:,:,:,2).*...
	    conv_results(:,:,:,:,3);
	T(:,:,:,:,2,1)=T(:,:,:,:,1,2);
	T(:,:,:,:,1,3)=(conv_results(:,:,:,:,6)+...
	    conv_results(:,:,:,:,8)).*...
	    conv_results(:,:,:,:,11)+...
	    conv_results(:,:,:,:,10).*...
	    conv_results(:,:,:,:,13)+...
	    conv_results(:,:,:,:,12).*...
	    conv_results(:,:,:,:,15)+...
	    gamma*conv_results(:,:,:,:,2).*...
	    conv_results(:,:,:,:,4);
	T(:,:,:,:,3,1)=T(:,:,:,:,1,3);
	T(:,:,:,:,1,4)=(conv_results(:,:,:,:,6)+...
	    conv_results(:,:,:,:,9)).*...
	    conv_results(:,:,:,:,12)+...
	    conv_results(:,:,:,:,10).*...
	    conv_results(:,:,:,:,14)+...
	    conv_results(:,:,:,:,11).*...
	    conv_results(:,:,:,:,15)+...
	    gamma*conv_results(:,:,:,:,2).*...
	    conv_results(:,:,:,:,5);
	T(:,:,:,:,4,1)=T(:,:,:,:,1,4);
	T(:,:,:,:,2,3)=(conv_results(:,:,:,:,7)+...
	    conv_results(:,:,:,:,8)).*...
	    conv_results(:,:,:,:,13)+...
	    conv_results(:,:,:,:,10).*...
	    conv_results(:,:,:,:,11)+...
	    conv_results(:,:,:,:,14).*...
	    conv_results(:,:,:,:,15)+...
	    gamma*conv_results(:,:,:,:,3).*...
	    conv_results(:,:,:,:,4);
	T(:,:,:,:,3,2)=T(:,:,:,:,2,3);
	T(:,:,:,:,2,4)=(conv_results(:,:,:,:,7)+...
	    conv_results(:,:,:,:,9)).*...
	    conv_results(:,:,:,:,14)+...
	    conv_results(:,:,:,:,10).*...
	    conv_results(:,:,:,:,12)+...
	    conv_results(:,:,:,:,13).*...
	    conv_results(:,:,:,:,15)+...
	    gamma*conv_results(:,:,:,:,3).*...
	    conv_results(:,:,:,:,5);
	T(:,:,:,:,4,2)=T(:,:,:,:,2,4);
	T(:,:,:,:,3,4)=(conv_results(:,:,:,:,8)+...
	    conv_results(:,:,:,:,9)).*...
	    conv_results(:,:,:,:,15)+...
	    conv_results(:,:,:,:,11).*...
	    conv_results(:,:,:,:,12)+...
	    conv_results(:,:,:,:,13).*...
	    conv_results(:,:,:,:,14)+...
	    gamma*conv_results(:,:,:,:,4).*...
	    conv_results(:,:,:,:,5);
	T(:,:,:,:,4,3)=T(:,:,:,:,3,4);
	
	T=squeeze(T);	
    otherwise
	error('More than four dimensions are not supported.')
end

if nargout>1
    params.spatial_size=spatial_size;
    params.region_of_interest=region_of_interest;
    params.gamma=gamma;
    params.sigma=sigma;
    params.delta=delta;
end
