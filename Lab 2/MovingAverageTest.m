x = zeros(1,20);

for i = 1:length(x)
    x(i) = sin(i);
%     x(i) = i^2;
%     x(i) = i;
end

windowSize = 10;

movingAverage = zeros(1,length(x));

for i = 1:length(x)
    if i<windowSize
        movingAverage(i) = sum(x(1:i))/windowSize;
    else
        movingAverage(i) = sum(x(i-windowSize+1:i))/windowSize;
    end
end

disp('   input   movingAvg');
disp([x' movingAverage']);

