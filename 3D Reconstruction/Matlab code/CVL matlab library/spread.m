function spreadfilter = spread(filter);
% spreadfilter = spread(filter);
% Generate spred kernel version of filter (zeros inserted)
% Morgan Ulvklo 971108

[ysize xsize] = size(filter);
spreadfilter(1:2:(ysize*2-1),1:2:(xsize*2-1)) = filter;