function Im_supp = non_max_suppression(Im,wsz)

% Im_supp = non_max_suppression(Im,wsz)
%
% Calculates non-maximum suppression, i.e. the value at a certain
% position is set to zero if it is not the maximal value within a square
% window of size wsz.
%
% Im_supp - M/N matrix
%
%      Im - M/N matrix
%     wsz - size of suppression window
%           (default wsz = 3)
% 
%
%
%         Björn Johansson
%         Computer Vision Laboratory
%         Linköping University, Sweden
%         bjorn@isy.liu.se
%

error('non_max_suppression is implemented as a mex-file. It does not seem to be available on this platform.')

