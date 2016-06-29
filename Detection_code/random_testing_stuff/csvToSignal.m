function [x,y,z] = csvToSignal(csvInput) 
    x = csvInput(1:end, 1);
    y = csvInput(1:end, 2);
    z = csvInput(1:end, 3);
end