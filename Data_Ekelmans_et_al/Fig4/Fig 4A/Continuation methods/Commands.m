a=[1.079e-04,2.210e-06];b=[-11.14,4.77];n=[3.081,3.815];
load('F.mat')
Bistability_SSN=Continuation_SSN([5 -10 7 -11],[0 0],1,0,a,b,n,5e-2,1e5);
NU = Continuation_FiniteSize([5 -10 7 -11],[0 0],1,-5,[0.02 0.01],0.01,1e4);