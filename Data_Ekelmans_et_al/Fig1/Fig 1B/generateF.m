z=sym(-100:0.0001:10);%range of z over which f is evaluated
f=exp(z.^2).*(1+erf(z));
f=double(f);
z=-100:0.0001:10;

%trapezoidal integration of f
I=0.5*(f(1:end-1)+f(2:end));
F=[0 cumsum(I)];
F=F*1e-4;
