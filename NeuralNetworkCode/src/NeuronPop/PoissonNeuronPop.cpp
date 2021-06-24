#include "PoissonNeuronPop.hpp"


void PoissonNeuronPop::advect(std::vector<double> * synaptic_dV)
{
    // double dt           = info->dt;

    ClearSpiker();

    //#pragma omp parallel for
    for(unsigned long i = 0 ; i < noNeurons; i++)
    {
        // set target rate
        r_target = synaptic_dV->at(i);
        //double lambda 	= r_target*dt;
        double lambda 	= r_target;

        //check spiking
        if (uni_distribution(generator) < lambda)
        spiker.push_back(i);
    }

}

void PoissonNeuronPop::LoadParameters(std::vector<std::string> *input){

    NeuronPop::LoadParameters(input);

    /*std::string              name,token;
    std::vector<std::string> values;

    for(std::vector<std::string>::iterator it = (*input).begin(); it != (*input).end(); ++it) {
        SplitString(&(*it),&name,&values);
        if(name.find("r_target") != std::string::npos){
            r_target = std::stoi(values.at(0));
        }
    }*/

    if(info->globalSeed != -1){
        std::uniform_int_distribution<int> distribution(0,INT32_MAX);
        seed = distribution(info->globalGenerator);
        generator = std::default_random_engine(seed);
    }

}


void PoissonNeuronPop::SaveParameters(std::ofstream * stream){

    std::string id = "neurons_" + std::to_string(GetId());

    //std::cout<<"printing neuron id: "<<id<<"\n";

    //NeuronPop::SaveParameters(stream);
    *stream <<  "#***********************************************\n";
    *stream <<  id + "_noNeurons                   " << noNeurons << "\n";
    *stream <<  id + "_type                        " << GetType() << "\n";
	if (info->globalSeed == -1) {
		*stream << id + "_seedPoisson                 " << std::to_string(seed) << "\n";
	}
    //*stream <<  id + "_r_target                   " << std::to_string(r_target)  << "\n";
    *stream <<  "#\t\tPoisson neuron: produces Poisson spiking with rate r_target (defined under stimulus) \n";

}
