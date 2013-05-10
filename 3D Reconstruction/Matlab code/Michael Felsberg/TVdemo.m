function fn = TVdemo(f0,alpha,lambda)

% f0 = im2double(imread('newboat.png'));
% fn = TVdemo(f0,0.001,0.2);
% imshow(abs(fn-f0),[])
% f0 = im2double(imread('newboatGN.png'));
% fn = TVdemo(f0,0.0005,0.5);
% imshow(abs(fn-f0),[])

h=[1 2 1]/4;
f=f0;
[fx,fy]=gradient(f);
fx=conv2(fx,h','same');
fy=conv2(fy,h,'same');
fa=sqrt(eps+fx.^2+fy.^2);
[dx,dummy]=gradient(fx./fa);
[dummy,dy]=gradient(fy./fa);
dx=conv2(dx,h','same');
dy=conv2(dy,h,'same');
fn=f-alpha*(f-f0-lambda*(dx+dy));
itn=0;
while max(fn(:)-f(:))>2*alpha*lambda
    imshow(fn);
    title(['iteration ' num2str(itn)]);
    itn = itn + 1;
    drawnow();
    f=fn;
    [fx,fy]=gradient(f);
    fa=sqrt(eps+fx.^2+fy.^2);
    [dx,dummy]=gradient(fx./fa);
    [dummy,dy]=gradient(fy./fa);
    fn=f-alpha*(f-f0-lambda*(dx+dy));
end