
#include "WhiteNoiseRescaled.hpp"

WhiteNoiseRescaled::WhiteNoiseRescaled(NeuronPopSample *neur,std::vector<std::string> *input,GlobalSimInfo  * info):WhiteNoiseStimulus(neur,input,info){

}

void WhiteNoiseRescaled::SaveParameters(std::ofstream * stream){

    WhiteNoiseStimulus::SaveParameters(stream);

    *stream <<  "#\t\tmeanCurrent_i is rescaled with N^(-scalingSynapticStrength), if scaling_C_N = 1.\n";
}
