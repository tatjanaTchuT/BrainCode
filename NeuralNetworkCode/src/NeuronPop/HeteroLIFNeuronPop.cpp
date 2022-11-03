//
// Created by Saif Ahmed on 07.09.21.
//

#include "HeteroLIFNeuronPop.hpp"

HeteroLIFNeuronPop::HeteroLIFNeuronPop(GlobalSimInfo *info, int id): HeteroNeuronPop(info, id) {
    reset_type = 0;
}

void HeteroLIFNeuronPop::SaveParameters(std::ofstream * stream) {
    HeteroNeuronPop::SaveParameters(stream);

    // TODO: additional save operations
}

void HeteroLIFNeuronPop::LoadParameters(std::vector<std::string> *input) {
    HeteroNeuronPop::LoadParameters(input);

    std::string              name,token;
    std::vector<std::string> values;

    for(auto& it : *input) {
        SplitString(&it,&name,&values);
        if(name.find("resetType") != std::string::npos){
            reset_type = std::stoi(values.at(0));
        }
    }
}

void HeteroLIFNeuronPop::advect(std::vector<double> * synaptic_dV) {
    double dt           = this->info->dt;
    double expdt        = exp(-dt/this->tau_m);

    ClearSpiker();

    for(unsigned long i = 0 ; i < noNeurons; i++)
    {
        //(a) wait for refractory period
        if(((double) this->info->time_step - (double) this->previous_spike_step[i]) <= this->refractorySteps)
            continue;

        //(b) advect
        this->potential[i] = this->potential[i]*expdt + synaptic_dV->at(i);

        //(c) determine if neuron has spiked
        if(this->potential[i] > this->v_thresh) {

            this->recordSpiker(i);

            //Reset potential
            if(this->reset_type == 0)
                this->potential[i] = v_reset;
            else if(this->reset_type == 1){
                while(this->potential[i] > this->v_thresh)
                    this->potential[i] = this->v_reset + (this->potential[i] - this->v_thresh);
            }
        }
    }
}

std::string HeteroLIFNeuronPop::GetType() {
    return str_HeteroLIFNeuron;
}