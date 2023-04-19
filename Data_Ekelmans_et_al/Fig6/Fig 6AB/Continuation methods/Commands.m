fid=fopen('Output.txt','w');
for N=4e3*10.^[0.0:0.1:2.1];
NU = Continuation_FiniteSize([2 -12 6 -1],[0 0],1,0,[0.02 0.01],0.01,1e5,N);
fprintf(fid,'%g\t',NU(1,1:end-1));
fprintf(fid,'\n');
fprintf(fid,'%g\t',NU(2,1:end-1));
fprintf(fid,'\n');
fprintf(fid,'%g\t',NU(3,1:end-1));
fprintf(fid,'\n');
end
fclose(fid);