a=[7.8904e-05 2.2099e-06];b=[-9.4929 4.7670];n=[3.1357 3.8150];%power law fit of fi curve, tau_m=18ms
Bista_SSN_18ms=Continuation_SSN([5 -10 7 -11],[0 0],1,0,a,b,n,5e-2,1e5);

a=[1.0792e-04 2.2099e-06];b=[-11.1425 4.7670];n=[3.0814 3.8150];%power law fit of fi curve, tau_m=20ms
Bista_SSN_20ms=Continuation_SSN([5 -10 7 -11],[0 0],1,0,a,b,n,5e-2,1e5);

a=[1.5997e-04 2.2099e-06];b=[-12.1213 4.7670];n=[3.0053 3.8150];%power law fit of fi curve, tau_m=22ms
Bista_SSN_22ms=Continuation_SSN([5 -10 7 -11],[0 0],1,0,a,b,n,5e-2,1e5);