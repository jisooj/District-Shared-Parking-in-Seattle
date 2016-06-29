function csvPlot(csvInput, name)
    f = figure;
    set(f,'name',name);
    x = csvInput(1:end, 1);
    y = csvInput(1:end, 2);
    z = csvInput(1:end, 3);
    plot(x)
    hold on
    plot(y)
    hold on
    plot(z)
    legend('x','y','z')
end