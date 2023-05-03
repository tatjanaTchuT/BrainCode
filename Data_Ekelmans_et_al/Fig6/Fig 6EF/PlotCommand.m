load('DataPhi.mat')
load('DataSim.mat')
a=[1.079e-04,2.210e-06];b=[-11.14,4.77];n=[3.081,3.815];
col_B=[0 0.447 0.741];
col_R=[0.85 0.325 0.098];
%% 

figure
plot(NegSupersat4e3Data(5:end,1)/2,NegSupersat4e3Data(5:end,4),'Color',[0 0.4470 0.7410]+(0)*([0.8500, 0.3250, 0.0980]-[0 0.4470 0.7410])/21,'LineWidth',2.5)
hold on
%plot(NegSupersat1e4Data(5:end,1)*sqrt(2.5)/2,NegSupersat1e4Data(5:end,4),'Color',[0 0.4470 0.7410]+(4)*([0.8500, 0.3250, 0.0980]-[0 0.4470 0.7410])/21,'LineWidth',2)
plot(NegSupersat2e4Data(5:end,1)*sqrt(5)/2,NegSupersat2e4Data(5:end,4),'Color',[0 0.4470 0.7410]+(7)*([0.8500, 0.3250, 0.0980]-[0 0.4470 0.7410])/21,'LineWidth',2.5)
plot(NegSupersat4e4Data(5:end,1)*0.03,NegSupersat4e4Data(5:end,4),'Color',[0 0.4470 0.7410]+(10)*([0.8500, 0.3250, 0.0980]-[0 0.4470 0.7410])/21,'LineWidth',2.5)
plot(max(70-2/5*NegSupersat4e4revData(5:end,1),33-3/100*NegSupersat4e4revData(5:end,1)),NegSupersat4e4revData(5:end,4),'Color',[0 0.4470 0.7410]+(10)*([0.8500, 0.3250, 0.0980]-[0 0.4470 0.7410])/21,'LineWidth',2.5)

axis([0 50 0 15])

for i=[0 7 10]%[0 4 7 10]
% plot(OutputFw(i*3+3,1:10:end),OutputFw(i*3+1,1:10:end),'k','LineWidth',1)
% plot(OutputRv(i*3+3,1:10:end),OutputRv(i*3+1,1:10:end),'k','LineWidth',1)
PlotSSN_noN(a,b,n,[1.25 -1;1 -1.25]*sqrt(10^(0.1*i)),3,[0 50 0 20],[0 0 0],2)
end
xlabel('External input \mu_{ext}','FontSize',16)
ylabel('E Firing rate \nu_E (Hz)','FontSize',16)
set(gca,'FontSize',16)
box on
axis([0 50 0 6])

axes('YAxisLocation','left','Position',[.63 .65 .25 .25])
hold on
for i=0:21
% plot(OutputFw(i*3+3,1:10:end),OutputFw(i*3+1,1:10:end),'Color',[0 0.4470 0.7410]+(i)*([0.8500, 0.3250, 0.0980]-[0 0.4470 0.7410])/21,'LineWidth',1.5)
% plot(OutputRv(i*3+3,1:10:end),OutputRv(i*3+1,1:10:end),'Color',[0 0.4470 0.7410]+(i)*([0.8500, 0.3250, 0.0980]-[0 0.4470 0.7410])/21,'LineWidth',1.5)
PlotSSN_noN(a,b,n,[1.25 -1;1 -1.25]*sqrt(10^(0.1*i)),3,[0 50 0 20],col_B+(col_R-col_B)*i/21,1.5)
end
axis([0 50 0 15])
xticks([0 50])
yticks([0 15])
box on


N=4e3*10.^[0:0.1:2.1];
figure
hold on
for i=0:0.1:2.1
Plot_SSN(a,b,n,[1.25 -1;1 -1.25],3,[-0.01 0.5 0 12],4e3*10^[i],col_B+(col_R-col_B)*i/2.1,2)
end
% for i=0:21
% plot(OutputFw(i*3+3,1:10:end)/sqrt(N(1+i)),OutputFw(i*3+1,1:10:end),'Color',[0 0.4470 0.7410]+(i)*([0.8500, 0.3250, 0.0980]-[0 0.4470 0.7410])/21,'LineWidth',2.5)
% plot(OutputRv(i*3+3,1:10:end)/sqrt(N(1+i)),OutputRv(i*3+1,1:10:end),'Color',[0 0.4470 0.7410]+(i)*([0.8500, 0.3250, 0.0980]-[0 0.4470 0.7410])/21,'LineWidth',2.5)
% end
xlabel('External input \mu_{ext} /?N','FontSize',16)
ylabel('E Firing rate \nu_E (Hz)','FontSize',16)
set(gca,'FontSize',16)
box on
axis([-0.01 0.5 0 12])
l=plot([0 1],[0 1]*(3*1-1.25)/(1.25^2-1^2)*sqrt(4e3),'k:','LineWidth',3);

h = legend(l,'Balanced state');
pos = get(h,'Position');
set(h,'Position',[0.445 0.847 0.3 0.07]);
legend boxoff

axes('YAxisLocation','left','Position',[.619 .554 .25 .25])
hold on
for i=0:21
plot(OutputFw(i*3+3,1:10:end)/sqrt(N(1+i)),OutputFw(i*3+1,1:10:end),'Color',[0 0.4470 0.7410]+(i)*([0.8500, 0.3250, 0.0980]-[0 0.4470 0.7410])/21,'LineWidth',1.5)
plot(OutputRv(i*3+3,1:10:end)/sqrt(N(1+i)),OutputRv(i*3+1,1:10:end),'Color',[0 0.4470 0.7410]+(i)*([0.8500, 0.3250, 0.0980]-[0 0.4470 0.7410])/21,'LineWidth',1.5)
end
% for i=0:0.1:2.1
% Plot_SSN(a,b,n,[1.25 -1;1 -1.25],3,[-0.01 0.5 0 12],4e3*10^[i],col_B+(col_R-col_B)*i/2.1)
% end
axis([-0.01 0.5 0 12])
xticks([])
yticks([])
box on