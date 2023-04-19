load('XOR_Data.mat')


for E=0:10
for I=0:10
t=50*E+5*I;
nuE(E+1,I+1)=XORData(1+10*t,6);
end
end
nuE(1,1)=XORData(2,6);
nuE(:,1)=nuE(1,:);
figure
imagesc(0:10,0:10,nuE)
set(gca,'YDir','normal')
set(gca,'FontSize',16)
xlabel('X_1 Firing rate (Hz)','FontSize',16)
ylabel('X_2 Firing rate (Hz)','FontSize',16)
colormap('hot')
title('Excitatory Firing rate')
c=colorbar('eastoutside');
title(c,'Hz');
% 
% print -depsc -tiff -r300 -painters Fig7B.eps