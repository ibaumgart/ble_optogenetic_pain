%% Battery Life Modeling
% Implantable Light Source for Drving Optogenetic Constructs
% BME 402
% 4/21/2018

% Content by: Karam Khateeb

% The purpose of this file is to model the battery life of the device. 
% The current draw of the device was measured under different supply
% voltages during the different states. A conservative model employs the
% use of only 70% of the battery capacity. In this file, the CR1632 and the
% CR2032 batteries are modelled.

clear all;
clc;
close all;

rest = 1.0; % mA current in the resting state

test(1).volt = 3.3; % supplied voltage
test(1).red = 45.6667; % mA current for RED LED
test(1).blue = 16.0 % mA current for BLUE LED

test(2).volt = 3.0;
test(2).red = 37.6667;
test(2).blue = 8.0;

test(3).volt = 2.9;
test(3).red = 34.6667;
test(3).blue = 8.0

test(4).volt = 2.8;
test(4).red = 32.0;
test(4).blue = 7.0;

test(5).volt = 2.7;
test(5).red = 29.3333;
test(5).blue = 6.0;

test(6).volt = 2.6;
test(6).red = 26.6667;
test(6).blue = 5.0;

test(7).volt = 2.5;
test(7).red = 22.8333;
test(7).blue = 5.0;

test(8).volt = 2.4;
test(8).red = 21.0;
test(8).blue = 5.0;

test(9).volt = 2.3;
test(9).red = 18.6667;
test(9).blue = 5.0;

% The duty cycle, assuming that both LEDs are ON for equal times
duty = 0:0.01:1;

CR1632Cap = 140; % mAh capacity of CR1632 battery
CR2032Cap = 225; % mAh capacity of CR2032 battery

for i = 1:9
    test(i).t_life16 = CR1632Cap./((rest.*(1-duty)) + (test(i).red * 0.5 .* duty) + (test(i).blue * 0.5 .* duty));
    test(i).t_life20 = CR2032Cap./((rest.*(1-duty)) + (test(i).red * 0.5 .* duty) + (test(i).blue * 0.5 .* duty));
end

% Plot the t_life vs duty cycle plot for 1632 Battery
figure
for k = 1:9
    plot(duty, test(k).t_life16)
    hold on
end
title('CR1632 Battery Life Expectancy')
xlabel('Duty Cycle')
ylabel('Expected Life (hrs)')

% Plot the t_life vs duty cycle plot for 2032 Battery
figure
for l = 1:9
    plot(duty, test(l).t_life20)
    hold on
end
title('CR2032 Battery Life Expectancy')
xlabel('Duty Cycle')
ylabel('Expected Life (hrs)')
    
% Plot the t_life vs duty cycle plot for 1632 Battery Assuming 70% useful
% capacity
figure
for k = 1:9
    plot(duty, 0.7*test(k).t_life16)
    hold on
end
title('CR1632 70% Battery Life Expectancy')
xlabel('Duty Cycle')
ylabel('Expected Life (hrs)')

% Plot the t_life vs duty cycle plot for 2032 Battery assuming 70% useful
% capacity
figure
for l = 1:9
    plot(duty, 0.7*test(l).t_life20)
    hold on
end
title('CR2032 70% Expected Battery Life (1mA Resting Current)')
xlabel('Duty Cycle')
ylabel('Expected Life (hrs)')
