function resp = harris(I, sigma, kernelsize);

kappa = 0.05;
orient = orient_tensor(I, sigma, kernelsize);

%                     det(Z)                                          trace(Z)^2
%      r11            r22              r12                        r11           r22  
resp = orient(:,:,1).*orient(:,:,3) - orient(:,:,2).^2 - kappa .* (orient(:,:,1)+orient(:,:,3)).^2;