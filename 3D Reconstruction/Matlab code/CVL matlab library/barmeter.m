function barmeter(x);

%BARMETER(X) displays a meter with a bar of height 0 <= X <= 1. A new window
%for the barmeter is initialized with argument 'i' (or 'init') and the
%window is closed with argument 'c' (or 'close').
%Note: If there is a plot command in the loop, remember to use the FIGURE
%command.
%
%Example:
%
%barmeter('init')
%for f = 1:n
%  calculations ...
%  if rem(f,n/10) == 0 % mark every 10:th iteration
%    barmeter(f/n);
%  end
%end
%barmeter('close')
%
%Author: Magnus Borga, Dept of EE,
%        Linköping University, Sweden.

global barmeterhandle;
global lastbarmetervalue;

if x(1) == 'i'
  
  % --- initiate bar window ---
  
  barmeterhandle = figure;
  position = [1040 526 100 350];
  set(barmeterhandle,'Position',position)
  lastbarmetervalue = 0;
else
  
  if x(1) == 'c'
  
    % --- close bar window

    delete(barmeterhandle)
    clear barmeterhandle lastbarmetervalue
  else
    
    % --- draw bar ---
    fighandle = gcf;
    if fighandle ~= barmeterhandle
      figure(barmeterhandle);
    end
    fill ([0,1,1,0],[0,0,x,x],[127/255 1 212/255]);
    axishandle = get(barmeterhandle,'Children');
    axis ([0 1 0 1])
    set(axishandle,'XTickLabels','')
    set(axishandle,'YTickLabels','')
    if lastbarmetervalue == 0
      lastbarmetervalue = x;
      tic
    else
      t = round(toc/(x - lastbarmetervalue)*(1-x));
      tic
      lastbarmetervalue = x;
      h = floor(t/3600);
      m = floor(rem(t,3600)/60);
      s = rem(rem(t,3600),60);
      if h > 0
	tid = [num2str(h) 'h ' num2str(m) 'm'];
      else
	tid = [num2str(s) 's'];
	if m > 0
	  tid = [num2str(m) 'm ' tid];
	end
      end
      title([tid,' kvar'])
    end
    drawnow
    if fighandle ~= barmeterhandle
      figure(fighandle);
    end
  end
end
