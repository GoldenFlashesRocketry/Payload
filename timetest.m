%% Time Counter
% Plots time and records average
% Jeremy Cwik 2024

[filename, path] = uigetfile(".csv");  % Prompts to get file
C = readcell([path filename]);          % Read file to variable C
T = (C(5:4:end, 1))'; % Reads table
time = diff(cell2mat(T)); % Converts cells to numbers
time = rmoutliers(time, "mean"); % Removes outliers
fprintf("Average: %.3f\n", mean(time)') % Prints average to command window
x = 1:length(time);
leg = strcat("Average: ", num2str(mean(time)), " ms"); % Conctenate values
plot(x, time)
legend(leg)
xlabel("# of iterations")
ylabel("Time (ms)")
title("Time per Iteration")