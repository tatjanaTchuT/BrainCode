fid=fopen('SolExpval.txt','w');
load('F.mat')
NU = ContinuationExpVal([5 -10 7 -11],[0 0],1,-7.5,[0.02 0.01],0.01,1e4);
fprintf(fid,'%g\t',NU(1,1:end-1));
fprintf(fid,'\n');
fprintf(fid,'%g\t',NU(2,1:end-1));
fprintf(fid,'\n');
fprintf(fid,'%g\t',NU(3,1:end-1));
fprintf(fid,'\n');
fclose(fid);
%%
fid=fopen('SolNorec.txt','w');
load('F.mat')
NU = ContinuationNorec([5 -10 7 -11],[0 0],1,-7.5,[0.02 0.01],0.01,1e4);
fprintf(fid,'%g\t',NU(1,1:end-1));
fprintf(fid,'\n');
fprintf(fid,'%g\t',NU(2,1:end-1));
fprintf(fid,'\n');
fprintf(fid,'%g\t',NU(3,1:end-1));
fprintf(fid,'\n');
fclose(fid);
%%
fid=fopen('soltau1.txt','w');
load('F.mat')
NU = Continuation_tau1([5 -10 7 -11],[0 0],1,-7.5,[0.02 0.01],0.01,1e4);
fprintf(fid,'%g\t',NU(1,1:end-1));
fprintf(fid,'\n');
fprintf(fid,'%g\t',NU(2,1:end-1));
fprintf(fid,'\n');
fprintf(fid,'%g\t',NU(3,1:end-1));
fprintf(fid,'\n');
fclose(fid);
%%
fid=fopen('SolFS.txt','w');
load('F.mat')
NU = Continuation_FiniteSize([5 -10 7 -11],[0 0],1,-7.5,[0.02 0.01],0.01,1e4);
fprintf(fid,'%g\t',NU(1,1:end-1));
fprintf(fid,'\n');
fprintf(fid,'%g\t',NU(2,1:end-1));
fprintf(fid,'\n');
fprintf(fid,'%g\t',NU(3,1:end-1));
fprintf(fid,'\n');
fclose(fid);