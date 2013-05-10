lambda = 0.5;

f0 = im2double(imread('newboat.png'));
%f0 = f0(1:2:end,1:2:end);

figure, imshow(f0,[])

[nr,nc] = size(f0);
G = zeros(2*nr,ceil(nc/2));
G(1:2:nr,:)=1;
G = G(:);
Gnz = find(G);
G = sparse(1:numel(Gnz),Gnz,1,numel(Gnz),nr*nc);
%for k=1:numel(Gnz)
%    G(k,Gnz(k))=1;
%end

f1 = G*f0(:);

figure, imshow(reshape(f1, ceil(nr/2), []),[])

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