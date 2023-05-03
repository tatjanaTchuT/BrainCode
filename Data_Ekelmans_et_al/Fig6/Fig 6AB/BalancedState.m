function NU = BalancedState(J,N,p,mu)
NU=linsolve(J.*p.*[N';N']/(sum(N)),-mu);