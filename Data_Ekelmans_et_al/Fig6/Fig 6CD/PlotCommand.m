%close all
load('BistabilityPhi_N.mat')
load('BistabilitySim_N.mat')
a=[1.079e-04,2.210e-06];b=[-11.14,4.77];n=[3.081,3.815];
col_B=[0 0.447 0.741];
col_R=[0.85 0.325 0.098];
%%
figure
hold on
X=[0:0.005*4:10 10:-0.005*4:0];
Y4e3=movmean(Bistability4e3Data(3:end,4),10);
Y4e4=movmean(Bistability4e4Data(1e4+3:end,4),10);

plot(X(3:end),Y4e3(1:50*4:end),'Color',[0    0.4470    0.7410],'LineWidth',2.5)
plot(X(3:end),Y4e4(1:50:end),'Color',([0    0.4470    0.7410]*1.1/2.1+1/2.1*[0.8500    0.3250    0.0980]),'LineWidth',2.5)

% plot(Bistability4e3Data(3:10:end,1)*0.01-0.02*max(0,Bistability4e3Data(3:10:end,1)-1000),movmean(Bistability4e3Data(3:10:end,4),10),'Color',[0    0.4470    0.7410],'LineWidth',2)
% plot((Bistability4e4Data(3:10:end,1)-400)*0.01-0.02*max(0,(Bistability4e4Data(3:10:end,1)-400)-1000),movmean(Bistability4e4Data(3:10:end,4),3),'Color',([0    0.4470    0.7410]*1.1/2.1+1/2.1*[0.8500    0.3250    0.0980]),'LineWidth',2)

%%
for i=[0 10]%[0 4 7 10] 
%plot(BistabilityPhi_N(i*3+3,1:end),BistabilityPhi_N(i*3+1,1:end),'k','LineWidth',1)
PlotSSN_noN(a,b,n,[5 -10;7 -11]*sqrt(10^(0.1*i)),1,[0 50 0 10],[0 0 0],2)
end
xlabel('External input \mu_{ext}','FontSize',16)
ylabel('E Firing rate \nu_E (Hz)','FontSize',16)
set(gca,'FontSize',16)
axis([0 10 0 12])

box on
axes('YAxisLocation','left','Position',[.165 .67 .24 .24])
for i=0:21
%plot(BistabilityPhi_N(i*3+3,1:25:end),BistabilityPhi_N(i*3+1,1:25:end),'Color',[0 0.4470 0.7410]+(i)*([0.8500, 0.3250, 0.0980]-[0 0.4470 0.7410])/21,'LineWidth',1.5)
PlotSSN_noN(a,b,n,[5 -10;7 -11]*sqrt(10^(0.1*i)),1,[-30 20 0 10],col_B+(col_R-col_B)*i/21,1.5)
hold on
end
axis([-20 10 0 7])
xticks([-20 0 10])
yticks([0 7])

N=4e3*10.^[0:0.1:2.1];
figure
hold on
% for i=0:21
% plot(BistabilityPhi_N(i*3+3,1:10:end)/sqrt(N(1+i)),BistabilityPhi_N(i*3+1,1:10:end),'Color',[0 0.4470 0.7410]+(i)*([0.8500, 0.3250, 0.0980]-[0 0.4470 0.7410])/21,'LineWidth',2.5)
% end
for i=0:0.1:2.1
Plot_SSN(a,b,n,[5 -10;7 -11],1,[-0.06 0.06 0 7],4e3*10^[i],col_B+(col_R-col_B)*i/2.1,2)
end
xlabel('External input \mu_{ext} /?N','FontSize',16)
ylabel('E Firing rate \nu_E (Hz)','FontSize',16)
set(gca,'FontSize',16)
axis([-0.06 0.06 0 7])
box on


s=BalancedState([5 -10; 7 -11]./[195 200;825 100]*sqrt(4e3),[3e3;1e3],[0.065 0.2;0.275 0.1], [1;1]);
l=plot([0 1],[0 1]*s(1),'k:','LineWidth',3);

% legend(l,'Balanced State','Location','north')
% legend boxoff
h = legend(l,'Balanced state');
pos = get(h,'Position');
set(h,'Position',[0.445 0.847 0.3 0.07]);
legend boxoff
%text(-0.05,3.5, '\Phi_{sc}','FontSize',16)

axes('YAxisLocation','left','Position',[.13 .687 .24 .24])
hold on
for i=0:21
%plot(BistabilityPhi_N(i*3+3,1:10:end)/sqrt(N(1+i)),BistabilityPhi_N(i*3+1,1:10:end),'Color',[0 0.4470 0.7410]+(i)*([0.8500, 0.3250, 0.0980]-[0 0.4470 0.7410])/21,'LineWidth',1.5)
plot(BistabilityPhi(i*3+3,1:10:end)/sqrt(N(1+i)),BistabilityPhi(i*3+1,1:10:end),'Color',[0 0.4470 0.7410]+(i)*([0.8500, 0.3250, 0.0980]-[0 0.4470 0.7410])/21,'LineWidth',1.5)
end
% for i=0:0.1:2.1
% Plot_SSN(a,b,n,[5 -10;7 -11],1,[-0.06 0.06 0 7],4e3*10^[i],col_B+(col_R-col_B)*i/2.1)
% end
axis([-0.06 0.06 0 7])
xticks([])
yticks([])
box on
%text(-0.05,6, 'SSN','FontSize',16)