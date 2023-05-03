figure
hold on
load('Bistability_SSN.mat')
load('Bistability_Sim.mat')


%%
plot1=plot(Bistability_Sim(5:end,1)*0.02-0.04*max(Bistability_Sim(5:end,1)-500,0),movmean(Bistability_Sim(5:end,4),25),'Color',[0.0000    0.4470    0.7410],'Linewidth',1.5);

plot2=plot(Bistability_SSN(3,:),Bistability_SSN(1,:),'k--','LineWidth',2.5);
xlabel('External input \mu_{ext}','FontSize',16)
ylabel('E Firing rate \nu_E (Hz)','FontSize',16)
set(gca,'FontSize',16)
axis([0 10 0 10])
box on

legend([plot1 plot2],{'Network simulation','SSN'},'FontSize',15,'Location','northwest')
legend boxoff
axes('YAxisLocation','right','Position',[.55 .25 .3 .3])
hold on

plot(Bistability500Data(5:end,1)*0.04-0.08*max(Bistability500Data(5:end,1)-250,0),movmean(Bistability500Data(5:end,4),10),'Color',[0.0 0.6863 0.1961],'Linewidth',1.5)
plot(Bistability_Sim(5:end,1)*0.02-0.04*max(Bistability_Sim(5:end,1)-500,0),movmean(Bistability_Sim(5:end,4),20),'Color',[0.8500 0.3250 0.0980],'Linewidth',1.5)
plot(Bistability2000Data(5:end,1)*0.01-0.02*max(Bistability2000Data(5:end,1)-1000,0),movmean(Bistability2000Data(5:end,4),30),'Color',[0 0.4470 0.7410],'Linewidth',1.5)

axis([2 5 0 10])
xticks([2 5])
set(gca,'FontSize',10)
box on