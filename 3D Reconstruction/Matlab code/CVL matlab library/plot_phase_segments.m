function plot_phase_segments(xarray,sigarray, magthresh,linetype) 
% 
% plot_phase_segments(xarray,sigarray, magthresh,linetype)
%
% plot_phase_segments plots the phase for magnitudes above the 
% threshold. 
%
% Parameter  Range      Default      Description
% =========  =========  ===========  =================================
% xarray        -           -        1D array containing x values. 
% sigarray      -           -        Any 1D array. 
% magthresh     -           0        Magnitude threshold          
% linetype      -          '-'       Line type (see PLOT)          
% ====================================================================
%
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se

if nargin < 2
   help plot_phase_segments
   return
end;

[rows,cols]=size(sigarray);

if (rows == 0)|(cols == 0)
   fprintf(1,'  ERROR! Empty array!!!!!!!!!!!!')
   return
end;

if (rows ~= 1)&(cols ~= 1)
   fprintf(1,'  ERROR! Only one dimensional arrays!!!!!!!!!!!!')
   help plot_phase_segments
   return
end;

if nargin < 3
   magthresh = -1.0;
end;

if nargin < 4
   linetype='-';
end;

[xrows, xcols]=size(xarray);
if (rows ~= xrows)|(cols ~= xcols)
   fprintf(1,'  ERROR! xarray and sigarray must of same size')
   help plot_phase_segments
   return
end;

nelem=max([rows cols]);

x=[];
y=[];

t=(abs(sigarray)>magthresh);

for i=1:nelem,
   if t(i)==1 
     x=[x xarray(i)];
     y=[y sigarray(i)];
   elseif max(size(x)) > 0,
      plot(real(x),angle(y),linetype);
      hold on
      x=[];
      y=[];
   end
end

if max(size(x)) > 0,
   plot(real(x),angle(y),linetype);
end

hold off
