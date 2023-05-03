function cs=PowerLawFit

    close all;
    load F.mat F;
    tic
    Tau=(7:0.2:23)*1e-3;%Range of tau and sigma for which the curves will be fitted
    Sigma=1:0.05:5;
    mu=-50:0.5:150;%Range of mu (external input) over which each firing rate curve is calculated
    Vr=0.0;%Reset potential
    i2=41;%i2,j2 are the values of sigma and tau which will be fitted first. The parameters obtained by each fit can be used as initial values to fit curves with similar values of sigma and tau.
    j2=66;
    ig0    = [2.2e-06 4.77 3.82];%initial fitting parameters
    cs    = NaN(length(Sigma),length(Tau),length(ig0));%initialisation of cs, the matrix which will contain the fitted parameters
    for j=[j2:length(Tau) j2-1:-1:1]%scan through values of tau, starting from the middle of the parameter domain
            tau=Tau(j);
        for i = [i2:length(Sigma) i2-1:-1:1]
            sigma=Sigma(i);
            y2=(1-mu*tau)/(sigma*sqrt(tau));%y1 and y2 are the boudaries over which the integral must be calculated to get the firing rate
            y1=(Vr-mu*tau)/(sigma*sqrt(tau));
            I=getI(y1,y2,F);%The integral is calculated using the matrix F which stores the value of the integral between -12.75 and 10 (see function below);
            nu=1./(sqrt(pi)*tau*I);%firing rate
          	ig=get_ig(cs,ig0,i,j,i2,j2);%The initial estimation of the fitted parameters, based on previous sets of parameters stored in cs (see function below)  
            cs(i,j,:) = Analyse_opp(nu,mu,ig);%fitting (see function below)
        end
    end
    toc
    figure
    imagesc(Tau,Sigma,cs(:,:,1)')
    title('a')
    figure
    imagesc(Tau,Sigma,cs(:,:,2)')
    title('b')
    figure
    imagesc(Tau,Sigma,cs(:,:,3)')
    title('n')

    function I=getI(y1,y2,F)
        %if y1 and/or y2 is out of domain of z stored in F, the integral must be approximated 
        b1=(y2<=F(1,1));
        b2=(y1<F(1,1) & y2>F(1,1) & y2<=F(end,1));
        b3=(y1<=F(end,1) & y2>F(end,1) & y1>=F(1,1));
        b4=(y1>F(end,1) & y2>F(end,1));
        b5=(y1<F(1,1) & y2>F(end,1));
        b0=~b1 & ~b2 & ~b3 & ~b4 & ~b5;%b0 is the set of points where y1 and y2 can both be read from F
        I=zeros(1,numel(y1));
        z2=round(1+1e4*(y2-F(1,1)));%z1 and z2 are the indices of y1 and y2 in F
        z1=round(1+1e4*(y1-F(1,1)));
        I(b1)=NaN;
        I(b2)=NaN;
        I(b3)=sqrt(pi)*erfi(y2(b3))-(F(z1(b3),2))';
        I(b4)=sqrt(pi)*(erfi(y2(b4))-erfi(z1(b4)));
        I(b5)=sqrt(pi)*erfi(y2(b5));
        I(b0)=F(z2(b0),2)-F(z1(b0),2);
    end

    function [cs] = Analyse_opp(nu,mu,ig)
        mark = isfinite(nu) & (nu<10);%the fit is applied on a domain where the firing is below 10Hz
        fo = fitoptions('Method','NonlinearLeastSquares',...
                        'Lower',[0.,-2*abs(min(mu(mark))), 1.],...
                        'Upper',[1.,max(mu(mark)),10.],...
                        'StartPoint',[ig(1) ig(2) ig(3)]);
        ft = fittype('a*max(0,x-b)^n','options',fo);%<<<<<    FITTING FORMULA (parameters : a,b,n)
        if length(mark(mark))>20
            [coeffs,gof2] = fit(mu(mark)',nu(mark)',ft);
            cs = [coeffs.a,coeffs.b,coeffs.n];
        else
            cs=[NaN,NaN,NaN,NaN,NaN];
        end
    end

    function ig=get_ig(cs,ig0,i,j,i2,j2)
        %the initial value of the fitting parameters are based on the parameters obtained using the previous value of sigma or tau
        %if neither exist, we use the default starting parameters ig0
        if (i>i2)
            ig1=cs(i-1,j,:);
        elseif (i<i2)
            ig1=cs(i+1,j,:);
        else
            ig1=NaN(1,length(ig0));
        end
        if (j>j2)
            ig2=cs(i,j-1,:);
        elseif (j<j2)
            ig2=cs(i,j+1,:);
        else
            ig2=NaN(1,length(ig0));
        end
        if (any(isnan(ig1)))
            if (any(isnan(ig2)))
                ig=ig0;
            else
                ig=ig2;
            end
        else
            if (any(isnan(ig2)))
                ig=ig1;
            else
                ig=0.5*(ig1+ig2);
            end
        end
    end
end