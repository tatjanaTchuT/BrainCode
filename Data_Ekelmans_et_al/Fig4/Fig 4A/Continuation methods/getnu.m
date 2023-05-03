function nu=getnu(sigma,tau,mu,F)

%     close all;
    %load F.mat F;
    Vr=0.0;%Reset potential

    y2=(1-mu*tau)/(sigma*sqrt(tau));%y1 and y2 are the boudaries over which the integral must be calculated to get the firing rate
    y1=(Vr-mu*tau)/(sigma*sqrt(tau));
    I=getI(y1,y2,F);%The integral is calculated using the matrix F which stores the value of the integral between -12.75 and 10 (see function below);
    nu=1./(sqrt(pi)*tau*I);%firing rate

    function I=getI(y1,y2,F)
        %if y1 and/or y2 is out of domain of z stored in F, the integral must be approximated 
        b1=(y2<=F(1,1));
        b2=(y1<F(1,1) & y2>F(1,1) & y2<=F(end-1,1));
        b3=(y1<=F(end-1,1) & y2>F(end-1,1) & y1>=F(1,1));
        b4=(y1>F(end-1,1) & y2>F(end-1,1));
        b5=(y1<F(1,1) & y2>F(end-1,1));
        b0=~b1 & ~b2 & ~b3 & ~b4 & ~b5;%b0 is the set of points where y1 and y2 can both be read from F
        I=zeros(1,numel(y1));
        z2=floor(1+1e4*(y2-F(1,1)));%z1 and z2 are the indices of y1 and y2 in F
        z1=floor(1+1e4*(y1-F(1,1)));
        rz2=1+1e4*(y2-F(1,1))-z2;%in order for nu(mu) to be continuous, the value of I will be interpolated between entries of the table
        rz1=1+1e4*(y1-F(1,1))-z1;
        I(b1)=NaN;
        I(b2)=NaN;
        I(b3)=sqrt(pi)*erfi(y2(b3))-(F(z1(b3),2).*(1-rz1(b3)')+F(z1(b3)+1,2).*rz1(b3)')';
        I(b4)=sqrt(pi)*(erfi(y2(b4))-erfi(z1(b4)));
        I(b5)=sqrt(pi)*erfi(y2(b5));
        I(b0)=F(z2(b0),2).*(1-rz2(b0)')+F(z2(b0)+1,2).*rz2(b0)'-(F(z1(b0),2).*(1-rz1(b0)')+F(z1(b0)+1,2).*rz1(b0)');
    end
end