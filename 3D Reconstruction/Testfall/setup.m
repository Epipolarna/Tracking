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
            
%% Definitions
t = [1 2 3]';
R = Rx(0.3)*Ry(0.2)*Rz(0.1)
K = eye(3);

C1 = K*[eye(3) zeros(3,1)]
C2 = K*[R t]

%% 3D points

p3D = [2 2 2
       
       