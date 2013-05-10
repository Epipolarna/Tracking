function figureinit(n, cl, del)

% figureinit(n,cl)
%
% creates n figures in suitable positions
% n = 2, 3, 4, 5, 6 or 7
%
% if cl=1 then clear all figures (default cl = 0)
% if del=1 then delete all other figures (default del = 1)
%
%
% Björn Johansson
% CVL, Linköpings universitet
% 1999


if nargin<2,
    cl=0;
end
if nargin<3,
    del=1;
end

if (n<2) | (n>7),
    error('Not a valid number of figures!');
end

units = 'pixels';
su = get(0,'Units');
set(0,'Units',units);
sz = get(0,'ScreenSize');
set(0,'Units',su);

d = 20;h = 110;
% cases n = 2, 3, 4, 5
fsz1_x = (sz(3)-5*d)/4;
fsz1_y = (sz(4)-h)*0.33;
pos1_x = [d fsz1_x+2*d 2*fsz1_x+3*d 3*fsz1_x+4*d];
pos1_y = [h h+d+80+fsz1_y];

d = 15;h = 110;
% cases n = 6, 7
fsz2_x = (sz(3)-6*d)/5;
fsz2_y = (sz(4)-h)*0.33;
pos2_x = [d fsz2_x+2*d 2*fsz2_x+3*d 3*fsz2_x+4*d 4*fsz2_x+5*d];
pos2_y = [h h+d+80+fsz2_y];

if n==2
    figure(1);
    set(1,'position',[pos1_x(1) pos1_y(1) fsz1_x fsz1_y],'Units',units);
    figure(2);
    set(2,'position',[pos1_x(2) pos1_y(1) fsz1_x fsz1_y],'Units',units);
elseif n==3
    figure(1);
    set(1,'position',[pos1_x(1) pos1_y(1) fsz1_x fsz1_y],'Units',units);
    figure(2);
    set(2,'position',[pos1_x(2) pos1_y(1) fsz1_x fsz1_y],'Units',units);
    figure(3);
    set(3,'position',[pos1_x(3) pos1_y(1) fsz1_x fsz1_y],'Units',units);
elseif n==4
    figure(1);
    set(1,'position',[pos1_x(1) pos1_y(1) fsz1_x fsz1_y],'Units',units);
    figure(2);
    set(2,'position',[pos1_x(2) pos1_y(1) fsz1_x fsz1_y],'Units',units);
    figure(3);
    set(3,'position',[pos1_x(3) pos1_y(1) fsz1_x fsz1_y],'Units',units);
    figure(4);
    set(4,'position',[pos1_x(4) pos1_y(1) fsz1_x fsz1_y],'Units',units);
elseif n==5
    figure(1);
    set(1,'position',[pos1_x(1) pos1_y(1) fsz1_x fsz1_y],'Units',units);
    figure(2);
    set(2,'position',[pos1_x(2) pos1_y(1) fsz1_x fsz1_y],'Units',units);
    figure(3);
    set(3,'position',[pos1_x(3) pos1_y(1) fsz1_x fsz1_y],'Units',units);
    figure(4);
    set(4,'position',[pos1_x(4) pos1_y(1) fsz1_x fsz1_y],'Units',units);
    figure(5);
    set(5,'position',[pos1_x(4) pos1_y(2) fsz1_x fsz1_y],'Units',units);
elseif n==6
    figure(1);
    set(1,'position',[pos2_x(1) pos2_y(1) fsz2_x fsz2_y],'Units',units);
    figure(2);
    set(2,'position',[pos2_x(2) pos2_y(1) fsz2_x fsz2_y],'Units',units);
    figure(3);
    set(3,'position',[pos2_x(3) pos2_y(1) fsz2_x fsz2_y],'Units',units);
    figure(4);
    set(4,'position',[pos2_x(4) pos2_y(1) fsz2_x fsz2_y],'Units',units);
    figure(5);
    set(5,'position',[pos2_x(5) pos2_y(1) fsz2_x fsz2_y],'Units',units);
    figure(6);
    set(6,'position',[pos2_x(5) pos2_y(2) fsz2_x fsz2_y],'Units',units);
elseif n==7
    figure(1);
    set(1,'position',[pos2_x(1) pos2_y(1) fsz2_x fsz2_y],'Units',units);
    figure(2);
    set(2,'position',[pos2_x(2) pos2_y(1) fsz2_x fsz2_y],'Units',units);
    figure(3);
    set(3,'position',[pos2_x(3) pos2_y(1) fsz2_x fsz2_y],'Units',units);
    figure(4);
    set(4,'position',[pos2_x(4) pos2_y(1) fsz2_x fsz2_y],'Units',units);
    figure(5);
    set(5,'position',[pos2_x(5) pos2_y(1) fsz2_x fsz2_y],'Units',units);
    figure(6);
    set(6,'position',[pos2_x(4) pos2_y(2) fsz2_x fsz2_y],'Units',units);
    figure(7);
    set(7,'position',[pos2_x(5) pos2_y(2) fsz2_x fsz2_y],'Units',units);	
end

if cl==1,
    for k=1:n,
	figure(k);
	clf;
    end
end

if del==1,
    for k=n+1:7,
	if ishandle(k),
	    delete(k);
	end
    end
end
