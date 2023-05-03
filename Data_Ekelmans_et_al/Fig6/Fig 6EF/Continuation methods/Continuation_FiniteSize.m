function NU = Continuation_FiniteSize(J_,nu0,r_,mu0,tau_,delta,MaxIt,N,Vd)
    options=optimoptions('fsolve','FunctionTolerance', 1.0e-3, 'StepTolerance',1.0e-3,'MaxFunctionEvaluations',10000,'Display','off');
    load F.mat F;
    %nu=Continuation_FiniteSize(J2, [0;0],5,0,[0.02,0.01],1e-3,1e5,4e3);
    global sigmaX tau J mu nu r V ff C
    C=[190   190   830   110];
    ff=F;
    sigmaX=[3 3];
    r=r_;
    tau=tau_;
    J=J_./C;
    J=J*sqrt(4e3)/sqrt(N);
    C=[C(1:2);C(3:4)]*N/(4e3);    
    J=[J(1:2);J(3:4)];
    mu=mu0;
    it=0;
    NU=zeros(3,MaxIt+1);
    nu=fsolve(@solve_Phi,[nu0(1) nu0(2)],options);
    NU(:,it+1)=[nu mu]';
    V=[1,0]*Vd;
    V=V/norm(V);
    it=1;
    while it<MaxIt
        if mu>0.8*sqrt(N)
            NU=NU(:,1:it-1);
            break
        end
        if mu<-5
            NU=NU(:,1:it-1);
            break
        end
        if V(2)>0 && nu(1)>15
            NU=NU(:,1:it-1);
            break
        end
        if it/10-floor(it/10)==0
            disp([it,mu,nu(1),nu(2)])
        end
        nu(1)=nu(1)+V(2)*delta;
        mu=mu+V(1)*delta;
        sol=fsolve(@solve_Phi_continuation,[NU(1,it) NU(2,it) NU(3,it)],options);
        NU(:,it+1)=sol';
        nu=[sol(1) sol(2)];
        mu=sol(3);
        V=[NU(3,it+1)-NU(3,it) NU(1,it+1)-NU(1,it)];
        V=V/norm(V);
        it=it+1;
    end
	disp(it)
end

    function F=solve_Phi(nu)
    global sigmaX tau J C mu r ff
    F(1) = nu(1)-getnu(sqrt(J(1,1)^2*nu(1)*C(1,1)+J(1,2)^2*nu(2)*C(1,2)+sigmaX(1)^2),tau(1),J(1,1)*nu(1)*C(1,1)+J(1,2)*nu(2)*C(1,2)+mu,ff);
    F(2) = nu(2)-getnu(sqrt(J(2,1)^2*nu(1)*C(2,1)+J(2,2)^2*nu(2)*C(2,2)+sigmaX(2)^2),tau(2),J(2,1)*nu(1)*C(2,1)+J(2,2)*nu(2)*C(2,2)+r*mu,ff);
    end
    
    function F=solve_Phi_continuation(X)
    global sigmaX tau J C r V nu mu ff
    F(1) = X(1)-getnu(sqrt(J(1,1)^2*X(1)*C(1,1)+J(1,2)^2*X(2)*C(1,2)+sigmaX(1)^2),tau(1),J(1,1)*X(1)*C(1,1)+J(1,2)*X(2)*C(1,2)+X(3),ff);
    F(2) = X(2)-getnu(sqrt(J(2,1)^2*X(1)*C(2,1)+J(2,2)^2*X(2)*C(2,2)+sigmaX(2)^2),tau(2),J(2,1)*X(1)*C(2,1)+J(2,2)*X(2)*C(2,2)+r*X(3),ff);
    F(3) = 100*((X(3)-mu)*V(1)+(X(1)-nu(1))*V(2));%V1=mu0-mu_-1;V2=nu0-nu_-1
    end
