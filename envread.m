%% Environmental Data Collection
% Collects data from environmental sensor CSV and plots into a visual
% format.
% Jeremy Cwik 2/8/2024

%% Initialization
clear; clc
rowspace = 8;
[filename, path] = uigetfile(".csv");  % Prompts to get file
C = readcell([path filename]);          % Read file to variable C
cellraw = C;
C = cell2mat(C(:, 2));     % Converts cell into matrix
temp = C(1:rowspace:length(C));    % Stores temperature data into a variable
pressure = C(2:rowspace:length(C));    % Stores pressure data into a variable
alt = C(3:rowspace:length(C));     % Store altitude data into a variable
magx = C(4:rowspace:length(C));    % Stores mag data into a variable
magy = C(5:rowspace:length(C));
magz = C(6:rowspace:length(C));
iterations = length(C(7:rowspace:length(C)));
elapsedtime = C(8:rowspace:length(C));
timesec = sum(diff(elapsedtime));
date = seconds(timesec);
date.Format = 'hh:mm:ss';
datestring = strcat("Time Elapsed: ", string(date), " (hr:min:sec)");
msgbox(datestring, "Time")

grid on

%% Temperature Plotting
plot(1:length(temp), temp, "DisplayName", "Temperature")
xlabel("Iterations");
ylabel("Temperature in C")
title("Temperature")
figure

%% Pressure Plotting
grid on
plot(1:length(pressure), pressure, "DisplayName", "Pascals")
xlabel("Iterations")
ylabel("Pressure in Pascals")
title("Pressure")
figure

%% Altitude Plotting
grid on
plot(1:length(alt), alt, "DisplayName", "Altitude")
title("Altitude")
xlabel("Iterations")
ylabel("<unit>")
figure

%% Magnetometer Plotting
hold on; grid on
plot(1:length(magx), magx, "DisplayName", "Mag X")
plot(1:length(magy), magy, "DisplayName", "Mag Y")
plot(1:length(magz), magz, "DisplayName", "Mag Z")
title("Magnetometer")
xlabel("Iterations")
ylabel("<unit>")

%% End Code
legend
fprintf("End:   %.100s\n", duration)
fprintf("%.40s\n", datestring)
fprintf("Total Iterations: %.i\n", iterations)
