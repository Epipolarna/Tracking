
N=20000
a = rand(1,N);
b = rand(1,N);

pause(0.5);

disp('Explicit loop without preallocated memory');
ca = [];
tic
for i = 1:N
    ca(i) = sin(a(i)*b(i));
end
toc

pause(0.5);

disp('Explicit loop with preallocated memory');
cb = zeros(1,N);
tic
for i = 1:N
    cb(i) = sin(a(i)*b(i));
end
toc

pause(0.5);

disp('Vector operation')
tic
cc = sin(a.*b);
toc

