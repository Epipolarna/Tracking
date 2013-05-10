%function h=show_regions(xl,yl,roisz)
%
% Visualise regions as red squares centered
% around given positions.
%
% XL    List of ROI x-centres
% YL    List of ROI y-centres
% ROISZ Side of ROI
%
%Per-Erik Forssen, Nov 2004

function h=show_regions(xl,yl,roisz)

Np=size(xl,1);

h1=zeros(Np,1);
boxx=[-1 1 1 -1 -1]*roisz/2;
boxy=[-1 -1 1 1 -1]*roisz/2;

for k=1:Np,
  h(k)=plot(boxx+xl(k),boxy+yl(k),'r');
end

h2=zeros(Np,1);
for k=1:Np,
  h2(k)=text(xl(k),yl(k),num2str(k));
end
set(h2,'HorizontalAlignment','center');

if nargout>0,
  h=[h1;h2];
end