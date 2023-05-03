load('Correlation_Data.mat')
load('Bistability_Sim.mat')
col_B=[0 0.447 0.741];
col_R=[0.85 0.325 0.098];


%%

RE={};
RI={};
for i=1:1000
    RE{i}=Rasterplot(Rasterplot(:,3)==0 & Rasterplot(:,2)==3*(i-1),1);
    RI{i}=Rasterplot(Rasterplot(:,3)==1 & Rasterplot(:,2)==i-1,1);
end
for i=1:1000
    RE{i}=RE{i}(2:end)-RE{i}(1:end-1);
    RI{i}=RI{i}(2:end)-RI{i}(1:end-1);
end
R2ae=zeros(1,1000);
R2ai=zeros(1,1000);
for i=1:1000
    R2ae(i)=1/mean(RE{i});
    R2ai(i)=1/mean(RI{i});
    S2ae(i)=std(RE{i});
    S2ai(i)=std(RI{i});
end

%subplot(2,2,2)
figure
h1=histogram(S2ae.*R2ae,'Normalization','probability','BinWidth',0.05);
hold on
h2=histogram(S2ai.*R2ai,'Normalization','probability','BinWidth',0.05);


slice1=ismember(round(h1.BinEdges(1:end-1)*1e4),round(h2.BinEdges(1:end-1)*1e4));%the last edge does not correspond to a new bar
slice2=ismember(round(h2.BinEdges(1:end-1)*1e4),round(h1.BinEdges(1:end-1)*1e4));%somehow I need to round the error to identify matching bin edges
Overlap=min([h1.BinCounts(slice1);h2.BinCounts(slice2)]);
V=[];
Bin_center=h1.BinEdges(slice1)+0.05/2;
for bin=1:length(Bin_center)
    V=[V Bin_center(bin)*ones(1,Overlap(bin))];
end
V=[V 0*ones(1,sum(h1.BinCounts)-sum(Overlap))];
histogram(V,'Normalization','probability','BinWidth',0.05,'FaceAlpha',1,'FaceColor',0.6*col_R+0.4*0.4+0.4*0.6*col_B);

ylim([0 max( [h1.BinCounts/sum(h1.BinCounts) h2.BinCounts/sum(h2.BinCounts)] )*1.1])
yticks([])
ylabel('Frequency','FontSize',16)
xlabel('ISI variability CV_{ISI}','FontSize',16)
set(gca,'FontSize',16)
xlim([0.4 1.5])

%title('B')
axes('XAxisLocation','bottom','Position',[.572 .775 .3 .3/2],'Box','on')
hold on
histogram(R2ae,'Normalization','probability','BinWidth',0.25)
yticks([])
xticks([6 8.5])
xlim([6 8.5])
axes('Position',[.572 .55 .3 .3/2],'Box','on')
histogram(R2ai,'Normalization','probability','BinWidth',0.25,'FaceColor',[0.85 0.33 0.1])
yticks([])
xticks([1 2.5])
xlim([1 2.5])
an=annotation('textbox',[.616 .45952 .21264 .074502],'String','Firing rate [Hz]','FontWeight','normal','EdgeColor','none');
an.FontSize = 12;

%%
figure
boxplot([-mean(Irec01(end-100:end,:))./(mean(Irec00(end-100:end,:))+mean(Iffd0(end-100:end,:)));-mean(Irec11(end-100:end,:))./(mean(Irec10(end-100:end,:))+mean(Iffd1(end-100:end,:)))]','Labels',{'E','I'},'Notch','on','Widths',0.6,'symbol','k.','Color',col_B)

xlabel('Population','FontSize',16)%xlabel('\rho (\mu_{E},\mu_{I})')
ylabel('Balance Factor I/E','FontSize',16)
set(gca,'FontSize',16)

yticks([0 0.25 0.5 0.75])
ylim([0 0.75])

axes('Position',[.605 .626 .3 .3],'Box','on')
hold on
L1=movmean(Bistability_Sim(:,6),500);
l1=movmean(Bistability_Sim(:,4),500);
l2=movmean(Bistability_Sim(:,5),500);
plot(L1(1:20:end),l1(1:20:end),'LineWidth',1.5)
plot(L1(1:20:end),l2(1:20:end),'LineWidth',1.5)
axis([0 10 0 10])
xticks([])
yticks([])
plot([5.5 5.5 6.5 6.5 5.5],[6.75 7.75 7.75 6.75 6.75],'k','LineWidth',1.5)
plot([5.5 5.5 6.5 6.5 5.5],[1.25 2.25 2.25 1.25 1.25],'k','LineWidth',1.5)
% set(gcf,'Position',[680 558 560 420])
% saveas(gcf,?../Foldername/Filename?,?svg')
