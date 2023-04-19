% tic
load('Data4e3.mat')
A4e3=corr(Irec00(:,:),Irec01(:,:));
B4e3=corr(Irec10(:,:),Irec11(:,:));
load('Data1e4.mat')
A1e4=corr(Irec00(:,:),Irec01(:,:));
B1e4=corr(Irec10(:,:),Irec11(:,:));
load('Data2e4.mat')
A2e4=corr(Irec00(:,:),Irec01(:,:));
B2e4=corr(Irec10(:,:),Irec11(:,:));
load('Data4e4.mat')
A4e4=corr(Irec00(:,:),Irec01(:,:));
B4e4=corr(Irec10(:,:),Irec11(:,:));
% toc
col_B=[0 0.447 0.741];
col_R=[0.85 0.325 0.098];
a=[1.47e-2 5.04e-4];b=[0.34 17.85];n=[1.925 2.635];
J=[5 10 7 11];
s=-inv([5 -10;7 -11])*[1;1];
%%
% 
% figure
% 
% 
% Ix=[0 4 7 10];
% 
% bs=0.015;
% hold on
% 
% histogram(A4e3(triu(A4e3)~=0),'Normalization','probability','BinWidth',bs,'FaceColor',[0 0.4470 0.7410])
% histogram(A1e4(triu(A1e4)~=0),'Normalization','probability','BinWidth',bs,'FaceColor',[0 0.4470 0.7410]*.8)
% histogram(A2e4(triu(A2e4)~=0),'Normalization','probability','BinWidth',bs,'FaceColor',[0 0.4470 0.7410]*.6)
% histogram(A4e4(triu(A4e4)~=0),'Normalization','probability','BinWidth',bs,'FaceColor',[0 0.4470 0.7410]*.4)
% 
% histogram(B4e3(triu(B4e3)~=0),'Normalization','probability','BinWidth',bs,'FaceColor',[0.8500, 0.3250, 0.0980])
% histogram(B1e4(triu(B1e4)~=0),'Normalization','probability','BinWidth',bs,'FaceColor',[0.8500, 0.3250, 0.0980]*.8)
% histogram(B2e4(triu(B2e4)~=0),'Normalization','probability','BinWidth',bs,'FaceColor',[0.8500, 0.3250, 0.0980]*.6)
% histogram(B4e4(triu(B4e4)~=0),'Normalization','probability','BinWidth',bs,'FaceColor',[0.8500, 0.3250, 0.0980]*.4)
% xlim([-0.9 0.0])
% xlabel('Recurrent input correlation \rho_{E,I}','FontSize',16)%xlabel('\rho (\mu_{E},\mu_{I})')
% ylabel('Frequency','FontSize',16)
% set(gca,'FontSize',16)
% yticks([])
% ylim([0 0.45])
% 
% box on
% 
% a=[1.47e-2 5.04e-4];b=[0.34 17.85];n=[1.925 2.635];
% J=[5 10 7 11];
% s=-inv([5 -10;7 -11])*[1;1];
% 
% 
% 
% 
% axes('Position',[.711 .732 .195 .19375],'Box','on')
% hold on
% 
% Plot_SSN(a,b,n,J,1,[-20 250 0 20],1)
% xticks([])
% yticks([])
% set(gca,'FontSize',12)
% plot(14.72*[1 1 1 1 1]+ 5*[-1 1 1 -1 -1],10*[1 1 1 1 1]+[-1 -1 1 1 -1]*1,'k','LineWidth',2)
% plot([-20 250],10*[1 1],'k--','LineWidth',0.5)
% plot(14.72*[1 1 1 1 1]+ 5*[-1 1 1 -1 -1],3.5*[1 1 1 1 1]+[-1 -1 1 1 -1]*1,'k','LineWidth',2)
% yticks([])
% plot([0 0],[0 1.5],'k','LineWidth',1.5)
% plot([0 250],[0 s(1)*250],'k:','LineWidth',1)%%
% axis([-20 250 0 20])
% 
% 
% axes('Position',[.518 .732 .195 .19375],'Box','on')
% hold on
% Plot_SSN(a,b,n,J*sqrt(2.5),1,[-20 250 0 20],.8)
% xticks([])
% yticks([])
% set(gca,'FontSize',12)
% plot(41.9*[1 1 1 1 1]+ 5*[-1 1 1 -1 -1],10*[1 1 1 1 1]+[-1 -1 1 1 -1]*1,'k','LineWidth',2)
% plot([-20 250],10*[1 1],'k--','LineWidth',0.5)
% plot(41.9*[1 1 1 1 1]+ 5*[-1 1 1 -1 -1],5.75*[1 1 1 1 1]+[-1 -1 1 1 -1]*1,'k','LineWidth',2)
% yticks([])
% plot([0 0],[0 1.5],'k','LineWidth',1.5)
% plot([0 250],[0 s(1)*250/sqrt(2.5)],'k:','LineWidth',1)%%
% axis([-20 250 0 20])
% 
% 
% axes('Position',[.324 .732 .195 .19375],'Box','on')
% hold on
% Plot_SSN(a,b,n,J*sqrt(5),1,[-20 250 0 20],.6)
% xticks([])
% yticks([])
% set(gca,'FontSize',12)
% plot(95.43*[1 1 1 1 1]+ 5*[-1 1 1 -1 -1],10*[1 1 1 1 1]+[-1 -1 1 1 -1]*1,'k','LineWidth',2)
% plot([-20 250],10*[1 1],'k--','LineWidth',0.5)
% plot(95.43*[1 1 1 1 1]+ 5*[-1 1 1 -1 -1],7.91*[1 1 1 1 1]+[-1 -1 1 1 -1]*1,'k','LineWidth',2)
% yticks([])
% plot([0 0],[0 1.5],'k','LineWidth',1.5)
% plot([0 250],[0 s(1)*250/sqrt(5)],'k:','LineWidth',1)%%
% axis([-20 250 0 20])
% 
% 
% axes('Position',[.13 .732 .195 .19375],'Box','on')
% hold on
% Plot_SSN(a,b,n,J*sqrt(10),1,[-20 250 0 20],.4)
% xticks([])
% yticks([])
% set(gca,'FontSize',12)
% plot(194*[1 1 1 1 1]+ 5*[-1 1 1 -1 -1],10*[1 1 1 1 1]+[-1 -1 1 1 -1]*1,'k','LineWidth',2)
% plot([-20 250],10*[1 1],'k--','LineWidth',0.5)
% plot(194*[1 1 1 1 1]+ 5*[-1 1 1 -1 -1],10.2*[1 1 1 1 1]+[-1 -1 1 1 -1]*1,'k','LineWidth',2)
% yticks([])
% plot([0 0],[0 1.5],'k','LineWidth',1.5)
% plot([0 250],[0 s(1)*250/sqrt(10)],'k:','LineWidth',1)%%
% axis([-20 250 0 20])
% 
% 
% an=annotation('textbox',[.70 .425 .14464 .074502],'String','N=4000','FontWeight','normal','EdgeColor','none');
% an.FontSize = 12;
% an=annotation('textbox',[.55 .475 .14464 .074502],'String','N=10000','FontWeight','normal','EdgeColor','none');
% an.FontSize = 12;
% an=annotation('textbox',[.36 .525 .14464 .074502],'String','N=20000','FontWeight','normal','EdgeColor','none');
% an.FontSize = 12;
% an=annotation('textbox',[0.215 0.65 .14464 .074502],'String','N=40000','FontWeight','normal','EdgeColor','none');
% an.FontSize = 12;

