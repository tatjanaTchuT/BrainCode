load('Brain_Sim.mat')
load('Brain_Target.mat')

%%
T=linspace(0,BrainSim(end,1),length(BrainTarget(:,1)));
figure
subplot(2,2,1)
plot(BrainSim(5:end,1),movmean(BrainSim(5:end,4),5),'LineWidth',2.5)
hold on
plot(BrainSim(5:end,1),movmean(BrainSim(5:end,5),5),'LineWidth',2.5)
plot(T,BrainTarget(:,1),'k','LineWidth',2)
hold on
plot(T,BrainTarget(:,2),'k','LineWidth',2)
ylabel('Firing Rate (Hz)')
legend('\nu_E','\nu_I','Target','Location','northeastoutside')
axis([0 5 0 10])
set(gca,'FontSize',16)


%axes('Position',[0.13 0.769 0.773 0.152])
subplot(2,2,3)
plot(BrainSim(5:end,1),movmean(BrainSim(5:end,6),5),'LineWidth',2.5)
hold on
plot(BrainSim(5:end,1),movmean(BrainSim(5:end,7),5),'LineWidth',2.5)
ylabel('External input (mV/s)')
plot([0 5],[0 0],'k','LineWidth',0.25)
axis([0 5 -25 125])
xlabel('time (s)')
set(gca,'FontSize',16)
% xlabel('time (s)')
legend('\mu_{ext E}','\mu_{ext I}','Location','northeastoutside')
%%
figure
hold on
plot(movmean(BrainSim(5:end,4),10),movmean(BrainSim(5:end,5),10),'LineWidth',2.5)%,'Color',[0.4940, 0.1840, 0.5560]
plot(BrainTarget(:,1),BrainTarget(:,2),'k','LineWidth',2)
axis equal
axis([0 10 0 10])

xlabel('Excitatory Firing Rate (Hz)')
ylabel('Inhibitory Firing Rate (Hz)')
xticks([0 2.5 5 7.5 10])
yticks([0 2.5 5 7.5 10])
box on
legend('Simulation','Target','Location','southeast')
set(gca,'FontSize',16)
% legend boxoff