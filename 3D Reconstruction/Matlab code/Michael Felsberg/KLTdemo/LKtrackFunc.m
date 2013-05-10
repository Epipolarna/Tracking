function dtot = LKtrackFunc(I,J,x,y,dd,lambda);
kernelsize = 11;
sigma = 2;

[dx, dy, Ig, Jg] = gradient_function(I, J, sigma, kernelsize);

% center region on integer value
x = round(x);
y = round(y);

xrad=20; yrad=20;
dtot = [0 0]';

iters = 1; d = 1;
while( iters < 10 && norm(d) > 0.00001)
  dtot = dtot + lambda*dd;
  dxt = interpolation_function(dx,dtot);
  dyt = interpolation_function(dy,dtot);
  Jgt = interpolation_function(Jg,dtot);
    
  Z = estimate_Z(dxt, dyt, x, y, xrad, yrad);
  e = estimate_e(Ig, Jgt, dxt, dyt, x, y, xrad, yrad);

  d = Z\e;
  
  dtot = dtot + d;
  plot(dtot(1)+x,dtot(2)+y,'x'); hold on;
  disp(sprintf('Iteration %d: d: %0.7g, %0.7g', iters, dtot(1), dtot(2)));
  iters = iters +1;
end

end
