function Vout=Reduce(Vin,Lout)
    Lin=length(Vin);
    Exc=mod(Lin,Lout);
    Vexc=[2:floor((Lin-2)/(Exc-1)):(Exc-1)*floor((Lin-2)/(Exc-1)) Lin-1];
    Vin(Vexc)=[];
    Vout=Vin(1:(Lin-Exc)/Lout:end);
end