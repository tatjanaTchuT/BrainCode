
#include "Stimulus.hpp"

Stimulus::Stimulus(NeuronPopSample * neur,GlobalSimInfo  * info)
{
    this->info    = info;
    this->neurons = neur;
    //int N         = neurons->GetTotalNeurons();
    int P         = neurons->GetTotalPopulations();
    signal_array  = new double*[P];

    for(int p = 0; p < P; p++){
        signal_array[p] = new double[neurons->GetNeuronsPop(p)];
        for(unsigned long i = 0; i < neurons->GetNeuronsPop(p); i++)
            signal_array[p][i] = 0.0;
    }
}

void Stimulus::SaveParameters(std::ofstream * stream){
    *stream <<  "#**************************************************\n";
    *stream <<  "#************** Stimulus Parameters ***************\n";
    *stream <<  "#**************************************************\n";
    *stream <<  "stimulus_type                        " << GetType() << "\n";
}

void Stimulus::Update(std::vector<std::vector<double>> * synaptic_dV){
    for(unsigned int p = 0; p < neurons->GetTotalPopulations(); p++){
		for (unsigned long i = 0; i < neurons->GetNeuronsPop(p); i++)
			synaptic_dV->at(p)[i] = synaptic_dV->at(p)[i] + signal_array[p][i];
    }
}
