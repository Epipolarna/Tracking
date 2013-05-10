%*******************************************************************
%*                                                                 *
%*                          FILTERLAB                              *
%*                                                                 *
%*       A Matlab package for filter design and analysis           *
%*                                                                 *
%*                    Carl-Johan Westelius                         *
%*                      cawe@isy.liu.se                            *
%*                                                                 *
%*                 Computer Vision Laboratory                      *
%*               Dept. of Electrical Engineering                   *
%*                   Linkoeping University                         *
%*                    S-581 83 LINKOEPING                          *
%*                          SWEDEN                                 *
%*                                                                 *
%*******************************************************************
%
%
%FILTERLAB was created in order to give undergraduate students a 
%possibility to develop an intuitive feeling for the basic Fourier
%transform theorems. 
%
%How to get started
%------------------
%In order to run FILTERLAB copy the .m-files to your matlab directory 
%and start matlab. Initialize the environment using the setup macro:
%
%>> CVlabinit
%
%All functions contain there own manuals which can be accesed by typing:
%
%>> help <function>
%
%-------------------------------------------------------------------

global CVLABINIT

CVLABINIT=1;

global FD
FD=figure('Name','Frequency Domain','NumberTitle', 'off', ...
          'Position',[10 337 512 512]);
colormap(goptab);

global SD
SD=figure('Name','Spatial Domain','NumberTitle', 'off',...
          'Position',[530 337 512 512]);
colormap(goptab);

