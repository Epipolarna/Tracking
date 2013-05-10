lambda = 0.5;

f0 = im2double(imread('newboat.png'));
%f0 = f0(1:2:end,1:2:end);

figure, imshow(f0,[])

[nr,nc] = size(f0);
G1 = 1/2*speye(nr+2)+1/4*circshift(speye(nr+2),[1,0])+1/4*circshift(speye(nr+2),[-1,0]);
G1 = G1(2:end-1,2:end-1);

G2 = 1/2*speye(nc+2)+1/4*circshift(speye(nc+2),[1,0])+1/4*circshift(speye(nc+2),[-1,0]);
G2 = G2(2:end-1,2:end-1);

G = kron(G2,G1);
G = G*G;

f1 = G*f0(:);

figure, imshow(reshape(f1, nr, []),[])

D = speye(nr+2)-1/2*circshift(speye(nr+2),[1,0])-1/2*circshift(speye(nr+2),[-1,0]);
D = D(2:end-1,2:end-1);
Dy2 = kron(speye(nc),D);

D = speye(nc+2)-1/2*circshift(speye(nc+2),[1,0])-1/2*circshift(speye(nc+2),[-1,0]);
D = D(2:end-1,2:end-1);
Dx2 = kron(D,speye(nr));

f2 = (G'*G+lambda*(Dx2+Dy2))\(G'*f1(:));

figure, imshow(reshape(f2, nr, []),[])

figure, imshow(abs(f0-reshape(f2, nr, [])),[])


pause()

clear all
close all