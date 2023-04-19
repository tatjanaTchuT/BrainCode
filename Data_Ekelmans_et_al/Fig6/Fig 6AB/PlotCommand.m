%close all
figure

load('SupersatPhi_N.mat')
load('SupersatSim_N.mat')
a=[1.079e-04,2.210e-06];b=[-11.14,4.77];n=[3.081,3.815];
col_B=[0 0.447 0.741];
col_R=[0.85 0.325 0.098];

hold on

plot(Supersaturation4e3Data(:,1),movmean(Supersaturation4e3Data(:,4),10),'Color',[0    0.4470    0.7410],'LineWidth',2.5)
plot(Supersaturation1e4Data(:,1)*79.0569/50,movmean(Supersaturation1e4Data(:,4),10),'Color',([0    0.4470    0.7410]*1.7/2.1+0.4/2.1*[0.8500    0.3250    0.0980]),'LineWidth',2.5)
plot(Supersaturation2e4Data(:,1)*111.8/50,movmean(Supersaturation2e4Data(:,4),10),'Color',([0    0.4470    0.7410]*1.4/2.1+0.7/2.1*[0.8500    0.3250    0.0980]),'LineWidth',2.5)
plot(Supersaturation4e4Data(:,1)*158.11/50,movmean(Supersaturation4e4Data(:,4),10),'Color',([0    0.4470    0.7410]*1.1/2.1+1/2.1*[0.8500    0.3250    0.0980]),'LineWidth',2.5)
N=4e3*10.^[0:0.1:2.1];
for i=[0 4 7 10] %14 17]
%plot(SupersatPhi_N(i*3+3,1:16:end),SupersatPhi_N(i*3+1,1:16:end),'k','LineWidth',1)
PlotSSN_noN(a,b,n,[2 -12;6 -1]*sqrt(10^(0.1*i)),1,[0 50 0 10],[0 0 0],2)
end
xlabel('External input \mu_{ext}','FontSize',16)
ylabel('E Firing rate \nu_E (Hz)','FontSize',16)
set(gca,'FontSize',16)
axis([0 50 0 3])

box on
axes('YAxisLocation','left','Position',[.619 .631 .25 .25])
for i=0:21
%plot(SupersatPhi_N(i*3+3,1:16:end),SupersatPhi_N(i*3+1,1:16:end),'Color',[0 0.4470 0.7410]+(i)*([0.8500, 0.3250, 0.0980]-[0 0.4470 0.7410])/21,'LineWidth',1.5)
PlotSSN_noN(a,b,n,[2 -12;6 -1]*sqrt(10^(0.1*i)),1,[0 50 0 10],col_B+(col_R-col_B)*i/21,1.5)
hold on
end
axis([0 50 0 3])
xticks([0 50])
yticks([0 3])
%% -----------------------------------

figure
hold on
% for i=0:21
% plot(SupersatPhi_N(i*3+3,1:16:end)/sqrt(N(1+i)),SupersatPhi_N(i*3+1,1:16:end),'Color',[0 0.4470 0.7410]+(i)*([0.8500, 0.3250, 0.0980]-[0 0.4470 0.7410])/21,'LineWidth',2.5)
% end
for i=0:0.1:2.1
Plot_SSN(a,b,n,[2 -12;6 -1],1,[0 0.75 0 3],4e3*10^[i],col_B+(col_R-col_B)*i/2.1,2)
end
xlabel('External input \mu_{ext} /?N','FontSize',16)
ylabel('E Firing rate \nu_E (Hz)','FontSize',16)
set(gca,'FontSize',16)
axis([0 0.75 0 3])
xticks([0 0.25 0.5 0.75])
box on

s=BalancedState([2 -12;6 -1]./[195 200;825 100]*sqrt(4e3),[3e3;1e3],[0.065 0.2;0.275 0.1], [1;1]);
plot([0 1],[0 1]*s(1),'r:','LineWidth',3)


axes('YAxisLocation','left','Position',[.619 .631 .25 .25])
hold on
for i=0:21
plot(SupersatPhi_N(i*3+3,1:16:end)/sqrt(N(1+i)),SupersatPhi_N(i*3+1,1:16:end),'Color',[0 0.4470 0.7410]+(i)*([0.8500, 0.3250, 0.0980]-[0 0.4470 0.7410])/21,'LineWidth',1.5)
end
% for i=0:0.1:2.1
% Plot_SSN(a,b,n,[2 -12;6 -1],1,[0 0.75 0 3],4e3*10^[i],col_B+(col_R-col_B)*i/2.1)
% end
axis([0 0.75 0 3])
xticks([])
yticks([])
box on