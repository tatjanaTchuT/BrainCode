fid=fopen('BistabilityPhi.txt','w');

% i=11
% L=[2*(i-1)/10 2*(i-1)/10+0.1]

% for N=4e3*10.^L
% N

%for N=4e3*10.^[0:0.1:2.1]
for N=4e3*10.^[1.4:-0.1:1.4]
% fprintf(fid,'%g\t',N);
% fprintf(fid,'\n');
%NU = ContinuationPhi([5 -10 7 -11],[0 0],1,-15,[0.02 0.01],0.0025,5e5,N);
NU = ContinuationPhi([5 -10 7 -11],[0 0],1,-15,[0.02 0.01],0.01,5e5,N);
fprintf(fid,'%g\t',NU(1,1:end-1));
fprintf(fid,'\n');
fprintf(fid,'%g\t',NU(2,1:end-1));
fprintf(fid,'\n');
fprintf(fid,'%g\t',NU(3,1:end-1));
fprintf(fid,'\n');
end

fclose(fid);