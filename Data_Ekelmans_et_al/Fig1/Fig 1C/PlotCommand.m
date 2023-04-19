% close all
Tau=(7:0.2:23)*1e-3;%Range of tau and sigma for which the curves will be fitted
Sigma=1:0.05:5;

figure
load('cs.mat')
subplot(3,1,1)
pcolor(1000*Tau,Sigma,log10(cs(:,:,1)))
shading interp


ylim([1 5])
xlim([7.5 22.5])
xticks([])
yticks([1 3 5])
c=colorbar('eastoutside');
title('a')
set(gca,'FontSize',16)
c.FontSize=12;
box on
colormap('hot')
caxis([-8 -1])
set(c,'YTick',[-7 -4 -1])
set(c,'YTickLabel',{'10^{-7}','10^{-4}','10^{-1}'},'FontSize',16)
%------------------------------------------------------------
subplot(3,1,2)
pcolor(1000*Tau,Sigma,cs(:,:,2))
ylabel('\sigma (mV/?s)','FontSize',18)
shading interp

ylim([1 5])
xlim([7.5 22.5])
xticks([])
yticks([1 3 5])
box on
c=colorbar('eastoutside');
title('b')
set(gca,'FontSize',16)
c.FontSize=12;
box on
colormap('hot')
caxis([-65 80])
set(c,'YTick',[-50 0 50],'FontSize',16)
%------------------------------------------------------------
subplot(3,1,3)
pcolor(1000*Tau,Sigma,cs(:,:,3))

shading interp
xlabel('\tau (ms)','FontSize',18)
ylim([1 5])
xlim([7.5 22.5])
xticks([10 15 20 25])
yticks([1 3 5])
c=colorbar('eastoutside');
title('n')
set(gca,'FontSize',16)
c.FontSize=12;
box on
colormap('hot')
caxis([1.9 4.6])
set(c,'YTick',[2 3 4],'FontSize',16)