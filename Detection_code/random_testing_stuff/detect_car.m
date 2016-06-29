close all;
clear all;
clc;

test1 = load('fast.txt');
test2 = load('overandback.txt');
test3 = load('slowandstop.txt');
test4 = load('totheright.txt');

% csvPlot(test1, 'fast');
% csvPlot(test2, 'over and back');
csvPlot(test3, 'slow and stop');
% csvPlot(test4, 'to the right');

[x,y,z] = csvToSignal(test3);

a = 5;
b = [1 1 1 1 1];
avgX = filter(b, a, x);
avgY = filter(b, a, y);
avgZ = filter(b, a, z);

imp = [-1; 1];
csvPlot([avgX, avgY, avgZ], 'slow and stop');
avgX = conv(avgX, imp);
avgY = conv(avgY, imp);
avgZ = conv(avgZ, imp);
csvPlot([avgX(1:end-1).^2, avgY(1:end-1).^2, avgZ(1:end-1).^2], 'slow and stop');
hold on;
plot(10000*ones(469,1))

% xfft = abs(fftshift(fft(z(300:350))));
% w = linspace(-2*pi,2*pi,length(xfft));
% figure;
% plot(w,xfft)

% [x,y,z] = csvToSignal(test2);
% [x,y,z] = csvToSignal(test3);
% [x,y,z] = csvToSignal(test4);

