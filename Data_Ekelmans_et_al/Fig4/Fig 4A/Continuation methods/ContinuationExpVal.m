function NU = ContinuationExpVal(J_,nu0,r_,mu0,tau_,delta,MaxIt)
    options=optimoptions('fsolve','FunctionTolerance', 1.0e-5, 'StepTolerance',1.0e-5,'MaxFunctionEvaluations',3000,'Display','off');
    load F.mat F;
    global sigmaX tau J mu nu r V ff C tauSyn
    C=[0.0625*3000	0.1933*1000	0.275*3000	0.1054*1000];%C=[190   190   830   110];
    ff=F;
    sigmaX=[3 3];
    r=r_;
    tau=tau_;
    J=J_./C;
    J=J;
    C=[C(1:2);C(3:4)];    
    J=[J(1:2);J(3:4)];
    mu=mu0;
    it=0;
    NU=zeros(3,MaxIt+1);
    nu=fsolve(@solve_Phi,[nu0(1) nu0(2)],options);
    NU(:,it+1)=[nu mu]';
    V=[1,0]*1;
    V=V/norm(V);
    it=1;
    cos=0;
    P=1e-3;
    tauSyn=0.25;
    while it<MaxIt
        if abs(mu)>max(15,0.06*sqrt(4e3))
            NU=NU(:,1:it-1);
            break
        end
        if max(nu)>15
            NU=NU(:,1:it-1);
            break
        end
        if it/10-floor(it/10)==0
            disp([it,mu,nu(1),nu(2)])
        end
        nu(1)=nu(1)+V(2)*delta*(P*exp(cos*log(1/P)));%guesses based on V
        mu=mu+V(1)*delta*(P*exp(cos*log(1/P)));
        sol=fsolve(@solve_Phi_continuation,[NU(1,it) NU(2,it) NU(3,it)],options);
        NU(:,it+1)=sol';
        nu=[sol(1) sol(2)];
        mu=sol(3);
        Vnew=[NU(3,it+1)-NU(3,it) NU(1,it+1)-NU(1,it)];%newV
        cos=abs((V(1)*Vnew(1)+V(2)*Vnew(2))/norm(V)/norm(Vnew));
        V=Vnew;
        V=V/norm(V);
        it=it+1;
    end
	disp(['iterations',num2str(it)])
end

    function F=solve_Phi(nu)
    global sigmaX tau J C mu r ff
    F(1) = nu(1)-getnu(sqrt(J(1,1)^2*nu(1)*C(1,1)+J(1,2)^2*nu(2)*C(1,2)+sigmaX(1)^2),tau(1),J(1,1)*nu(1)*C(1,1)+J(1,2)*nu(2)*C(1,2)+mu,ff);
    F(2) = nu(2)-getnu(sqrt(J(2,1)^2*nu(1)*C(2,1)+J(2,2)^2*nu(2)*C(2,2)+sigmaX(2)^2),tau(2),J(2,1)*nu(1)*C(2,1)+J(2,2)*nu(2)*C(2,2)+r*mu,ff);
    end
    
    function F=solve_Phi_continuation(X)
    global sigmaX tau J C r V nu mu ff tauSyn
    a1=tauSyn*max(X(1),1e-3);%In case nu is very close to 0, we cap nu*tau to 
    a2=tauSyn*max(X(2),1e-3);
    F(1) = X(1)-getnu(sqrt(J(1,1)^2*X(1)*C(1,1)*(1-a1*log(1+1/a1))+J(1,2)^2*X(2)*C(1,2)*(1-a1*log(1+1/a1))+sigmaX(1)^2),tau(1),J(1,1)*X(1)*C(1,1)+J(1,2)*X(2)*C(1,2)+X(3),ff);
    F(2) = X(2)-getnu(sqrt(J(2,1)^2*X(1)*C(2,1)*(1-a2*log(1+1/a2))+J(2,2)^2*X(2)*C(2,2)*(1-a2*log(1+1/a2))+sigmaX(2)^2),tau(2),J(2,1)*X(1)*C(2,1)+J(2,2)*X(2)*C(2,2)+r*X(3),ff);
    F(3) = X(3)*V(1)-(mu*V(1)+(nu(1)-X(1))*V(2));%V1=mu0-mu_-1;V2=nu0-nu_-1
    end
    
%     
% [x, fval, exitflag, output] = fsolve(...)
% if exitflag <= 0
%     return
% end

% load('temp.mat')
% fid=fopen('Case8N8.txt','w');
% for N=4e3*10.^[2.0:0.1:2.1];
% NU = Continuation_FiniteSize(Js(8,:),[0 0],r(8),0,10000,N,0.05);
% fprintf(fid,'%g\t',NU(1,1:end-1));
% fprintf(fid,'\n');
% fprintf(fid,'%g\t',NU(2,1:end-1));
% fprintf(fid,'\n');
% fprintf(fid,'%g\t',NU(3,1:end-1));
% fprintf(fid,'\n');
% end
% fclose(fid);