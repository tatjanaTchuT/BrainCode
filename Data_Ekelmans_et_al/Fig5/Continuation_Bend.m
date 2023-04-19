function S = Continuation_Bend(J,delta,MaxIt,axlim)
    options=optimoptions('fsolve','FunctionTolerance', 1.0e-10, 'StepTolerance',1.0e-10,'MaxFunctionEvaluations',300,'Display','off');

    global A B C D E G H I mu r z V
    
    
    Jee=J(1,1);
    Jei=abs(J(1,2));
    Jie=J(2,1);
    Jii=abs(J(2,2));
    a=[1.079e-04,2.210e-06];b=[-11.14,4.77];n=[3.081,3.815];
    ne=n(1); ni=n(2); ae=a(1); ai=a(2); be=b(1); bi = b(2);
    A=Jee*ae;
    B=Jei*ai;
    C=ae*(Jei*Jie-Jee*Jii)/Jei;
    D=Jii/Jei;
    G=D*be-bi;
    H=be;
    
    E=ne;
    I=ni;
    
    
    S=zeros(3,MaxIt+1);
    
    z=(A*E)^(-1/(E-1));
    mu=z-A*z^E+H;
    r=D-(C*z^E+D*z+G)/(mu);
    
    it=0;
    S(:,it+1)=[mu r-1 z]';
    it=1;
	S(:,it+1)=[mu r z]';
    
    
    V=[0, 1, 0]; %dmu dr dz 
    V(3)=V(3)*5;% rescaling: dz can be 5 times larger than dmu or dr 
    V=V/norm(V);
    for it=1:MaxIt
%         if (r<S(2,1) || r>axlim (4))
        if ( (r<axlim(3)&&V(2)<0) || r>axlim (4))
            S=S(:,1:it);
            break
        end
        if (mu>axlim(2) || mu<axlim(1))
            S=S(:,1:it);
            break
        end
        
        mu=mu+V(1)*delta;
        r=r+V(2)*delta;
        z=z+V(3)*delta;

        if isnan(r)
            break
        end
        
        sol=fsolve(@solve_SSN_continuation,[S(1,it) S(2,it) S(3,it)],options);
        S(:,it+1)=sol';
        mu=sol(1);
        r=sol(2);
        z=sol(3);
        V=[S(1,it+1)-S(1,it) S(2,it+1)-S(2,it) S(3,it+1)-S(3,it)];
        V(3)=V(3)*5;% rescaling: dz=5dr on average
        V=V/norm(V);
        
    end
end



function F=solve_SSN_continuation(X)
global A B C D E G H I r z mu V
F(1) = A*max(0,X(3))^E - B*max(0, (C*max(0,X(3))^E+D*X(3)+(X(2)-D)*X(1)+G) )^I-X(3)+X(1)-H; %F(z)=0
F(2) = 1 - A*E*max(0,X(3)).^(E-1) + B*I* (C*E*max(0,X(3)).^(E-1)+D) .* max(0, (C*max(0,X(3)).^E+D*X(3)+(X(2)-D)*X(1)+G)).^(I-1); %F'(z)=0
F(3) = V(1)*(X(1)-mu)+V(2)*(X(2)-r)+V(3)*(X(3)-z);
end

%S=Continuation_edge(J,0.001,1e5,axlim);