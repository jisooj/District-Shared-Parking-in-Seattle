function [x] = barker(m);
%function [x] = Barker(m);
% barker codes only for 
%m=2,3,4,5,7,11,13 ONLY, else Error
if(m==1); x = [1]';end
if(m==2); x = [1 0]';end
if(m==3); x = [1 1 0]';end
if(m==4); x = [1 1 0 1]';end
if(m==5); x = [1 1 1 0 1]';end
if(m==7); x = [1 1 1 0 0 1 0]';end
if(m==11);x = [1 1 1 0 0 0 1 0 0 1 0]';end
if(m==13);x = [1 1 1 1 1 0 0 1 1 0 1 0 1]';end
 x = -(-1).^x;    % Make 0 into 1