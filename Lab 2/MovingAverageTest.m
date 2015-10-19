x = zeros(1,20);

windowSize = 10;

movingAverage = zeros(1,length(x));

for i = 0:length(x)-1
    x(i+1) = i;
end

for i = 1:length(x)
    if i<windowSize
        movingAverage(i) = sum(x(1:i))/windowSize;
    else
        movingAverage(i) = sum(x(i-windowSize+1:i))/windowSize;
    end
end

disp('movingAvg');
disp([movingAverage']);


for i = 0:length(x)-1
    x(i+1) = i*i*(i/(i+1));
end

for i = 1:length(x)
    if i<windowSize
        movingAverage(i) = sum(x(1:i))/windowSize;
    else
        movingAverage(i) = sum(x(i-windowSize+1:i))/windowSize;
    end
end

disp('movingAvg');
disp([movingAverage']);

