function [K,R,t] = P2KRt(P)

% [K,R,t] = P2KRt(P)
%
% Computes camera matrix K, rotation R, and translation t from
% projection matrix P. Relation:
%
%         P ~ K[R t]
%
%   P - 3/4 projection matrix
%
%   K - 3/3 camera matrix
%   R - 3/3 rotation matrix
%   t - 3/1 translation vector

% bjojo 2006-03-02

[K,R] = rq(P(:,1:3));
t = inv(K)*P(:,4);
K = K/K(3,3);

% K should have positive sign

D = diag(sign(diag(K)));
K = K*D;
R = D*R;
t = D*t;

% R should have det(R)=1 (no mirroring)
t = det(R)*t;
R = det(R)*R;

function [R,Q] = rq(A)

% [R,Q] = rq(A)
%
% Orthogonal-triangular decomposition, A = R*Q, where R is an upper
% triangular matrix and Q is a unitary matrix.

A = A';
A = A(end:-1:1,end:-1:1);

[Q,R] = qr(A);

R = R';R = R(end:-1:1,end:-1:1);
Q = Q';Q = Q(end:-1:1,end:-1:1);

