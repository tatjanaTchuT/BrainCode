
load('Allen_Sim.mat')
load('Allen_SSN.mat')
figure
plot(AllenData(3:end,1)*2,movmean(AllenData(3:end,4),5),'LineWidth',2)
hold on
plot(AllenData(3:end,1)*2,movmean(AllenData(3:end,5),5),'LineWidth',2)

plot(Allen_SSN(3,:),Allen_SSN(1,:),'k--','LineWidth',2);
plot(Allen_SSN(3,:),Allen_SSN(2,:),'k--','LineWidth',2);

set(gca,'FontSize',16)
ylabel('Firing rate \nu (Hz)','FontSize',16)
xlabel('External input \mu_{ext}','FontSize',16)
axis([0 100 0 8])
text(15,1.75, 'r=1','FontSize',16)
%%
axes('YAxisLocation','right','Position',[.207 .609 .3 .3])
hold on

plot(Allenr150Data(3:end,1)*2,movmean(Allenr150Data(3:end,4),5),'LineWidth',2)
hold on
plot(Allenr150Data(3:end,1)*2,movmean(Allenr150Data(3:end,5),5),'LineWidth',2)

plot(Allen66_SSN(3,:),Allen66_SSN(1,:),'k--','LineWidth',2);
plot(Allen66_SSN(3,:),Allen66_SSN(2,:),'k--','LineWidth',2);

% an=annotation('textbox',[.209 .533 .32 .075],'String','\mu_{ext I} = 1.5 \mu_{ext E}','FontWeight','normal','EdgeColor','none');
% an.FontSize = 14;
text(10,7, 'r=1.5','FontSize',16)

axis([0 100 0 8])
xticks([0 100])
yticks([0 8])
set(gca,'FontSize',10)
box on