clear;clc;
%% load data and Calc the min arr time
Q1 = load('0.csv');
Q2 = load('1.csv');
% constants
MaxV = 16;
MaxA = 3;
Arr1 = zeros(length(Q1),1);
Arr2 = zeros(length(Q2),1);
for i = 1:length(Q1)
    % if can speed up all of the way
    if (MaxV * MaxV - Q1(i,1) * Q1(i,1)) / 2 / MaxA >= Q1(i,2)
        Arr1(i) = (sqrt(2 * MaxA * Q1(i,2) + Q1(i,1) * Q1(i,1)) ...
            - Q1(i,1) * Q1(i,1)) / MaxA;
    else
        Arr1(i) = (MaxV - Q1(i,1)) / MaxA + ...
        (Q1(i,2) - (MaxV * MaxV - Q1(i,1) * Q1(i,1)) / 2 / MaxA) / MaxV;
    end
end
for i = 1:length(Q2)
    % if can speed up all of the way
    if (MaxV * MaxV - Q2(i,1) * Q2(i,1)) / 2 / MaxA >= Q2(i,2)
        Arr1(i) = (sqrt(2 * MaxA * Q2(i,2) + Q2(i,1) * Q2(i,1)) ...
            - Q2(i,1) * Q2(i,1)) / MaxA;
    else
        Arr2(i) = (MaxV - Q2(i,1)) / MaxA + ...
        (Q2(i,2) - (MaxV * MaxV - Q2(i,1) * Q2(i,1)) / 2 / MaxA) / MaxV;
    end
end
Q1 = [Q1,Arr1,zeros(length(Q1),1)];
Q2 = [Q2,Arr2,zeros(length(Q2),1)];
Q = cell(1,2);
Q{1} = Q1;
Q{2} = Q2;
clear Arr1 Arr2 MaxA MaxV i Q1 Q2;


