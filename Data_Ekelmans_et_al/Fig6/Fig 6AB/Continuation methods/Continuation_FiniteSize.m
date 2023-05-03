function NU = Continuation_FiniteSize(J_,nu0,r_,mu0,tau_,delta,MaxIt,N)
    options=optimoptions('fsolve','FunctionTolerance', 1.0e-5, 'StepTolerance',1.0e-5,'MaxFunctionEvaluations',3000,'Display','off');
    load F.mat F;
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
    V=[1,0]*1;
    V=V/norm(V);
    it=1;
    while it<MaxIt
        if abs(mu)>0.75*sqrt(N)
            NU=NU(:,1:it-1);
            break
        end
        if nu(1)>3
            NU=NU(:,1:it-1);
            break
        end
        if (mu>35 && nu(1)<1e-4)
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