function NU = Continuation_SSN(J_,nu0,r_,mu0_,a_,b_,n_,delta,MaxIt)
    options=optimoptions('fsolve','FunctionTolerance', 1.0e-10, 'StepTolerance',1.0e-10,'MaxFunctionEvaluations',300,'Display','off');

    global J mu nu r V a b n mu0
    %delta=0.05;
    r=r_;
    J=J_;
    J=[J(1:2);J(3:4)];
    mu=mu0_(1);
    mu0=mu0_(2);
    a=a_;
    b=b_;
    n=n_;
    it=0;
    NU=zeros(3,MaxIt+1);
    nu=fsolve(@solve_SSN,[nu0(1) nu0(2)],options);
    NU(:,it+1)=[nu mu]';
    V=[1,0];
    V=V/norm(V);
    for it=1:MaxIt
        if (abs(mu)>100)
            NU=NU(:,1:it);
            break
        end
        if nu(1)>25
            NU=NU(:,1:it);
            break
        end
        nu(1)=nu(1)+V(2)*delta;
        mu=mu+V(1)*delta;
        sol=fsolve(@solve_SSN_continuation,[NU(1,it) NU(2,it) NU(3,it)],options);
        NU(:,it+1)=sol';
        nu=[sol(1) sol(2)];
        mu=sol(3);
        V=[NU(3,it+1)-NU(3,it) NU(1,it+1)-NU(1,it)];
        V=V/norm(V);
    end
end

function F=solve_SSN(nu)
global J mu r a  b n mu0
mue=J(1,1)*nu(1)+J(1,2)*nu(2)+mu;
mui=J(2,1)*nu(1)+J(2,2)*nu(2)+max(mu0,r*mu);
F(1) = nu(1)-a(1)*max(mue-b(1),0)^n(1);
F(2) = nu(2)-a(2)*max(mui-b(2),0)^n(2);
end

function F=solve_SSN_continuation(X)
global J r V nu mu a b n mu0
mue=J(1,1)*X(1)+J(1,2)*X(2)+X(3);
mui=J(2,1)*X(1)+J(2,2)*X(2)+max(r*X(3),mu0);
F(1) = 1*(X(1)-a(1)*max(mue-b(1),0)^n(1));
F(2) = 1*(X(2)-a(2)*max(mui-b(2),0)^n(2));
F(3) = X(3)*V(1)-(mu*V(1)+(nu(1)-X(1))*V(2));
end
