%function [y,ind1,ind2]=joint_min(M)
%
% Find joint minima in both row and columns
% of a matrix.
%
% M    Matrix with distance measures
%
% Returns: 
% Y    List of min values
% IND1 List of column indices
% IND2 List of row indices
%
% I.e. M(ind1(k),ind2(k))==y
%
%Per-Erik Forssen, Nov 2004
function [y,ind1,ind2]=joint_min(M)

[N1,N2]=size(M);
keepl=zeros(N1,1);      % List of flags
y=zeros(N1,1);          % List of min values
[a,ind1]=min(M,[],1);   % Min along dim 1
[a,ind2]=min(M,[],2);   % Min along dim 2

for k=1:N1,
  if(k==ind1(ind2(k))), % Min along both dimensions?
    y(k)=M(k,ind2(k));  % Read min value
    keepl(k)=1;         % Keep this one
  end
end
%
% Remove non-joint minima
%
keepl=find(keepl);
y=y(keepl);
ind2=ind2(keepl);
ind1=[1:N1]';
ind1=ind1(keepl);
