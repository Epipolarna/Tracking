function D = stereo_displacement_1D(ImLeft,ImRight,bsz,maxdisp,method)

% D = stereo_displacement_1D(ImLeft,ImRight,bsz,maxdisp,method)
%
% Compute stereo (i.e. only positive) displacement in 1D by SAD
% blockmatching. For each 1D block of size bsz, find optimal
% displacement in region [0 maxdisp]
%
%        D - M1/M2 displacement image
%
%  ImLeft,ImRight - M1/M2/M3 images
%             bsz - Block/template size, [sz1 sz2]
%         maxdisp - Maximal displacement
%          method - 'SAD', 'SSD', 'corr' (default 'SAD')
%
%
%
% Björn Johansson, Fredrik Larsson, and Johan Hedborg
% 2007-10-29

if nargin<5 || isempty(method)
  method = 'SAD';
end

[M1 M2 M3] = size(ImLeft);
ind = 0:maxdisp;
N = maxdisp+1;

% Expand and subtract
%=====================
if isfloat(ImLeft)
  dim = zeros(M1,M2,N);
  dimC = zeros(M1,M2,N);
else
  dim = zeros(M1,M2,N,'int16');
  dimC = zeros(M1,M2,N,'int16');
end

switch method
 case 'SAD'
  for n=1:N
    dn = ImLeft - [nan(M1,ind(n),M3) ImRight(:,1:end-ind(n),:) ];
    dim(:,:,n) = sum(abs(dn),3)/M3;
  end
  
 case 'SSD'
  for n=1:N
    dn = ImLeft - [nan(M1,ind(n),M3) ImRight(:,1:end-ind(n),:) ];
    dim(:,:,n) = sum(abs(dn).^2,3)/M3;
  end
  
 case 'corr'
  dimL = zeros(M1,M2,N);
  dimR = zeros(M1,M2,N);
  
  for n=1:N
    dn = ImLeft .* [nan(M1,ind(n),M3) ImRight(:,1:end-ind(n),:) ];
    dim(:,:,n) = sum(dn,3)/M3;
    
    dn = ImLeft .* ImLeft;
    dimL(:,:,n) = sum(dn,3)/M3;
    
    dn = [nan(M1,ind(n),M3) ImRight(:,1:end-ind(n),:) ];
    dn = dn.*dn;
    dimR(:,:,n) = sum(dn,3)/M3;
  end
end


for n=1:N
  dimC(:,:,n) = [zeros(M1,ind(n)) ones(M1,M2-ind(n))];
end

% Sum
%=====
w1 = ones(bsz(1),1);
w2 = ones(1,bsz(2));
dim = imfilter(imfilter(dimC.*dim,w1,'same'),w2,'same')./...
      (eps + imfilter(imfilter(dimC,w1,'same'),w2,'same'));

if strcmp(method,'corr')
  dimL = imfilter(imfilter(dimC.*dimL,w1,'same'),w2,'same')./...
         (eps + imfilter(imfilter(dimC,w1,'same'),w2,'same'));
  dimR = imfilter(imfilter(dimC.*dimR,w1,'same'),w2,'same')./...
         (eps + imfilter(imfilter(dimC,w1,'same'),w2,'same'));
  dim = -dim./sqrt(dimL)./sqrt(dimR);
end

% Find min
%==========
[mn,D] = min(dim,[],3);
D = D - 1;

% Ad-hoc interpolation
%======================
D(:,1:maxdisp) = repmat(D(:,maxdisp+1),[1 maxdisp]);
