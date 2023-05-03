a=[1.0792e-04 2.2099e-06];b=[-11.1425 4.7670];n=[3.0814 3.8150];
AllenISN1_SSN=Continuation_SSN([4.75 -13.173 23.675 -11.846],[0 0],0.5,[0 0],a,b,n,5e-2,1e5);
AllenISN2_SSN=Continuation_SSN([-11.846 23.675 -13.173 4.75],[7.6 4],0,[50 100],fliplr(a),fliplr(b),fliplr(n),5e-2,1e5);
