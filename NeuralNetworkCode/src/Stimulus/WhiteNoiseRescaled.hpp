#ifndef WhiteNoiseRescaled_hpp
#define WhiteNoiseRescaled_hpp

#include <stdio.h>

#include <iostream>
#include <vector>
#include <random>
#include <limits.h>
#include "WhiteNoiseStimulus.hpp"

class WhiteNoiseRescaled : public WhiteNoiseStimulus
{

public:
    WhiteNoiseRescaled(NeuronPopSample * neur,std::vector<std::string> *input,GlobalSimInfo  * info);
    virtual ~WhiteNoiseRescaled(){}

    std::string GetType(){return str_whitenoiseRescaled;}
    void SaveParameters(std::ofstream * stream);

    double GetScaling(int pop){
        if(info->networkScaling_extern == 1){
                return (neurons->GetTotalNeurons());}
        else{// if(info->networkScaling_extern == 0){
            std::cout << "external Scaling not well defined!";
            std::terminate();
            return 0;}
    };

};

#endif // WhiteNoiseRescaled_hpp
