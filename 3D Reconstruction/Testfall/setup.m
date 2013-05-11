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
            
%% Definitions
t1 = [0 0 1]';
R1 = eye(1);
t2 = [1 2 0.5]';
R2 = Rx(0.3)*Ry(0.2)*Rz(0.1)
K = eye(3);

C1 = K*[eye(3) zeros(3,1)]
C2 = K*[R t]

%% 3D points

p3D = [-1 -1  2
       -1  1  2
        1 -1  2
        1  1  2
        0  1  2
        0  3  2
        2  1  2
        2  3  2]
        
%% Visualize

n1 = cameraCenter(R1,t1);
n2 = cameraCenter(R2,t2);

figure(1); clf; hold on;
plot3(p3D(:,1), p3D(:,2), p3D(:,3), 'x');
plot3(n1(1), n1(2), n1(3), 'or');
plot3(n2(1), n2(2), n2(3), 'og');



       