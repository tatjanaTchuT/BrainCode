a=[1.079e-04,2.210e-06];b=[-11.14,4.77];n=[3.081,3.815];
load('F.mat')
f=1
Supersat_SSN1=Continuation_SSN([2 -12*f 6/f -1],[0 0],1/f,0,a,b,n,5e-2,1e5);
Supersat_Phi1=Continuation_FiniteSize([2 -12*f 6/f -1],[0 0],1/f,0,[0.02 0.01],1e-2,1e4)

f=1.25
Supersat_SSN125=Continuation_SSN([2 -12*f 6/f -1],[0 0],1/f,0,a,b,n,5e-2,1e5);
Supersat_Phi125=Continuation_FiniteSize([2 -12*f 6/f -1],[0 0],1/f,0,[0.02 0.01],1e-2,1e4)

f=1.5
Supersat_SSN15=Continuation_SSN([2 -12*f 6/f -1],[0 0],1/f,0,a,b,n,5e-2,1e5);
Supersat_Phi15=Continuation_FiniteSize([2 -12*f 6/f -1],[0 0],1/f,0,[0.02 0.01],1e-2,1e4)