//
// Created by Saif Ahmed on 20.09.21.
//

#include "HeteroPoissonNeuronPop.hpp"

HeteroPoissonNeuronPop::HeteroPoissonNeuronPop(GlobalSimInfo *info, int id): HeteroNeuronPop(info, id) {
    r_target = 0;
    seed = 2;
    generator = std::default_random_engine(seed);
    uni_distribution = std::uniform_real_distribution<double>(0.0,1.0);
}

void HeteroPoissonNeuronPop::SaveParameters(std::ofstream * stream) {
    HeteroNeuronPop::SaveParameters(stream);

    // TODO: additional save operations
}

void HeteroPoissonNeuronPop::LoadParameters(std::vector<std::string> *input) {
    HeteroNeuronPop::LoadParameters(input);

    if(info->globalSeed != -1){
        std::uniform_int_distribution<int> distribution(0,INT32_MAX);
        seed = distribution(info->globalGenerator);
        generator = std::default_random_engine(seed);
    }
}

void HeteroPoissonNeuronPop::advect(std::vector<double> * synaptic_dV) {
    ClearSpiker();

    //#pragma omp parallel for
    for(unsigned long i = 0 ; i < noNeurons; i++)
    {
        // set target rate
        r_target = synaptic_dV->at(i);
        //double lambda 	= r_target*dt;
        double lambda 	= r_target;

        //check spiking
        if (uni_distribution(generator) < lambda) {
            this->recordSpiker(i);
        }
    }
}

std::string HeteroPoissonNeuronPop::GetType() {
    return str_HeteroPoissonNeuron;
}