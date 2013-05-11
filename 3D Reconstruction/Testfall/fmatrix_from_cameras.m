%function F=fmatrix_from_cameras(C1,C2)
%
% Combine camera projection matrices C1,C2
% into a fundamental matrix F.
%
%Per-Erik Forssen, March 2008
function F=fmatrix_from_cameras(C1,C2)

if 1,
    % Extract focal point
    [U,D,V] = svd(C2);
    n = V(:,4);
    e=C1*n;

    F=cross_matrix(e)*C1*pinv(C2);
else
    [U,D,V] = svd(C1);
    n = V(:,4);

    e21=C2*n;
    C3=C2*C1'*inv(C1*C1');
    F=zeros(3,3);
    F(1,:)=cross(e21,C3(:,1));
    F(2,:)=cross(e21,C3(:,2));
    F(3,:)=cross(e21,C3(:,3));
	
end
