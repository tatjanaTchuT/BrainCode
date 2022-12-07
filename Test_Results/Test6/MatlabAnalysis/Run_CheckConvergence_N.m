
close all; 

data = {};
clear('fullData');

%***********************************
% Import data
%***********************************
%TODO: Set here folder in which Test6_.. folders are located
base_folder = '../';
%***********************************

folders = dir(base_folder);
data    = {};

for i = 1:length(folders)
    name       = (folders(i).name);
    if((length(name) >= 4) && (strcmp(name(1:4),'Test')))
        dat_name = dir(fullfile([folders(i).folder filesep name], '*.dat'));        
        fullname = [dat_name.folder filesep dat_name.name];
        disp(['Loading ',fullname,'..']);
        data{end+1} = Import_DataFile(fullname);
    end    
end

%***********************************
% Plotting
%***********************************

for i = 1:length(data)
    fullData.N(i)     = data{i}.params.N;
    fullData.nu_E(i)  = data{i}.nu{1}(end);
    fullData.nu_I(i)  = data{i}.nu{2}(end);
    
    fullData.mu_E(i)  = data{i}.mu{1}(end);
    fullData.mu_I(i)  = data{i}.mu{2}(end);
    
    fullData.sigma_E(i)  = data{i}.sigma{1}(end);
    fullData.sigma_I(i)  = data{i}.sigma{2}(end);
    
    fullData.nu_E_Eq3(i) = Phi(data{i}.mu{1}(end),data{i}.sigma{1}(end),0.0,0.01); 
    fullData.nu_I_Eq3(i) = Phi(data{i}.mu{2}(end),data{i}.sigma{2}(end),0.0,0.01); 
        
    [fullData.nu_E_finite(i),fullData.nu_I_finite(i),...
	fullData.mu_E_finite(i),fullData.mu_I_finite(i),...
	fullData.sigma_E_finite(i),fullData.sigma_I_finite(i)] = ComputeSteadyStates(data{i}.params);    
end

[nu_E_balanced,nu_I_balanced,...
          mu_E_balanced,mu_I_balanced,...
          sigma_E_balanced,sigma_I_balanced] = ComputeBalancedState(data{end}.params);

O = ones(size(fullData.N));
      

%***********************************
% Plotting
%***********************************
%%
figure();

color_1 = 'r';
color_2 = 'b';

subplot(1,3,1);

l_exc_data = plot(fullData.N,fullData.nu_E,'o','color',color_1); hold on;
l_inh_data = plot(fullData.N,fullData.nu_I,'o','color',color_2); hold on;

l_exc = plot(fullData.N,fullData.nu_E,'o','color',color_1); hold on;
l_inh = plot(fullData.N,fullData.nu_I,'o','color',color_2); hold on;


l_exc_finite = plot(fullData.N,fullData.nu_E_finite,'--','color',color_1); hold on;
plot(fullData.N,fullData.nu_I_finite,'--','color',color_2); hold on;

l_exc_balanced = plot(fullData.N,nu_E_balanced*O,'-.','color',color_1); hold on;
plot(fullData.N,nu_I_balanced*O,'-.','color',color_2); hold on;

xlabel('N');
ylabel('Firing rate (Hz)','Interpreter','Latex');

subplot(1,3,2);
plot(fullData.N,fullData.mu_E,'o','color',color_1); hold on;
plot(fullData.N,fullData.mu_I,'o','color',color_2); hold on;

plot(fullData.N,fullData.mu_E_finite,'--','color',color_1); hold on;
plot(fullData.N,fullData.mu_I_finite,'--','color',color_2); hold on;

plot(fullData.N,mu_E_balanced*O,'-.','color',color_1); hold on;
plot(fullData.N,mu_I_balanced*O,'-.','color',color_2); hold on;

xlabel('N');
ylabel('Current $\mu$','Interpreter','Latex');

subplot(1,3,3);
plot(fullData.N,fullData.sigma_E,'o','color',color_1); hold on;
plot(fullData.N,fullData.sigma_I,'o','color',color_2); hold on;

plot(fullData.N,fullData.sigma_E_finite,'--','color',color_1); hold on;
plot(fullData.N,fullData.sigma_I_finite,'--','color',color_2); hold on;

plot(fullData.N,sigma_E_balanced*O,'-.','color',color_1); hold on;
plot(fullData.N,sigma_I_balanced*O,'-.','color',color_2); hold on;

xlabel('N');
ylabel('$\sigma$','Interpreter','Latex');

% ax_legend = axes('Position', [0.25 0.9 0.5 0.1]);
ax_legend = axes('Position', [0.73 0.727 0.081 0.18]);
axes(ax_legend);
leg = legend(ax_legend, [l_exc,l_inh,l_exc_data,l_exc_finite,l_exc_balanced],...
            {'exc','inh','simulation','finite','balanced'},'Box','off');
leg.Location = 'northeastoutside';
axis off;

print(gcf,['N_convergence.pdf'],'-dpdf','-bestfit');

