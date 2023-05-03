load('CorrelationData.mat')
load('Supersaturation_Sim.mat')
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

figure
h1=histogram(S2ae.*R2ae,20,'Normalization','probability','BinWidth',0.05);
hold on
h2=histogram(S2ai.*R2ai,20,'Normalization','probability','BinWidth',0.05);


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

axes('XAxisLocation','bottom','Position',[.572 .775 .3 .3/2],'Box','on')
hold on
histogram(R2ae,'Normalization','probability','BinWidth',0.125)
yticks([])
xticks([2 3])
xlim([2 3])
axes('Position',[.572 .55 .3 .3/2],'Box','on')
histogram(R2ai,'Normalization','probability','BinWidth',0.1,'FaceColor',[0.85 0.33 0.1])
yticks([])
xticks([0.5 1.0])
xlim([0.4 1.1])
an=annotation('textbox',[.616 .445 .21264 .074502],'String','Firing rate [Hz]','FontWeight','normal','EdgeColor','none');
an.FontSize = 12;

%print -depsc -tiff -r300 -painters Fig2D.eps


%%
figure
boxplot([-mean(Irec01(end-100:end,:))./(mean(Irec00(end-100:end,:))+mean(Iffd0(end-100:end,:)));-mean(Irec11(end-100:end,:))./(mean(Irec10(end-100:end,:))+mean(Iffd1(end-100:end,:)))]','Labels',{'E','I'},'Notch','on','Widths',0.6,'symbol','k.','Color',col_B)

xlabel('Population','FontSize',16)%xlabel('\rho (\mu_{E},\mu_{I})')
ylabel('Balance Factor I/E','FontSize',16)
set(gca,'FontSize',16)

yticks([0 0.2 0.4 0.6])
ylim([0 0.6])

axes('Position',[.605 .626 .3 .3],'Box','on')
hold on
L1=movmean(Supersaturation_Sim(:,6),20);
l1=movmean(Supersaturation_Sim(:,4),20);
l2=movmean(Supersaturation_Sim(:,5),20);
plot(L1(1:20:end),l1(1:20:end),'LineWidth',1.5)
plot(L1(1:20:end),l2(1:20:end),'LineWidth',1.5)
axis([0 50 0 3.5])
xticks([])
yticks([])
plot([18 18 20 20 18],[2.4 2.7 2.7 2.4 2.4],'k','LineWidth',1.5)
plot([18 18 20 20 18],[0.65 0.95 0.95 0.65 0.65],'k','LineWidth',1.5)
