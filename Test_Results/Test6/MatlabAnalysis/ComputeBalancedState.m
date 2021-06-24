function [nu_E_balanced,nu_I_balanced,...
          mu_E_balanced,mu_I_balanced,...
          sigma_E_balanced,sigma_I_balanced] = ComputeBalancedState(params)

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
    elseif(strcmp(opts,'C_scaling'))
        J_EE_rescaled     = J_EE;
        J_EI_rescaled     = J_EI * sqrt(C_I/C_E);
        J_II_rescaled     = J_II * sqrt(C_I/C_E);
        J_IE_rescaled     = J_IE;        
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