%% Functions
Rx = @(theta) [ 1 0           0
                0 cos(theta) -sin(theta)
                0 sin(theta)  cos(theta) ]
            
Ry = @(theta) [ cos(theta) 0  sin(theta)
                0          1  0
               -sin(theta) 0  cos(theta) ]
           
Rz = @(theta) [ cos(theta) -sin(theta) 0  
                sin(theta)  cos(theta) 0
                0           0          1 ]
            
cameraCenter = @(R,t) -R'*t
cameraToWorld = @(R,t) [R' -R'*t; 0 0 0 1]
            
%% Definitions
t1 = [0 0 0]';
R1 = eye(3);
t2 = -[1 0 0]';
R2 = Rx(0.1);%Rx(0.3)*Ry(0.2)*Rz(0.1)
K = eye(3);

C1 = K*[eye(3) zeros(3,1)]
C2 = K*[R2 t2]

%% 3D points

p3D = [-1 -1  2
       -1  1  2
        1 -1  2
        1  1  2
        0  1  2
        0  3  2
        2  1  2
        2  3  2]

% sine
x = -1:0.1:3.1;
p3D = [x;sin(3.1*x);sin(x)+2]';

% circle
x = 0:0.1:(2*pi)
p3D = [cos(x)' sin(x)' 2*ones(size(x))'];

p3DHom = [p3D ones(length(p3D),1)];
    
%% Visualize

n1 = cameraCenter(R1,t1)
n2 = cameraCenter(R2,t2)

figure(1); clf; hold on;
plot3(p3D(:,1), p3D(:,2), p3D(:,3), 'x');
plot3(n1(1), n1(2), n1(3), 'om');
plot3(n2(1), n2(2), n2(3), 'oc');

%% Image plane coordinates

p1Hom = (C1*p3DHom')'
p1 = [p1Hom(:,1) p1Hom(:,2)]./[p1Hom(:,3) p1Hom(:,3)]   % Inhomogenous
p2Hom = (C2*p3DHom')'
p2 = [p2Hom(:,1) p2Hom(:,2)]./[p2Hom(:,3) p2Hom(:,3)]   % Inhomogenous

figure(1); clf; hold on;
plot3(p3D(:,1), p3D(:,2), p3D(:,3), 'x');
plot3(p1(:,1), p1(:,2), zeros(size(p1(:,1))), 'xr');
plot3(p2(:,1), p2(:,2), zeros(size(p2(:,1))), 'dg');
plot3(n1(1), n1(2), n1(3), 'om');
plot3(n2(1), n2(2), n2(3), 'oc');

imagePair1 = [p1 p2]

%% Triangulate points - (verification)

% Linear
disp('linear)');
p3D_Hom = [];
for n = 1:length(p1Hom)
    p3D_Hom = [p3D_Hom; triangulate_linear(C1,C2,p1Hom(n,:),p2Hom(n,:))'];
end
p3D_ = [p3D_Hom(:,1) p3D_Hom(:,2) p3D_Hom(:,3) ]./[p3D_Hom(:,4) p3D_Hom(:,4) p3D_Hom(:,4)]   % Inhomogenous

diff = p3D-p3D_
diff_norm = norm(diff)


%% F, E

F = fmatrix_stls(p1,p2)
E = K'*F*K


%% 

       