
figure
load('AllenISN_Sim.mat')
load('AllenISN_SSN.mat')
%%
plot(AllenISNData(5:end,1),movmean(AllenISNData(5:end,4),25),'LineWidth',2)
hold on
plot(AllenISNData(5:end,1),movmean(AllenISNData(5:end,5),25),'LineWidth',2)
set(gca,'FontSize',16)
ylabel('Firing rate \nu (Hz)','FontSize',16)
xlabel('Inhibitory external input \mu_{ext I}','FontSize',16)
plot([50 50],[0 10],'k:','LineWidth',1.5)

plot([50 100 100 50 50],[7.9 7.9 8.2 8.2 7.9],'k','LineWidth',0.25)
xticks([0 25 50 75 100])
yticks([0 2 4 6 8 10])

plot(AllenISN1_SSN(3,:)/2,AllenISN1_SSN(1,:),'k--','LineWidth',2)
plot(AllenISN1_SSN(3,:)/2,AllenISN1_SSN(2,:),'k--','LineWidth',2)

plot(AllenISN2_SSN(3,:),AllenISN2_SSN(1,:),'k--','LineWidth',2)
plot(AllenISN2_SSN(3,:),AllenISN2_SSN(2,:),'k--','LineWidth',2)


axis([0 100 0 10])

axes('YAxisLocation','right','Position',[.58 .56 .3 .15])
plot(AllenISNData(5:end,1),movmean(AllenISNData(5:end,5),25),'Color',[0.85,0.325,0.098],'LineWidth',2)
axis([50 100 7.9 8.2])
yticks([7.9 8.2])
xticks([50 75 100])

an1=annotation('textbox',[.2 .85 .32 .075],'String','\mu_{ext E} = 2 \mu_{ext I}','FontWeight','normal','EdgeColor','none');
an1.FontSize = 16;

an2=annotation('textbox',[.57 .85 .25 .075],'String','\mu_{ext E} = 100','FontWeight','normal','EdgeColor','none');
an2.FontSize = 16;

an3=annotation('textbox',[.2818 .75 .32 .075],'String','r = 0.5','FontWeight','normal','EdgeColor','none');
an3.FontSize = 16;


