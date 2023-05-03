% close all
figure
load('Supersaturation_Phi.mat')
load('Supersaturation_Sim.mat')
load('Supersaturation_SSN.mat')
l1=plot(Supersaturation_Sim(5:end,1),movmean(Supersaturation_Sim(5:end,4),10),'Color',[0.0000    0.4470    0.7410],'LineWidth',2);
hold on
l2=plot(Supersaturation_SSN(3,:),Supersaturation_SSN(1,:),'k--','LineWidth',2);
l3=plot(Supersaturation_Phi(3,:),Supersaturation_Phi(1,:),'k','LineWidth',2);
l4=plot(Supersaturation_Sim(5:end,1),movmean(Supersaturation_Sim(5:end,5),10),'Color',[0.8500    0.3250    0.0980],'LineWidth',2);
plot(Supersaturation_SSN(3,:),Supersaturation_SSN(2,:),'k--','LineWidth',2);
plot(Supersaturation_Phi(3,:),Supersaturation_Phi(2,:),'k','LineWidth',2);
axis([0 50 0 5])
xlabel('External input \mu_{ext}','FontSize',16)
ylabel('Firing rate \nu (Hz)','FontSize',16)
legend([l1 l4 l3 l2],{'Network simulation E','Network simulation I','\Phi_{sc}','SSN'},'FontSize',16,'Location','northwest')
set(gca,'FontSize',16)
legend boxoff