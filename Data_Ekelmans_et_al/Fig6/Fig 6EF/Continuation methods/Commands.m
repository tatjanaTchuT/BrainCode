fid=fopen('Outputfwd.txt','w');
for N=4e3*10.^[0.0:0.1:2.1]
    NU = Continuation_FiniteSize([1.25 -1 1 -1.25],[58*(N/(4e3))^(-1.961/(log(10))) 0],3,-5,[0.02 0.01],0.025,1e5,N,1);
    fprintf(fid,'%g\t',NU(1,1:end-1));
    fprintf(fid,'\n');
    fprintf(fid,'%g\t',NU(2,1:end-1));
    fprintf(fid,'\n');
    fprintf(fid,'%g\t',NU(3,1:end-1));
    fprintf(fid,'\n');
end
fclose(fid);
%% 

fid=fopen('Outputrev1921.txt','w');
for N=4e3*10.^[0.0:0.1:2.1]
    NU = Continuation_FiniteSize([1.25 -1 1 -1.25],[0 50],3,75,[0.02 0.01],0.025,1e5,N,-1);
    fprintf(fid,'%g\t',NU(1,1:end-1));
    fprintf(fid,'\n');
    fprintf(fid,'%g\t',NU(2,1:end-1));
    fprintf(fid,'\n');
    fprintf(fid,'%g\t',NU(3,1:end-1));
    fprintf(fid,'\n');
end
fclose(fid);