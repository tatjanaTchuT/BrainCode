function Map_Conditions(J,lim)

Jee=J(1,1);
Jei=-J(1,2);
Jie=J(2,1);
Jii=-J(2,2);
a=[1.079e-04,2.210e-06];b=[-11.14,4.77];n=[3.081,3.815];
ne=n(1); ni=n(2); ae=a(1); ai=a(2); be=b(1); bi = b(2);
figure

%% ISN
ze=(ae*Jee*ne)^(-1/(ne-1));
fze=@(mue,r) Jee*ae*ze.^ne-Jei*ai*max(0,(ae*det(J)/Jei*ze.^ne+Jii/Jei.*ze+mue.*(r-Jii/Jei)+Jii/Jei*be-bi)).^ni-ze+mue-be;
fimplicit(fze,lim,'MeshDensity',500,'Color',[0,0.447,0.7],'LineWidth',2)

%% Supersat
hold on
zi=@(r) max(0,(ai*ni*(r*Jei-Jii))).^(-1/(ni-1));
fzi=@(mue,r) -Jii*ai*zi(r).^ni+Jie*ae*max(0,(-ai*det(J)/Jie.*zi(r).^ni+Jee/Jie.*zi(r)+mue.*(1-Jee/Jie.*r)+Jee/Jie*bi-be)).^ne-zi(r)+r.*mue-bi;
fimplicit(fzi,lim,'MeshDensity',5000,'Color',[0.85,0.325,0.098],'LineWidth',2)


%% Bend
hold on

S=Continuation_Bend(J,0.001,1e5,lim);
plot(S(1,:),S(2,:),'Color',[0, 0.6863, 0.1961],'LineWidth',2)

axis(lim)

%% BS existence
if det(J)<0
    plot([0 lim(2)], max(Jii/Jei,Jie/Jee)*[1 1],'k','LineWidth',2);
else
    plot([0 lim(2)], min(Jii/Jei,Jie/Jee)*[1 1],'k','LineWidth',2);
end

xlabel('\mu_{ext}','FontSize',16)
ylabel('r','FontSize',16)
set(gca,'FontSize',16)
end