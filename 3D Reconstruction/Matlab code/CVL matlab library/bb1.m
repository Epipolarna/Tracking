% function [r,x] = bb1(y)
%
% Generates a new x= x_k+1= f({x_i}), i<=k and a corresponding reward
% r= r(x_k+1,y) such that max{r} is given by y= x_k+1, i.e. a correct
% prediction of the next x. Your mission is to obtain sum(r) > 100.
%
% The dimensions of y and x should be 100 x 1 or 1 x 100.
%
% If you get the error message "Logging failed, exiting..." please
% contact tc@isy.liu.se or knutte@isy.liu.se