% print -depsc -tiff -r300 -painters FigS3.eps

%%
figure
load('Data4e3.mat')
BFE4e3=-mean(Irec01(2:end,:))./(mean(Irec00(2:end,:))+mean(Iffd0(2:end,:)));
BFI4e3=-mean(Irec11(2:end,:))./(mean(Irec10(2:end,:))+mean(Iffd1(2:end,:)));
load('Data1e4.mat')
BFE1e4=-mean(Irec01(2:end,:))./(mean(Irec00(2:end,:))+mean(Iffd0(2:end,:)));
BFI1e4=-mean(Irec11(2:end,:))./(mean(Irec10(2:end,:))+mean(Iffd1(2:end,:)));
load('Data2e4.mat')
BFE2e4=-mean(Irec01(2:end,:))./(mean(Irec00(2:end,:))+mean(Iffd0(2:end,:)));
BFI2e4=-mean(Irec11(2:end,:))./(mean(Irec10(2:end,:))+mean(Iffd1(2:end,:)));
load('Data4e4.mat')
BFE4e4=-mean(Irec01(2:end,:))./(mean(Irec00(2:end,:))+mean(Iffd0(2:end,:)));
BFI4e4=-mean(Irec11(2:end,:))./(mean(Irec10(2:end,:))+mean(Iffd1(2:end,:)));


