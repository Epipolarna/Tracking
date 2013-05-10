function out=plotsig(sigarray, plotmode, viewmatrix) 
% 
% out=plotsig(sigarray, plotmode, viewmatrix/magthresh)  
%
% Plotsig interprets a 1D array as a spatial representation and plots 
% it in 2D or 3D. A global variable SD is expected to define which 
% figure window to use.
%
% NOTE! For even sized arrays the point x=1+N/2), i.e the rightmost 
% element of the two center elements, is considered to correspond to 
% the origin.
% 
% Parameter  Range      Default      Description
% =========  =========  ===========  =================================
% sigarray      -           -        Any 1D array. 
% plotmode    c, p, 3      'c'       Cartesian, polar or 3D plot. 
% viewmatrix    -           -        Viewpoint spec. see Plot.
% magthresh     -           0        Magnitude threshold for 'p' plot.
% ====================================================================
%
% See also PLOTDFT, VIEWMTX
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se

out=[];
global SD
figure(SD)

if nargin < 1
   help plotsig
   return
end;

[rows,cols]=size(sigarray);

if (rows == 0)|(cols == 0)
   fprintf(1,'  ERROR! Empty array!!!!!!!!!!!!')
   return
end;

if (rows ~= 1)&(cols ~= 1)
   fprintf(1,'  ERROR! Only one dimensional arrays!!!!!!!!!!!!')
   help plotsig
   return
end;

if nargin < 2
   plotmode = 'c';
end;

if plotmode == 'p'
  if (nargin < 3)
      viewmatrix= -1.0;
  elseif size(viewmatrix) ~= [1 1]
      viewmatrix= -1.0;
  end
else
  if nargin < 3
      viewmatrix=viewmtx(-37.5,30,0);
  elseif size(viewmatrix) ~= [4 4]
      viewmatrix= viewmtx(-37.5,30,0);
  end
end

nelem=max([rows cols]);

xstart=-floor(nelem/2);

if  rem(nelem,2)==1 % if odd size
   xstop = -xstart;
else
   xstop = -xstart-1;
end

x=xstart:xstop;

if plotmode(1) == 'c' | plotmode(1) == 'C'
   upper=real(sigarray);
   lower=imag(sigarray);
   totmin=min(min([upper lower]));
   totmax=max([max([upper lower]) totmin+realmin]);
   subplot(2,1,1)
   plot(x,upper)
   axis([xstart xstop totmin totmax]);
   grid
   xlabel('Spatial Position')
   ylabel('Real Part')
   subplot(2,1,2)
   plot(x,lower)
   axis([xstart xstop totmin totmax]);
   grid
   xlabel('Spatial Position')
   ylabel('Imaginary Part')
elseif plotmode(1) == 'p' | plotmode(1) == 'P'
      upper=abs(sigarray);
      lower=sigarray;
      subplot(2,1,1)
      plot(x,upper)
      axis([xstart xstop 0 max(abs(sigarray))+realmin]);
      grid
      xlabel('Spatial Position')
      ylabel('Magnitude')
      subplot(2,1,2)
      plot_phase_segments(x,lower,viewmatrix)
      axis([xstart xstop -pi pi]);
      grid
      xlabel('Spatial Position')
      ylabel('Argument')
   elseif plotmode(1) == '3'
         upper=real(sigarray);
         lower=imag(sigarray);
         totmin=min(min([upper lower]));
         totmax=max([max([upper lower]) totmin+realmin]);
         subplot(1,1,1)
         plot3(x,upper,lower)
         hold on
         plot3([xstart xstop],[0 0], [0 0], 'r');
         plot3([0 0], [totmin totmax], [0 0],'g');
         plot3([0 0], [0 0], [totmin totmax],'b');
         axis([xstart xstop totmin totmax totmin totmax]);
         grid
         xlabel('Spatial Position')
         ylabel('Real Part')
         zlabel('Imaginary Part')
         hold off
         view(viewmatrix);
       else
         fprintf(1,'ERROR! Illegal plot mode specification')
         help plotsig
         return
end   

out=sigarray;


