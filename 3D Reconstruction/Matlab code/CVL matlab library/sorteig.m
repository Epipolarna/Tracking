function [A, B] = sorteig(M)

% SORTEIG  Sorted Eigenvalues and Eigenvectors
%          SORTEIG(M) is a matrix whose columns are the eigenvectors of M 
%          sorted in descending order according to their respective 
%          eigenvalues.
%
%          [A, B] = SORTEIG(M) produces a matrix A with eigenvectors and a 
%          vector B with corresponding eigenvalues of M. The eigenvalues 
%          and  eigenvectors are sorted so that the first column of A is the 
%          eigenvector corresponding to the largest eigenvalue and so on.
%          The eigenvalues are sorted by abs(e).
%
%          See allso EIG.

[V, D] = eig(M);	% get egenvectors and eigenvalues
V = fliplr(V);		% reverse order of eigenvectors
d = flipud(diag(D));	% extract eigenvalues and reverse their order
[d,I]= sort(d);		% sort reversed eigenvalues in ascending order
B = flipud(d);		% restore sorted eigenvalues into descending order
for j = 1:length(I)
  A(:,j) = V(:,I(j));   % sort reversed eigenvectors in ascending order
end
A = fliplr(A);		% restore sorted eigenvectors into descending order