boxplot([BFE4e3;BFI4e3;BFE1e4;BFI1e4;BFE2e4;BFI2e4;BFE4e4;BFI4e4]','Labels',{'4000','','10000','','20000','','40000',''},'Notch','on','Widths',0.6,'symbol','k.','Color',col_B)
ylim([0 1.31])
yticks([0 0.2 0.4 0.6 0.8 1])
%xticks([])
ylabel('Balance Factor I/E')
xlabel('N')
set(gca,'FontSize',16)

axes('Position',[.711 .732 .195 .19375],'Box','on')
hold on
Plot_SSN(a,b,n,J*sqrt(10),1,[-20 250 0 20],.4)
xticks([])
yticks([])
set(gca,'FontSize',12)
plot(194*[1 1 1 1 1]+ 5*[-1 1 1 -1 -1],10*[1 1 1 1 1]+[-1 -1 1 1 -1]*1,'k','LineWidth',2)
plot([-20 250],10*[1 1],'k--','LineWidth',0.5)
plot(194*[1 1 1 1 1]+ 5*[-1 1 1 -1 -1],10.2*[1 1 1 1 1]+[-1 -1 1 1 -1]*1,'k','LineWidth',2)
yticks([])
plot([0 0],[0 1.5],'k','LineWidth',1.5)
plot([0 250],[0 s(1)*250/sqrt(10)],'k:','LineWidth',1)%%
axis([-20 250 0 20])


axes('Position',[.518 .732 .195 .19375],'Box','on')
hold on
Plot_SSN(a,b,n,J*sqrt(5),1,[-20 250 0 20],.6)
xticks([])
yticks([])
set(gca,'FontSize',12)
plot(95.43*[1 1 1 1 1]+ 5*[-1 1 1 -1 -1],10*[1 1 1 1 1]+[-1 -1 1 1 -1]*1,'k','LineWidth',2)
plot([-20 250],10*[1 1],'k--','LineWidth',0.5)
plot(95.43*[1 1 1 1 1]+ 5*[-1 1 1 -1 -1],7.91*[1 1 1 1 1]+[-1 -1 1 1 -1]*1,'k','LineWidth',2)
yticks([])
plot([0 0],[0 1.5],'k','LineWidth',1.5)
plot([0 250],[0 s(1)*250/sqrt(5)],'k:','LineWidth',1)%%
axis([-20 250 0 20])


axes('Position',[.324 .732 .195 .19375],'Box','on')
hold on
Plot_SSN(a,b,n,J*sqrt(2.5),1,[-20 250 0 20],.8)
xticks([])
yticks([])
set(gca,'FontSize',12)
plot(41.9*[1 1 1 1 1]+ 5*[-1 1 1 -1 -1],10*[1 1 1 1 1]+[-1 -1 1 1 -1]*1,'k','LineWidth',2)
plot([-20 250],10*[1 1],'k--','LineWidth',0.5)
plot(41.9*[1 1 1 1 1]+ 5*[-1 1 1 -1 -1],5.75*[1 1 1 1 1]+[-1 -1 1 1 -1]*1,'k','LineWidth',2)
yticks([])
plot([0 0],[0 1.5],'k','LineWidth',1.5)
plot([0 250],[0 s(1)*250/sqrt(2.5)],'k:','LineWidth',1)%%
axis([-20 250 0 20])


axes('Position',[.13 .732 .195 .19375],'Box','on')
hold on
Plot_SSN(a,b,n,J,1,[-20 250 0 20],1)
xticks([])
yticks([])
set(gca,'FontSize',12)
plot(14.72*[1 1 1 1 1]+ 5*[-1 1 1 -1 -1],10*[1 1 1 1 1]+[-1 -1 1 1 -1]*1,'k','LineWidth',2)
plot([-20 250],10*[1 1],'k--','LineWidth',0.5)
plot(14.72*[1 1 1 1 1]+ 5*[-1 1 1 -1 -1],3.5*[1 1 1 1 1]+[-1 -1 1 1 -1]*1,'k','LineWidth',2)
yticks([])
plot([0 0],[0 1.5],'k','LineWidth',1.5)
plot([0 250],[0 s(1)*250],'k:','LineWidth',1)%%
axis([-20 250 0 20])


% 
% an=annotation('textbox',[0.215 0.75 .14464 .074502],'String','10Hz','FontWeight','normal','EdgeColor','none');
% an.FontSize = 12;