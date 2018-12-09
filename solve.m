function solve(a, b, n)
    h = (b-a)/n;
    
    z = a:h:b;
    y = zeros(1,n+1);
    x = zeros(1,n+1);
    
    y(1) = 0;
    x(1) = 0;
    
    for i=1:n
        k1 = h*g1(z(i),       y(i),        x(i));
        l1 = h*g2(z(i),       y(i),        x(i));

        k2 = h*g1(z(i)+(h/2), y(i)+(k1/2), x(i)+(l1/2));
        l2 = h*g2(z(i)+(h/2), y(i)+(k1/2), x(i)+(l1/2));

        k3 = h*g1(z(i)+(h/2), y(i)+(k2/2), x(i)+(l2/2));
        l3 = h*g2(z(i)+(h/2), y(i)+(k2/2), x(i)+(l2/2));

        k4 = h*g1(z(i)+h,     y(i)+k3,     x(i)+l3);
        l4 = h*g2(z(i)+h,     y(i)+k3,     x(i)+l3);
    
        y(i+1) = y(i) + (k1+2*k2+2*k3+k4)/6;
        x(i+1) = x(i) + (l1+2*l2+2*l3+l4)/6;
    end
    
    plot(z, y);
end;

function result = g1(z, y, x)
    result = x;
end;

function result = g2(z, y, x)
    E = 1.08E8;
    I = 0.05;
    L = 29;
    result = f(z)*(L-z)/2*E*I;
end;

function result = f(z)
    result = 200*z*exp(-2*z/30)/(5+z);
end;
