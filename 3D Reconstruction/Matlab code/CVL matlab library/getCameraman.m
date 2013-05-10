function [I J dTrue]=getCameraman();
%function [I J dTrue]=getCameraman();
%
%This function returns the cropped cameraman image, the dTrueed image and the displacement vector.
%
% --- Output ---
%   'I'     : original cropped image
%   'J'     : dTrueed image
%   'dTrue' : used displacement
%

nBorder=10;

dTrue=[-2 1];
%rStart=1;%Uncomment to use a random dTrue
%dTrue=round((rand(1,2)-0.5)*2*rStart); 


%Getting full cameraman
imgFull=double(imread('cameraman.tif'));

%Extracting part of camerman
I=imgFull(nBorder+1:end-nBorder,nBorder+1:end-nBorder);        

%Extracting dTrueed part of %cameraman
J=imgFull(nBorder-dTrue(1)+1:end-nBorder-dTrue(1),nBorder-dTrue(2)+1:end-nBorder-dTrue(2));




