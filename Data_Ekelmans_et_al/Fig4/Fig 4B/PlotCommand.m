load('BistabilitySim_tau.mat')
load('BistabilitySSN_tau.mat')
col_B=[0,0.447,0.7];
col_G=[0,0.6863,0.1961];
col_R=[0.85,0.325,0.098];
figure
hold on

plot(Bistabilitytau18Data(:,1)*0.02-0.04*max(0,(Bistabilitytau18Data(:,1)-500)),movmean(Bistabilitytau18Data(:,5),25),'Color',col_R,'LineWidth',2);
plot(Bistabilitytau20Data(:,1)*0.02-0.04*max(0,(Bistabilitytau20Data(:,1)-500)),movmean(Bistabilitytau20Data(:,5),25),'Color',col_R*0.8,'LineWidth',2);
plot(Bistabilitytau22Data(:,1)*0.02-0.04*max(0,(Bistabilitytau22Data(:,1)-500)),movmean(Bistabilitytau22Data(:,5),25),'Color',col_R*0.6,'LineWidth',2);

plot1=plot(Bistabilitytau18Data(:,1)*0.02-0.04*max(0,(Bistabilitytau18Data(:,1)-500)),movmean(Bistabilitytau18Data(:,4),25),'Color',col_B,'LineWidth',2);
plot2=plot(Bistabilitytau20Data(:,1)*0.02-0.04*max(0,(Bistabilitytau20Data(:,1)-500)),movmean(Bistabilitytau20Data(:,4),25),'Color',col_B*0.8,'LineWidth',2);
plot3=plot(Bistabilitytau22Data(:,1)*0.02-0.04*max(0,(Bistabilitytau22Data(:,1)-500)),movmean(Bistabilitytau22Data(:,4),25),'Color',col_B*0.6,'LineWidth',2);


plot4=plot(Bista_SSN_18ms(3,:),Bista_SSN_18ms(1,:),'k--','LineWidth',2.5);
plot(Bista_SSN_20ms(3,:),Bista_SSN_20ms(1,:),'k--','LineWidth',2.5);
plot(Bista_SSN_22ms(3,:),Bista_SSN_22ms(1,:),'k--','LineWidth',2.5);

plot(Bista_SSN_18ms(3,:),Bista_SSN_18ms(2,:),'k--','LineWidth',2);
plot(Bista_SSN_20ms(3,:),Bista_SSN_20ms(2,:),'k--','LineWidth',2);
plot(Bista_SSN_22ms(3,:),Bista_SSN_22ms(2,:),'k--','LineWidth',2);

text(7.85,4.4,'\tau = 18 ms','FontSize',16,'Color',col_B)
text(4.67,6.1,'\tau = 20 ms','FontSize',16,'Color',col_B*0.8)
text(0.8,9.7,'\tau = 22 ms','FontSize',16,'Color',col_B*0.6)


xlabel('External input \mu_{ext}','FontSize',16)
ylabel('Firing rate \nu (Hz)','FontSize',16)
set(gca,'FontSize',16)
axis([0 10 0 12])
box on
legend boxoff