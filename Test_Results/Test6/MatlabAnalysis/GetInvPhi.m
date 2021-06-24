function mu = GetInvPhi(nu,sigma,VR,tau_m,mu_guess)
    %disp(num2str(nu));
    opts = optimset('display','off','tolfun',1e-8);    
    
    if(nargin < 5)
        mu_guess = 1/tau_m;
    end        
    
    for ii = 1:length(nu)
        nu_     = nu(ii);
        sigma_  = sigma(ii);
        if(ii>1)
            mu(ii)  = fsolve(@f,mu(ii-1),opts);
        else
            mu(ii)  = fsolve(@f,mu_guess,opts);
        end
    end
   
    function yy = f(mu)
        yy = Phi(mu,sigma_,VR,tau_m) - nu_;
    end
       
end