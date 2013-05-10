function out=plotdft(dftarray, plotmode, viewmatrix) 
% 
% out=plotdft(dftarray, plotmode, viewmatrix/magthresh) 
%
% Plotdft interprets a 1D array as a frequency representation with 
% -PI < u < PI for odd sized arrays and -PI <= u < PI for even sized 
% arrays. Plots can be made in 2D or 3D. A global variable FD is 
% expected to define which figure window to use.
%
% NOTE! For even sized arrays the point x=1+N/2), i.e the rightmost 
% element of the two center elements, is considered to correspond to 
% the origin.
% 
% Parameter  Range      Default      Description
% =========  =========  ===========  =================================
% dftarray      -           -        Any 1D array. 
% plotmode    c, p, 3      'c'       Cartesian, polar or 3D plot. 
% viewmatrix    -           -        Viewpoint spec. see Plot
% magthresh     -           0        Magnitude threshold for 'p' plot.
% ====================================================================
%
% See also PLOTSIG, VIEWMTX
%
% Author: Carl-Johan Westelius, Dept of EE,
%         Linkoeping University, Sweden.
%         cawe@isy.liu.se

out=[];
global FD
figure(FD)

if nargin < 1
   help plotdft
   return
end;

[rows cols]=size(dftarray);

if (rows == 0)|(cols == 0)
   fprintf(1,'  ERROR! Empty array!!!!!!!!!!!!')
   return
end;

if (rows ~= 1)&(cols ~= 1)
   fprintf(1,'  ERROR! Only one dimensional arrays!!!!!!!!!!!!')
   help plotdft
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

step=2*pi/nelem;

if rem(nelem,2) == 1
   x=-(pi-step/2):step:(pi-step/2); 
else
   x=-pi:step:pi-step; 
end

if plotmode(1) == 'c' | plotmode(1) == 'C'
   upper=real(dftarray);
   lower=imag(dftarray);
   totmin=min(min([upper lower]));
   totmax=max([max([upper lower]) totmin+realmin]);
   h=subplot(2,1,1);
   plot(x,upper);
   axis([-pi pi totmin totmax]);
   grid
   set(h,'XTick',[-3.14 -2.36 -1.57 -0.79 0 0.79 1.57 2.36 3.14])	
   xlabel('Frequency')
   ylabel('Real Part')
   h=subplot(2,1,2);
   plot(x,lower)
   axis([-pi pi totmin totmax]);
   grid
   set(h,'XTick',[-3.14 -2.36 -1.57 -0.79 0 0.79 1.57 2.36 3.14])	
   xlabel('Frequency')
   ylabel('Imaginary Part')
elseif plotmode(1) == 'p' | plotmode(1) == 'P'
      upper=abs(dftarray);
      lower=dftarray;
      h=subplot(2,1,1);
      plot(x,upper)
      axis([-pi pi 0 max(abs(dftarray))+realmin]);
      grid
      set(h,'XTick',[-3.14 -2.36 -1.57 -0.79 0 0.79 1.57 2.36 3.14])	
      xlabel('Frequency')
      ylabel('Magnitude')
      h=subplot(2,1,2);
      plot_phase_segments(x,lower,viewmatrix)
      axis([-pi pi -pi pi]);
      grid
      set(h,'YTick',[-3.14 -1.57 0 1.57 3.14])	
      set(h,'XTick',[-3.14 -2.36 -1.57 -0.79 0 0.79 1.57 2.36 3.14])	
      xlabel('Frequency')
      ylabel('Argument')
   elseif plotmode(1) == '3'
         upper=real(dftarray);
         lower=imag(dftarray);
         totmin=min(min([upper lower]));
         totmax=max([max([upper lower]) totmin+realmin]);
         h=subplot(1,1,1);
         plot3(x,upper,lower)
         hold on
         plot3([-pi pi],[0 0], [0 0], 'r');
         plot3([0 0], [totmin totmax], [0 0],'g');
         plot3([0 0], [0 0], [totmin totmax],'b');
         axis([-pi pi totmin totmax totmin totmax]);
         set(h,'XTick',[-3.14 -2.36 -1.57 -0.79 0 0.79 1.57 2.36 3.14])	
         grid
         xlabel('Frequency')
         ylabel('Real Part')
         zlabel('Imaginary Part')
         hold off
         view(viewmatrix);
       else
         fprintf(1,'ERROR! Illegal plot mode specification')
         help plotdft
         return
end   

out=dftarray;
