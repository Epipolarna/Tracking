lambda = 0.1;
stp = 3;
dd = [0;0];

I = double(rgb2gray(imread('view0.png')));
I = I(1:2:end,1:2:end);

%find points
hresp = harris(I, 1, 9);

% remove points close to the edges
hrespny = zeros(size(hresp));
hrespny(300:400, 300:400) = hresp(300:400, 300:400);
hresp = hrespny;

%figure(1); imagesc(hresp); colorbar;

%threshold and shrink areas to points
hresp = bwmorph(hresp > max(hresp(:))*.90, 'shrink', inf);

% get coordinates of the points, plot them
[cy,cx] = find(hresp);
figure(1); clf; imagesc(I); colorbar; colormap gray; hold on;
plot(cx,cy,'mo');

%select one feature to track
%r = ceil(numel(cy).*rand(1));
%cy = cy(r);
%cx = cx(r);

%plot(cx,cy,'go');
title(sprintf('view%d',0));

for i = 1:stp:6
    pause(0.01)
    J = double(rgb2gray(imread(sprintf('view%d.png',i))));
    J = J(1:2:end,1:2:end);
    figure(i+1); clf; imagesc(J); colorbar; colormap gray; hold on;

    for ti = 1:numel(cy)
        d = LKtrackFunc(I,J,cx(ti),cy(ti),dd,lambda);
        cx(ti) = cx(ti)+d(1);
        cy(ti) = cy(ti)+d(2);
    end

    plot(cx,cy,'go');
    title(sprintf('view%d',i));
    I = J;
    if i==1
        dd = stp*d;
    else
        dd = d;
    end
end

pause()

close all
clear all