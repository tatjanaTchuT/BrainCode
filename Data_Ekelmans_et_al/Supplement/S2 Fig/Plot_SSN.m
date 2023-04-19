function Plot_SSN(a,b,n,J,r,axlim,bright)
if length(J(1,:))==4
    J=abs([J(1,1:2);J(1,3:4)]);
end
Jee=abs(J(1,1));
Jei=abs(J(1,2));
Jie=abs(J(2,1));
Jii=abs(J(2,2));
detJ=Jei*Jie-Jee*Jii;
aE=a(1);
bE=b(1);
nE=n(1);
if length(a)>1
    aI=a(2);
else
    aI=aE;
end
if length(b)>1
    bI=b(2);
else
    bI=bE;
end
if length(n)>1
    nI=n(2);
else
    nI=nE;
end

%axlim=[-20 300 0 25];

f = @(x,y) Jee*max(0,y)-Jei*aI*max(0,max(0,y)*detJ/Jei+Jii/Jei*(bE-x+max(0,y/aE).^(1/nE))+x*r-bI).^(nI)+x-bE-max(0,y/aE).^(1/nE);
fimplicit(f,axlim,'Color',[0,0.447,0.7]*bright,'LineWidth',2)
hold on
f = @(x,y) Jie*aE*max(0,-max(0,y)*detJ/Jie+Jee/Jie*(bI-r*x+max(0,y/aI).^(1/nI))+x-bE).^(nE)-Jii*max(0,y)+r*x-bI-max(0,y/aI).^(1/nI);
fimplicit(f,axlim,'Color',[0.85,0.325,0.098]*bright,'LineWidth',2)
%plot([0 0],[0 5],'k:')
end
%% Older version
% if length(J(1,:))==4
%     J=abs([J(1:2);J(3:4)]);
% end
% Jee=abs(J(1,1))*a^(1/n);
% Jei=abs(J(1,2))*a^(1/n);
% Jie=abs(J(2,1))*a^(1/n);
% Jii=abs(J(2,2))*a^(1/n);
% 
% detJ=Jei*Jie-Jee*Jii;
% f = @(x,y) max(0,y).*Jee - Jei*max(0,(max(0,y).*detJ*Jei^(-1) + Jei^(-1)*Jii*max(0,y).^(1/n) -(Jei)^(-1)*Jii*(x-b)*a^(1/n) + (r*x-b)*a^(1/n))).^n-max(0,y).^(1/n)+(x-b)*a^(1/n);
% fimplicit(f,[0 125 0 15],'LineWidth',2)
% 
% hold on
% f = @(x,y) Jie*max(0,-max(0,y).*detJ*Jie^(-1) + Jie^(-1)*Jee*y.^(1/n)+ (x-b)*a^(1/n) - (x*r-b)*(Jie)^(-1)*Jee*a^(1/n)).^n-max(0,y).*Jii-max(0,y).^(1/n)+(x*r-b).*a^(1/n);
% fimplicit(f,[0 125 0 15],'LineWidth',2)
