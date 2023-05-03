figure
load('F.mat')%used to calculate Phi using getnu
load('cs.mat')%cs contains the a b and n parameters of a for various combinations of sigma and tau. See Fig1C
load('SimulatedFI.mat')
%%
hold on

col_B=[0 0.447 0.7];
col_R=[0.85 0.325 0.098];
col_G=[0 0.6863 0.1961];
X=0:0.1:50;

col_B=[0 0.447 0.7];
col_R=[0.85 0.325 0.098];
col_G=[0 0.6863 0.1961];
l4=plot(X,getnu(1,0.02,X,F),'k','LineWidth',1.5);
plot(X,getnu(3,0.015,X,F),'k','LineWidth',1.5)
plot(X,getnu(5,0.01,X,F),'k','LineWidth',1.5)

l5=plot(X,cs(1,66,1)*max(0,X-cs(1,66,2)).^cs(1,66,3),'k--','LineWidth',2.5);
plot(X,cs(41,41,1)*max(0,X-cs(41,41,2)).^cs(41,41,3),'k--','LineWidth',2.5)
plot(X,cs(81,16,1)*max(0,X-cs(81,16,2)).^cs(81,16,3),'k--','LineWidth',2.5)
axis([0 50 0 20])
l1=plot([0:5:50],SimulatedFIData(11:10:end,5),'^','Color',col_B,'MarkerFaceColor',col_B);
l2=plot([0:5:50],SimulatedFIData(11:10:end,6),'s','Color',col_R,'MarkerFaceColor',col_R);
l3=plot([0:5:50],SimulatedFIData(11:10:end,7),'d','Color',col_G,'MarkerFaceColor',col_G);

plot([cs(1,66,2) cs(1,66,2)],[0 1.5],'Color',col_B,'LineWidth',2)
plot([cs(41,41,2) cs(41,41,2)],[0 1.5],'Color',col_R,'LineWidth',2)
plot([cs(81,16,2) cs(81,16,2)],[0 1.5],'Color',col_G,'LineWidth',2)

legend([l1 l2 l3 l4 l5],{'Simulation (\sigma=1 \tau=20)','Simulation (\sigma=3 \tau=15)','Simulation (\sigma=5 \tau=10)','\Phi(\mu,\sigma,\tau)','Fitted power law'},'FontSize',12,'Location','northwest')
set(gca,'FontSize',16)
legend boxoff

xticks([0 10 20 30 40 50])
yticks([0 5 10 15 20])
box on

xlabel('Input \mu (mV/s)','FontSize',18)
ylabel('Firing rate \nu (Hz)','FontSize',18)