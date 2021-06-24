
#include "LIFNeuronPop.hpp"


void LIFNeuronPop::advect(std::vector<double> * synaptic_dV)
{
    double dt           = info->dt;
    double expdt        = exp(-dt/tau_m);

    ClearSpiker();

    //#pragma omp parallel for
    for(unsigned long i = 0 ; i < noNeurons; i++)
    {
        //(a) wait for refractory period
        if((info->time_step-previous_spike_step[i]) <= refractorySteps)
            continue;

        //(b) advect
        potential[i] = potential[i]*expdt + synaptic_dV->at(i);

        //(c) determine if neuron has spiked
        if(potential[i] > v_thresh)
        {
            spiker.push_back(i);

            //Reset potential
            if(reset_type == 0)
                potential[i] = v_reset;
            else if(reset_type == 1){
                while(potential[i] > v_thresh)
                    potential[i] = v_reset + (potential[i] - v_thresh);
            }

        }
    }

}

void LIFNeuronPop::LoadParameters(std::vector<std::string> *input){

    NeuronPop::LoadParameters(input);

    std::string              name,token;
    std::vector<std::string> values;

    for(std::vector<std::string>::iterator it = (*input).begin(); it != (*input).end(); ++it) {
        SplitString(&(*it),&name,&values);
        if(name.find("resetType") != std::string::npos){
            reset_type = std::stoi(values.at(0));
        }
    }

}


void LIFNeuronPop::SaveParameters(std::ofstream * stream){

    std::string id = "neurons_" + std::to_string(GetId());

    NeuronPop::SaveParameters(stream);
    *stream <<  id + "_resetType                   " << std::to_string(reset_type)  << "\n";
    *stream <<  "#\t\tLIF neuron: dV/dt = -V/tau_m + RI/tau_m \n";
    *stream <<  "#\t\tresetType 0: v = v_reset\n";
    *stream <<  "#\t\tresetType 1: v = v_reset + (v - v_thresh) \n";

}
