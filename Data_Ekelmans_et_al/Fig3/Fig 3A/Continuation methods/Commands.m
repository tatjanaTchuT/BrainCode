a=[1.0792e-04 2.2099e-06];b=[-11.1425 4.7670];n=[3.0814 3.8150];%power law fit of fi curve, tau_m=20ms
Allen_SSN=Continuation_SSN([0.672 -13.173 23.675 -11.846],[0 0],1,0,a,b,n,5e-2,1e5);
Allen66_SSN=Continuation_SSN([0.672 -13.173 23.675 -11.846],[0 0],1.5,0,a,b,n,5e-2,1e5);