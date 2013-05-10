%function rl=cut_out_rois(img,xl,yl,roisz);
%
% Cut out regions of interest (ROI) and
% store their pixel values in columns of
% a matrix.
%
% IMG   Input image
% XL    List of ROI x-centres
% YL    List of ROI y-centres
% ROISZ side of ROI (default 7)
%
%Per-Erik Forssen, Nov 2004
function rl=cut_out_rois(img,xl,yl,roisz);



if nargin<4,roisz=7;end

nroi=prod(size(xl));

rl=zeros(roisz*roisz,nroi);

[rows,cols]=size(img);

rfl=floor(roisz/2);

imgp=[zeros(rows,rfl) img zeros(rows,rfl)];
imgp=[zeros(rfl,cols+2*rfl);imgp;zeros(rfl,cols+2*rfl)];

roix=[1:roisz];

for k=1:nroi,
  croi=imgp(roix+yl(k),roix+xl(k));
  rl(:,k)=croi(:);
end

