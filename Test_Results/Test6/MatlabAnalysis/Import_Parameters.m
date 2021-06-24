function params = Import_Parameters(filename)

    fid = fopen(filename);
    tline = fgetl(fid);
    while ischar(tline)
        
        tline = fgetl(fid);
        
        if(~ischar(tline))
            break;
        end
        arr = split(tline);
        if(strcmp('neurons_0_noNeurons',arr{1}))
            disp(tline)
            N_E        = str2double(arr{2});        
            params.N_E = N_E;
        elseif(strcmp('neurons_1_noNeurons',arr{1}))
            disp(tline)
            N_I        = str2double(arr{2});
            params.N_I = N_I;
        elseif(strcmp('stimulus_meanCurrent',arr{1}))
            params.mu_E_ext = str2double(arr{2});        
            params.mu_I_ext = str2double(arr{3});        
        elseif(strcmp('stimulus_sigmaCurrent',arr{1}))
            params.mu_E_ext_sigma = str2double(arr{2});
            params.mu_I_ext_sigma = str2double(arr{3});            
        elseif(strcmp('dt',arr{1}))
            params.dt = str2double(arr{2});
        elseif(strcmp('synapses_0_0_J',arr{1}))
            params.J_EE = str2double(arr{2});        
        elseif(strcmp('synapses_0_1_J',arr{1}))
            params.J_EI = str2double(arr{2});
        elseif(strcmp('synapses_1_0_J',arr{1}))
            params.J_IE = str2double(arr{2});
        elseif(strcmp('synapses_1_1_J',arr{1}))
            params.J_II = str2double(arr{2}); 
        elseif(strcmp('neurons_0_vReset',arr{1}))
            params.V_R = str2double(arr{2});        
        elseif(strcmp('neurons_0_tauM',arr{1}))
            params.tau_m = str2double(arr{2});                   
        elseif(strcmp('synapses_0_0_connectivity_ConnectionProba',arr{1}))    
            params.conn_0_0 = str2double(arr{2});
        end
    end
    fclose(fid);
    
    params.N = N_E + N_I;



end
