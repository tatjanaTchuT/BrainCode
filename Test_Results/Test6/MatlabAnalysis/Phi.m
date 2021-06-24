 function z = Phi(mu,sigma,VR,tau_m,tau_s) 
 
    if(length(sigma) == 1)
        sigma = ones(size(mu))*sigma;
    end
 
    if(nargin < 5)
        tau_s = 0;
    end
    alpha   = sqrt(2)*abs(zeta(0.5));
    d_tau_s = sqrt(tau_s/tau_m)*alpha/2;
    
    z = zeros(size(mu));
    for ii = 1:length(mu)
        y1    = (VR-tau_m*mu(ii))/sigma(ii)/sqrt(tau_m) + d_tau_s;
        y2    = (1-tau_m*mu(ii))/sigma(ii)/sqrt(tau_m)  + d_tau_s;
        if(Phi_Integrand(y2) > 1e10)
            z(ii) = 0;
        else
            z(ii) = (tau_m*sqrt(pi)*integral(@Phi_Integrand,y1,y2))^(-1);
        end            
    end        
    
    function y = Phi_Integrand(z)
        mark     = (z < -4);
        y        = zeros(size(z));
        y(~mark) = Phi_Integrand_Full(z(~mark));
        y(mark)  = Phi_Integrand_Expansion(z(mark));       
    end
    function y = Phi_Integrand_Full(z)
        y = exp(z.^2).*(1+erf(z));        
    end
    function y = Phi_Integrand_Expansion(z)
        if(z > 0)
            throw('error');
        end
        z  = -z;
        zt = 2*z.^2;
        y  = 1/sqrt(pi)./z.*(1-1./zt + 3./zt.^2 - 3*5./zt.^3);
    end    


end