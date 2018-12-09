
clc;
clear all;

% d^2y/dx^2 = sin(x)

% dy/dx = z = g1(x,y,z)
% dz/dx = sin(x) = g2(x,y,z)

% analitical solution: y = -sin(x)

function result = g1(x, y, z)
    result = z;
end;

function result = g2(x, y, z)
    result = sin(x);
end;

h = 0.1;
x = 0:h:2*pi;
y = zeros(1,length(x));
z = zeros(1,length(x));

y(1) = 0;
z(1) = -1;

for i=1:length(x)-1
    k1 = h*g1(x(i),       y(i),        z(i));
    l1 = h*g2(x(i),       y(i),        z(i));

    k2 = h*g1(x(i)+(h/2), y(i)+(k1/2), z(i)+(l1/2));
    l2 = h*g2(x(i)+(h/2), y(i)+(k1/2), z(i)+(l1/2));

    k3 = h*g1(x(i)+(h/2), y(i)+(k2/2), z(i)+(l2/2));
    l3 = h*g2(x(i)+(h/2), y(i)+(k2/2), z(i)+(l2/2));

    k4 = h*g1(x(i)+h,     y(i)+k3,     z(i)+l3);
    l4 = h*g2(x(i)+h,     y(i)+k3,     z(i)+l3);

    y(i+1) = y(i) + (k1+2*k2+2*k3+k4)/6;
    z(i+1) = z(i) + (l1+2*l2+2*l3+l4)/6;
end

plot(x, y, x, z, x, sin(x));
