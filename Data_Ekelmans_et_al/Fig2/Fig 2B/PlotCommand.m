load('Continuation.mat')
load('SimData.mat')
col_B=[0,0.447,0.7];
col_R=[0.85,0.325,0.098];
figure

plot1=plot(Supersaturation1Data(:,1),movmean(Supersaturation1Data(:,4),10),'Color',col_B,'LineWidth',1.5);
hold on
plot2=plot(Supersaturation125Data(:,1),movmean(Supersaturation125Data(:,4),10),'Color',col_B*0.8,'LineWidth',1.5);
plot3=plot(Supersaturation15Data(:,1),movmean(Supersaturation15Data(:,4),10),'Color',col_B*0.6,'LineWidth',1.5);

plot4=plot(Supersat_SSN1(3,:),Supersat_SSN1(1,:),'k--','LineWidth',2.5);
plot(Supersat_SSN125(3,:),Supersat_SSN125(1,:),'k--','LineWidth',2.5);
plot(Supersat_SSN15(3,:),Supersat_SSN15(1,:),'k--','LineWidth',2.5);

plot6=plot(Supersaturation1Data(:,1),movmean(Supersaturation1Data(:,5),10),'Color',col_R,'LineWidth',1.5);
plot(Supersaturation125Data(:,1),movmean(Supersaturation125Data(:,5),10),'Color',col_R*0.8,'LineWidth',1.5);
plot(Supersaturation15Data(:,1),movmean(Supersaturation15Data(:,5),10),'Color',col_R*0.6,'LineWidth',1.5);

plot(Supersat_SSN1(3,:),Supersat_SSN1(2,:),'k--','LineWidth',2.5);
plot(Supersat_SSN125(3,:),Supersat_SSN125(2,:),'k--','LineWidth',2.5);
plot(Supersat_SSN15(3,:),Supersat_SSN15(2,:),'k--','LineWidth',2.5);

xlabel('External input \mu_{ext}','FontSize',16)
ylabel('Firing rate \nu (Hz)','FontSize',16)
set(gca,'FontSize',16)
axis([0 50 0 10])
box on
legend([plot1 plot2 plot3 plot4],{'Simulation \mu_{ext E}=\mu_{ext I}','Simulation \mu_{ext E}=1.25 \mu_{ext I}','Simulation \mu_{ext E}=1.5 \mu_{ext I}','SSN'},'FontSize',15,'Location','northwest')
legend boxoff