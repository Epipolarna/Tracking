function S = gensignal1d()
% S = gensignal1d()
%
% Generates a 1D signal containing blurred lines and edges.
% This is the signal used in chapter 7.1 in the book 
% "Signal Processing for Computer Vision"

blurfilter=gaussian(17, 0, 1.8);

line=zeros(1,39); 
line(20)=1; 
line=imconv(line,blurfilter); 
line=line/max(line); 

edge=[zeros(1,19) 0.5 ones(1,19)]; 
edge=imconv(edge, blurfilter); 
edge=edge/max(edge); 

S = [line 0 edge 1 1-line 1 1-edge]; 
