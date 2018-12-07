clc;
clear all;

a = 1;
b = 7;
n = 30;

h = (b-a)/n;
h2 = h/2;
h6 = h/6;

x = a:h:b;
y = zeros(1,n+1);

y(1) = 0;
f = @(t,r) sin(-t)-0.4*r;

for i=1:n
    k_1 = f(x(i),    y(i));
    k_2 = f(x(i)+h2, y(i)+k_1*h2);
    k_3 = f(x(i)+h2, y(i)+k_2*h2);
    k_4 = f(x(i)+h,  y(i)+k_3*h);

    y(i+1) = y(i) + h6*(k_1+2*k_2+2*k_3+k_4);
end

plot(x, y, x, f(x, 1));
