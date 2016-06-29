
% Moving Average Filter
% y[n] = 1/5 (x[n] + x[n-1] + x[n-2] + x[n-3] + x[n-4])
% 5*y[n] = (x[n] + x[n-1] + x[n-2] + x[n-3] + x[n-4])
% 5 Y(z) = X(z) + z^-1 X(z) + z^-2 X(z) + z^-3 X(z) + z^-4 X(z)
% H(z) = Y(z)/X(z) = (1/5) / (1+z^-1+z^-2+z^-3+z^-4)
% a = 5;
% b = [1 1 1 1 1];
% avgX = filter(b, a, x);
% avgY = filter(b, a, y);
% avgZ = filter(b, a, z);
% 
% subplot(3,1,1);
% plot(avgX)
% hold on;
% plot(x)
% 
% subplot(3,1,2);
% plot(avgY)
% hold on;
% plot(y)
% 
% subplot(3,1,3);
% plot(avgZ)
% hold on;
% plot(z)

plot(z)
% hold on
% plot(y)
% hold on
% plot(z)

