function [nu_E_finite,nu_I_finite,...
          mu_E_finite,mu_I_finite,...
          sigma_E_finite,sigma_I_finite] = ComputeSteadyStates(params)

    %************************************************************
    % Numerical parameters 
    %************************************************************    

    J_EE = abs(params.J_EE);
    J_EI = abs(params.J_EI);
    J_IE = abs(params.J_IE);
    J_II = abs(params.J_II);

    N    = params.N;
    VR   = params.V_R;
    tau_m = params.tau_m;

    mu_E_ext = params.mu_E_ext; 
    mu_I_ext = params.mu_I_ext; 

    mu_E_ext_sigma = params.mu_E_ext_sigma; 
    mu_I_ext_sigma = params.mu_I_ext_sigma;
    
    N_E = params.N_E;
    N_I = params.N_I;
    
	connProb = params.conn_0_0;    
        
    C_E = connProb*N_E;  %no. of excitatory inputs per neuron
    C_I = connProb*N_I;  %no. of inhibitory inputs per neuron
       
    opts = 'N_scaling'; %'C_scaling'    
    %*******************
    %Rescale weights
    %*******************    
    if(strcmp(opts,'N_scaling'))
        J_EE_rescaled     = J_EE * C_E/N;
        J_EI_rescaled     = J_EI * C_I/N;
        J_II_rescaled     = J_II * C_I/N;
        J_IE_rescaled     = J_IE * C_E/N;   
        
        NC                = N;
    elseif(strcmp(opts,'C_scaling'))
        J_EE_rescaled     = J_EE;
        J_EI_rescaled     = J_EI * sqrt(C_I/C_E);
        J_II_rescaled     = J_II * sqrt(C_I/C_E);
        J_IE_rescaled     = J_IE;
        NC                = C_E;
    end
       
    
    J = [J_EE_rescaled,-J_EI_rescaled;J_IE_rescaled,-J_II_rescaled];
    
    %*******************************************************************
    % Compute one data point
    %*******************************************************************   
    z             = fsolve(@BalancedState,[1,1]);
    nu_E_balanced = z(1);
    nu_I_balanced = z(2);
    
    [sigma_E_balanced,sigma_I_balanced] = GetSigma(nu_E_balanced,nu_I_balanced);
    mu_E_balanced    = GetInvPhi(nu_E_balanced,sigma_E_balanced,VR,tau_m);        
	mu_I_balanced    = GetInvPhi(nu_I_balanced,sigma_I_balanced,VR,tau_m);        
    
    x = [nu_E_balanced,nu_I_balanced,sigma_E_balanced,sigma_I_balanced];
    z = lsqnonlin(@BalancedStateCondition_Finite_C,x,[0,0,0,0],[1000,1000,1000,1000])    
    BalancedStateCondition_Finite_C(z)
    
    nu_E_finite     = z(1);
    nu_I_finite     = z(2);
    sigma_E_finite  = z(3);
    sigma_I_finite  = z(4);
    
    mu_E_finite    = GetInvPhi(nu_E_finite,sigma_E_finite,VR,tau_m);      %Eq. (14-E) Suppl. 
	mu_I_finite    = GetInvPhi(nu_I_finite,sigma_I_finite,VR,tau_m);      %Eq. (14-I) Suppl. 

    %******************************************************
    % Output
    %******************************************************    
    disp(['************************************************']);
	disp(['Condition 1      : mu_I_ext/mu_E_ext < J_II/J_EI < J_IE/J_EE']);
    disp(['for this network : ',num2str(mu_I_ext/mu_E_ext),' < ',num2str(J_II_rescaled/J_EI_rescaled),' < ',num2str(J_IE_rescaled/J_EE_rescaled)]);
    if ((mu_I_ext/mu_E_ext)>(J_II_rescaled/J_EI_rescaled)||(mu_I_ext/mu_E_ext)>(J_IE_rescaled/J_EE_rescaled)||(J_II_rescaled/J_EI_rescaled)>(J_IE_rescaled/J_EE_rescaled))
        disp(['Warning: conditions not met!']);
    end
    disp(['Condition 2     : J_IE > J_EE']);
    disp(['for this network : ',num2str(J_IE_rescaled),' > ',num2str(J_EE_rescaled)]);
    if (J_EI_rescaled<J_EE_rescaled)    
          disp(['Warning: J_EI < J_EE']);
    end
    disp(['det(J)  = ',num2str(det(J))]);
	disp(['************************************************']);

    disp(['Value   = finite C -- balanced state']);
    disp(['nu_E    = ',num2str(nu_E_finite),'   -- ',num2str(nu_E_balanced)]);
    disp(['nu_I    = ',num2str(nu_I_finite),'   -- ',num2str(nu_I_balanced)]);
    disp(['sigma_E = ',num2str(sigma_E_finite),'   -- ',num2str(sigma_E_balanced)]);
    disp(['sigma_I = ',num2str(sigma_I_finite),'   -- ',num2str(sigma_I_balanced)]);
    
    disp(['mu_E    = ',num2str(mu_E_finite),'   -- ',num2str(mu_E_balanced)]);
    disp(['mu_I    = ',num2str(mu_I_finite),'   -- ',num2str(mu_I_balanced)]);
    
                
    %**********************************************
    % Balanced state conditions (C_ext -> infinity)
    %**********************************************
    function z = BalancedState(x)
        nuE_ = x(1);
        nuI_ = x(2);
        
        z(1) = mu_E_ext + J_EE_rescaled*nuE_ - J_EI_rescaled*nuI_;
        z(2) = mu_I_ext + J_IE_rescaled*nuE_ - J_II_rescaled*nuI_;
        
    end    
    
    %**********************************************
    % Finite Network condition
    %**********************************************    
    function z = BalancedStateCondition_Finite_C(x)                
         nu_E    = x(1);
         nu_I    = x(2);
         sigma_E = x(3);
         sigma_I = x(4);

         mu_E    = GetInvPhi(nu_E,sigma_E,VR,tau_m,mu_E_balanced);      %Eq. (14-E) Suppl. 
         mu_I    = GetInvPhi(nu_I,sigma_I,VR,tau_m,mu_I_balanced);      %Eq. (14-I) Suppl. 

         z(1)    = mu_E_ext + J_EE_rescaled*nu_E - J_EI_rescaled*nu_I - mu_E/sqrt(NC);
         z(2)    = mu_I_ext + J_IE_rescaled*nu_E - J_II_rescaled*nu_I - mu_I/sqrt(NC);  %Eq. (25) Suppl.         
         [sE,sI] = GetSigma(nu_E,nu_I);
         z(3)    = sigma_E - sE;       %Eq. (11-E) Suppl.        
         z(4)    = sigma_I - sI;       %Eq. (11-I) Suppl.                         
    end	        

    function [sE,sI] = GetSigma(nu_E,nu_I)
        if(strcmp(opts,'N_scaling'))
            sE = sqrt(N/C_E*J_EE_rescaled^2*nu_E + N/C_I*J_EI_rescaled^2*nu_I + mu_E_ext_sigma^2);
            sI = sqrt(N/C_E*J_IE_rescaled^2*nu_E + N/C_I*J_II_rescaled^2*nu_I + mu_I_ext_sigma^2);
        elseif(strcmp(opts,'C_scaling'))
            sE = sqrt(J_EE_rescaled^2*nu_E + C_E/C_I*J_EI_rescaled^2*nu_I + mu_E_ext_sigma^2);
            sI = sqrt(J_IE_rescaled^2*nu_E + C_E/C_I*J_II_rescaled^2*nu_I + mu_I_ext_sigma^2);
        end         
    end
    
end